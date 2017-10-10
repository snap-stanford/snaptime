import numpy as np
from datetime import datetime
from sklearn.metrics import roc_curve
from sklearn.metrics import auc
from sklearn.metrics import roc_auc_score
import cPickle as pickle
import time
import os
from snaptime_helper import FillData

def _test_LSTM(input_directory,model,test_indices,sum,sumsq,obj,interval,X_cols,timeslice,granularity,train_indices=None):
    files = os.listdir(input_directory)
    y_true_test = []
    y_pred_test = []
    y_true_train = []
    y_pred_train = []
    files = [file for file in os.listdir(input_directory)]
    epoch = datetime.utcfromtimestamp(0)
    
    for i in xrange(len(files)):
        file = files[i]
        init_epoch = long((datetime.strptime('_'.join(file.split('_')[:2]),'%Y%m%d_%H') - epoch).total_seconds()*1000)
        if file not in test_indices:
            continue
        data = obj.createAndFillData(os.path.join(input_directory,file),init_epoch,timeslice,granularity)[:,X_cols]
        for j in xrange(data.shape[1]):
            if sumsq[j] != 0:
                data[:,j] = (data[:,j]-sum[j])/sumsq[j]
        positives = test_indices[file]['positive']
        negatives = test_indices[file]['negative']
        if len(positives) > 0 and len(negatives) > 0:
            X_test = np.concatenate(([data[idx:idx+interval,:] for idx in positives],[data[idx:idx+interval,:] for idx in negatives]))
        elif len(positives) > 0:
            X_test = np.array([data[idx:idx+interval,:] for idx in positives])
        else:
            X_test = np.array([data[idx:idx+interval,:] for idx in negatives])
        for idx in positives:
            y_true_test.append([0,1])
        for idx in negatives:
            y_true_test.append([1,0])
        y_pred_test += model.predict(X_test).tolist()

    if train_indices != None:
        for i in xrange(len(files)):
            file = files[i]
            init_epoch = long((datetime.strptime('_'.join(file.split('_')[:2]),'%Y%m%d_%H') - epoch).total_seconds()*1000)
            if file not in train_indices:
                continue
            data = obj.createAndFillData(os.path.join(input_directory,file),init_epoch,timeslice,granularity)[:,X_cols]
            for j in xrange(data.shape[1]):
                if sumsq[j] != 0:
                    data[:,j] = (data[:,j]-sum[j])/sumsq[j]
            positives = train_indices[file]['positive']
            negatives = train_indices[file]['negative']
            if len(positives) > 0 and len(negatives) > 0:
                X_test = np.concatenate(([data[idx:idx+interval,:] for idx in positives],[data[idx:idx+interval,:] for idx in negatives]))
            elif len(positives) > 0:
                X_test = np.array([data[idx:idx+interval,:] for idx in positives])
            else:
                X_test = np.array([data[idx:idx+interval,:] for idx in negatives])
            for idx in positives:
                y_true_train.append([0,1])
            for idx in negatives:
                y_true_train.append([1,0])
            y_pred_train += model.predict(X_test).tolist()

    fpr,tpr,thres = roc_curve([y_true_test[i][1] for i in xrange(len(y_true_test))],[y_pred_test[i][1] for i in xrange(len(y_pred_test))])
    test_score =  roc_auc_score([y_true_test[i][1] for i in xrange(len(y_true_test))],[y_pred_test[i][1] for i in xrange(len(y_pred_test))])
    if train_indices != None:
        fpr_t,tpr_t,thres_t = roc_curve([y_true_train[i][1] for i in xrange(len(y_true_train))],[y_pred_train[i][1] for i in xrange(len(y_pred_train))])
        train_score = roc_auc_score([y_true_train[i][1] for i in xrange(len(y_true_train))],[y_pred_train[i][1] for i in xrange(len(y_pred_train))])
    else:
        fpr_t,tpr_t,thres_t,train_score=None,None,None,None,None
    return fpr,tpr,thres,test_score,fpr_t,tpr_t,thres_t,train_score

def _train_LSTM(input_directory,train_indices,buffering,sum,sumsq,LSTM_model,iterations,obj,interval,X_cols,timeslice,granularity):
    global_X = []
    global_Y = []
    global_idx = -1
    global_file_idx = -1
    files = [file for file in os.listdir(input_directory)]
    epoch = datetime.utcfromtimestamp(0)
    def train_generator():
        global_file_idx = -1
        global_idx = -1
        global_X = []
        global_Y = []
        while True:
            #refresh with another file's data
            if global_file_idx == -1 or global_idx>= len(global_X):
                global_idx = 0
                global_X = []
                global_Y = []
                while files[global_file_idx] not in train_indices:
                    global_file_idx = (global_file_idx + 1)%len(files)
                init_epoch = long((datetime.strptime('_'.join(files[global_file_idx].split('_')[:2]),'%Y%m%d_%H') - epoch).total_seconds()*1000)
                positives = train_indices[files[global_file_idx]]['positive']
                negatives = train_indices[files[global_file_idx]]['negative']
                data = obj.createAndFillData(os.path.join(input_directory,files[global_file_idx]),init_epoch,timeslice,granularity)[:,X_cols]
                if len(positives) > 0 and len(negatives) > 0:
                    global_Y = np.concatenate(([[0,1] for idx in positives],[[1,0] for idx in negatives]))
                    global_X = np.concatenate(([data[idx:idx+interval,:] for idx in positives],[data[idx:idx+interval,:] for idx in negatives]))
                elif len(positives) > 0:
                    global_Y = np.array([[0,1] for idx in positives])
                    global_X = np.array([data[idx:idx+interval,:] for idx in positives])
                elif len(negatives) > 0:
                    global_Y = np.array([[1,0] for idx in negatives])
                    global_X = np.array([data[idx:idx+interval,:] for idx in negatives])
                order = np.arange(len(global_X))
                np.random.shuffle(order)
                global_X = global_X[order]
                for j in xrange(global_X.shape[2]):
                    if sumsq[j] != 0:
                        global_X[:,:,j] = (global_X[:,:,j] - sum[j])/sumsq[j]
                global_Y = global_Y[order]
            val = (global_X[global_idx:min(global_idx+buffering,len(global_X))],global_Y[global_idx:min(global_idx+buffering,len(global_Y))])
            global_idx = global_idx + buffering
            yield val

    LSTM_model.fit_generator(train_generator(),buffering,iterations)
    return LSTM_model

def run_LSTM(input_directory,X_cols,Y_cols,interval,lookahead,timeslice,granularity,y_differentiator,buffering,LSTM_model,iterations,imbalance=10,lastval=True):
    """input directory - directory containing files in snaptime format
    X_cols - columns with independent data
    Y_cols - columns with dependent data
    interval - window size
    lookahead - time duration for prediction
    timeslice - total length of timeseries in milliseconds
    granularity - minimum difference between timestamps in milliseconds
    y_differentiator : lambda function for specifying whether a certain y value maps to 1
    buffering : LSTM batch size
    LSTM_model : deep learning model
    iterations : number of training iterations
    imbalance : negative/positive imbalance ratio used in training
    lastval - if True, consider only the last point of the lookahead window for generating an example

    returns:
    fpr - test false positive rate array
    tpr - test true positive rate array
    thres - test roc thresholds array
    test_score - test auc score
    fpr_t - train false positive rate array
    tpr_t - train true positive rate array
    thres_t- train roc threshold array
    train_score - train auc score
    """
    #prepare training and test indices and preprocess the data
    data_train = {}
    data_test = {}
    sum = [0 for i in xrange(len(X_cols))]
    sumsq = [0 for i in xrange(len(X_cols))]
    files = os.listdir(input_directory)
    epoch = datetime.utcfromtimestamp(0)
    obj = FillData()
    total = 0
    for file in files:
        init_epoch = long((datetime.strptime('_'.join(file.split('_')[:2]),'%Y%m%d_%H') - epoch).total_seconds()*1000)
        data = obj.createAndFillData(os.path.join(input_directory,file),init_epoch,timeslice,granularity)
        full_data_Y = data[:,Y_cols]
        full_data_X = data[:,X_cols]
        total += len(full_data_X)
        #preprocess the data
        for j in xrange(full_data_X.shape[1]):
            sum[j] += np.sum(full_data_X[:,j])
        for j in xrange(full_data_X.shape[1]):
            sumsq[j] += np.sum(full_data_X[:,j]-sum[j])**2
        i = 0
        counter = 0
        pos_idx = []
        neg_idx = []
        while i + interval+lookahead - 1 < len(full_data_Y):
            pointer = 1
            while pointer <= lookahead and y_differentiator(full_data_Y[i+interval-1+pointer]) == False:
                pointer += 1
            if pointer <= lookahead:
                for j in xrange(i,i+pointer):
                    if lastval == True and np.any(map(y_differentiator,full_data_Y[j:j+interval,:])):
                        continue
                    pos_idx.append(j)
                i += pointer+interval
                while i < len(full_data_Y) and y_differentiator(full_data_Y[i]) == True:
                    i = i + 1
            else:
                neg_idx.append(i)
                i = i + 1
        pos_idx = np.array(pos_idx)
        if len(pos_idx) == 0:
            continue
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
    sum,sumsq = np.array(sum),np.array(sumsq)
    sum /= total
    sumsq = (sumsq/total - sum**2)**0.5
    #train and test
    model = _train_LSTM(input_directory,data_train,buffering,sum,sumsq,LSTM_model,iterations,obj,interval,X_cols,timeslice,granularity)
    fpr,tpr,thres,test_score,fpr_t,tpr_t,thres_t,train_score = _test_LSTM(input_directory,model,data_test,sum,sumsq,obj,interval,X_cols,timeslice,granularity,data_train)
    return fpr,tpr,thres,test_score,fpr_t,tpr_t,thres_t,train_score

