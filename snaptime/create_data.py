import cPickle as pickle
import os
import sys
from datetime import datetime
from datetime import timedelta
import pandas as pd
import numpy as np
from fillData import *

def create_raw_data(input_directory,output_directory,X_cols,Y_cols,use_pandas=False):
    files = os.listdir(input_directory)
    if use_pandas == True:
        for file in files:
            data = pd.read_csv(os.path.join(input_directory,file),sep='\t',header=None,low_memory=False)[np.concatenate((X_cols,Y_cols)).astype(int)]
            data = data.fillna(method='ffill').fillna(value=0).applymap(lambda x : 1 if x == 't' else 0 if x == 'f' else x)
            data_X = data[X_cols].astype('float').values
            data_Y = data[Y_cols].astype('float').values
            np.save(os.path.join(output_directory,'_X_'+file),data_X)
            np.save(os.path.join(output_directory,'_Y_'+file),data_Y)
    else:
        epoch = datetime.utcfromtimestamp(0)
        obj = FillData()
        X_cols += 1
        Y_cols += 1
        for file in files:
            init_epoch = long((datetime.strptime('_'.join(file.split('_')[:2]),'%Y%m%d_%H') - epoch).total_seconds()*1000)
            data = obj.createAndFillData(os.path.join(input_directory,file),init_epoch,3600000)
            data_X = data[:,X_cols]
            data_Y = data[:,Y_cols]
            np.save(os.path.join(output_directory,'_X_'+file),data_X)
            np.save(os.path.join(output_directory,'_Y_'+file),data_Y)
            np.save(os.path.join(output_directory,'_timestamp_'+file),data[:,0])

def create_LSTM_data(input_directory,output_trainfile,output_testfile,interval,lookahead,y_differentiator,imbalance=10):
    np.random.seed(1)
    data_train = {}
    data_test = {}
    files = os.listdir(input_directory)
    for file in files:
        if '_X_' in file or '_timestamp_' in file:
            continue
        try:
            full_data_Y = np.load(os.path.join(input_directory,file))
            data_timestamp = np.load(os.path.join(input_directory,'_timestamp_'+file[3:]))
            indices = []
            for i in xrange(1,len(data_timestamp)):
                for j in xrange((int(data_timestamp[i])-int(data_timestamp[i-1]))/100):
                    indices.append(i-1)
            indices = np.array(indices)
        except:
            print file,"error"
            continue
        i = 0
        counter = 0
        pos_idx = []
        neg_idx = []
        while i + interval+lookahead - 1 < len(indices):
            pointer = 1
            while pointer <= lookahead and y_differentiator(full_data_Y[indices[i+interval-1+pointer],:]) == False:
                pointer += 1
            if pointer <= lookahead:
                for j in xrange(i,i+pointer):
                    if np.any(map(y_differentiator,full_data_Y[indices[j:j+interval],:])): #j:j+interval,:])):
                        continue
                    pos_idx.append(indices[j:j+interval])
                    #print "positive",[indices[k] for k in xrange(j,j+interval)]
                i += pointer+interval
                while i < len(indices) and y_differentiator(full_data_Y[indices[i]]) == True:
                    i = i + 1
            else:
                neg_idx.append(indices[i:i+interval])
                #print "negative",[indices[k] for k in xrange(i,i+interval)]
                i = i + 1
            #print i,len(pos_idx),len(neg_idx)
        pos_idx = np.array(pos_idx)
        temp_neg_idx = np.array(neg_idx)
        samples = np.arange(len(temp_neg_idx)/(lookahead*10))
        np.random.shuffle(samples)
        neg_idx = []
        for i in samples:
            neg_idx += temp_neg_idx[lookahead*10*i:lookahead*10*(i+1)].tolist()
        neg_idx = np.array(neg_idx)
        np.random.shuffle(neg_idx)
        temp_neg_idx = neg_idx[:imbalance*len(pos_idx)]
        start = imbalance*len(pos_idx)
        np.random.shuffle(pos_idx)
        data_train[file] ={'positive':pos_idx[:int(0.9*len(pos_idx))],'negative':temp_neg_idx[:int(0.9*len(temp_neg_idx))]}
        data_test[file] ={'positive':pos_idx[int(0.9*len(pos_idx)):],'negative':np.concatenate((temp_neg_idx[int(0.9*len(temp_neg_idx)):],neg_idx[start:]))}
    pickle.dump(data_train,open(output_trainfile+'_'+str(interval)+'_'+str(lookahead),'w+'))
    pickle.dump(data_test,open(output_testfile+'_'+str(interval)+'_'+str(lookahead),'w+'))
        
    
