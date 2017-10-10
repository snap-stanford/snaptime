from create_data import create_raw_data
from create_data import create_LSTM_data
from run_LSTM import *
from fillData import *
import numpy as np
import cPickle as pickle
from time import time
import os

"""np.random.seed(1337)  # for reproducibility

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
tf.python.control_flow_ops = tf"""


#t1 = time()
#create_raw_data('/lfs/local/0/abhisg/vw/tests/first_input','/lfs/local/0/abhisg/vw/tests/first_output',np.array([i for i in xrange(1397) if i != 1174]),np.array([1174]))
#t2 = time()
#print t2-t1

t1 = time()
create_LSTM_data('/lfs/local/0/abhisg/vw/tests/first_output','/lfs/local/0/abhisg/vw/tests/train_file','/lfs/local/0/abhisg/vw/tests/test_file',1000,10,lambda y : y[0] == 1)
t2 = time()
print t2-t1

"""t1 = time()
train_indices = pickle.load(open('/lfs/local/0/abhisg/vw/tests/train_file_10_10','r'))
test_indices = pickle.load(open('/lfs/local/0/abhisg/vw/tests/test_file_10_10','r'))
model = Sequential()
model.add(LSTM(256,input_shape=(10,131+1265)))#dropout_W=0.95,dropout_U=0.95))
model.add(Dense(2))
model.add(Activation('sigmoid'))
opt = Adam(lr=0.0015)
model.compile(loss='binary_crossentropy',optimizer='adam',metrics=['accuracy'])

sum,sumsq = preprocess_data('/lfs/local/0/abhisg/vw/tests/first_output',1396)
model = train_LSTM('/lfs/local/0/abhisg/vw/tests/first_output',train_indices,10,50,sum,sumsq,model,10)
print test_LSTM('/lfs/local/0/abhisg/vw/tests/first_output',model,test_indices,10,sum,sumsq)
t2 = time()
print t2-t1"""
