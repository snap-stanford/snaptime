import numpy as np
np.random.seed(1337)  # for reproducibility

from keras.preprocessing import sequence
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation
from keras.layers.recurrent import LSTM
from keras.regularizers import l1
import tensorflow as tf
from keras.optimizers import Adam
from sklearn.metrics import roc_curve
from sklearn.metrics import auc
from sklearn.metrics import roc_auc_score
import cPickle as pickle
import time
import os
tf.python.control_flow_ops = tf


def preprocess_data(input_directory,X_dim):
    #return mean and stdev for preprocessing during training and test

    global file_count
    sum = [0 for i in xrange(X_dim)]
    sumsq = [0 for i in xrange(X_dim)]
    files = os.listdir(input_directory)
    file_count = len(files)
    total = 0
    for file in files:
        if '_Y_' in file or '_timestamp_' in file:
            continue
        data = np.load(os.path.join(input_directory,file))
        total += len(data)
        for j in xrange(data.shape[1]):
            sum[j] += np.sum(data[:,j])
    sum  = np.array(sum)
    sum /= float(total)
    for file in files:
        if '_Y_' in file or '_timestamp_' in file:
            continue
        data = np.load(os.path.join(input_directory,file))
        for j in xrange(data.shape[1]):
            sumsq[j] += (np.sum(data[:,j])-sum[j])**2
    sumsq = np.array(sumsq)
    sumsq = (sumsq*1.0/total)**0.5
    return (sum,sumsq)
        
def train_LSTM(input_directory,train_indices,interval,buffering,sum,sumsq,LSTM_model,iterations):
    global_X = []
    global_Y = []
    global_idx = -1
    global_file_idx = -1
    files = [file for file in os.listdir(input_directory) if '_X_' in file]
    keys = [file for file in os.listdir(input_directory) if '_Y_' in file]
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
                global_file_idx = (global_file_idx + 1)%len(files)
                data = np.load(os.path.join(input_directory,files[global_file_idx]))
                positives = train_indices[keys[global_file_idx]]['positive']
                negatives = train_indices[keys[global_file_idx]]['negative']
                if len(positives) > 0 and len(negatives) > 0:
                    global_Y = np.concatenate(([[0,1] for idx in positives],[[1,0] for idx in negatives]))
                elif len(positives) > 0:
                    global_Y = np.array([[0,1] for idx in positives])
                elif len(negatives) > 0:
                    global_Y = np.array([[0,1] for idx in negatives])
                #global_X = np.concatenate(([data[idx:idx+interval,:] for idx in positives],[data[idx:idx+interval,:] for idx in negatives]))
                global_X = np.concatenate(([data[idx,:] for idx in positives],[data[idx,:] for idx in negatives]))
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

def test_LSTM(input_directory,model,test_indices,interval,sum,sumsq,train_indices=None):
    files = os.listdir(input_directory)
    y_true_test = []
    y_pred_test = []
    y_true_train = []
    y_pred_train = []
    files = [file for file in os.listdir(input_directory) if '_X_' in file]
    keys = [file for file in os.listdir(input_directory) if '_Y_' in file]
    
    for i in xrange(len(files)[:2]):
        file,key = files[i],keys[i]
        data = np.load(os.path.join(input_directory,file))
        for j in xrange(data.shape[1]):
            if sumsq[j] != 0:
                data[:,j] = (data[:,j]-sum[j])/sumsq[j]
        positives = test_indices[key]['positive']
        negatives = test_indices[key]['negative']
        #X_test = np.concatenate(([data[idx:idx+interval,:] for idx in positives],[data[idx:idx+interval,:] for idx in negatives]))
        X_test = np.concatenate(([data[idx,:] for idx in positives],[data[idx,:] for idx in negatives]))
        for idx in positives:
            y_true_test.append([0,1])
        for idx in negatives:
            y_true_test.append([1,0])
        y_pred_test += model.predict(X_test).tolist()

    if train_indices != None:
        for i in xrange(len(files)):
            file,key = files[i],keys[i]
            data = np.load(os.path.join(input_directory,file))
            for j in xrange(data.shape[1]):
                if sumsq[j] != 0:
                    data[:,j] = (data[:,j]-sum[j])/sumsq[j]
            positives = test_indices[key]['positive']
            negatives = test_indices[key]['negative']
            #X_test = np.concatenate(([data[idx:idx+interval,:] for idx in positives],[data[idx:idx+interval,:] for idx in negatives]))
            X_test = np.concatenate(([data[idx,:] for idx in positives],[data[idx,:] for idx in negatives]))
            for idx in positives:
                y_true_train.append([0,1])
            for idx in negatives:
                y_true_train.append([1,0])
            y_pred_train += model.predict(X_test).tolist()

    fpr,tpr,thres = roc_curve([y_true_test[i][1] for i in xrange(len(y_true_test))],[y_pred_test[i][1] for i in xrange(len(y_pred_test))])
    if train_indices != None:
        fpr_t,tpr_t,thres_t = roc_curve([y_true_train[i][1] for i in xrange(len(y_true_train))],[y_pred_train[i][1] for i in xrange(len(y_pred_train))])
    else:
        fpr_t,tpr_t,thres_t=None,None,None
    return fpr,tpr,thres,fpr_t,tpr_t,thres_t
