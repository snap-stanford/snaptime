from snaptime import LSTM_monolithic
from fillData import *
import numpy as np
import cPickle as pickle
from time import time
import os

from keras.preprocessing import sequence
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation
from keras.layers.recurrent import LSTM
from keras.regularizers import l1
import tensorflow as tf
from keras.optimizers import Adam


model = Sequential()
model.add(LSTM(256,input_shape=(10,131+1265)))#dropout_W=0.95,dropout_U=0.95))
model.add(Dense(2))
model.add(Activation('sigmoid'))
opt = Adam(lr=0.0015)
model.compile(loss='binary_crossentropy',optimizer='adam',metrics=['accuracy'])

fpr,tpr,thres,test_score,fpr_t,tpr_t,thres_t,train_score = LSTM_monolithic.run_LSTM('../data/vw',\
        np.array([i for i in xrange(1397) if i != 1174]),\
        np.array([1174]),\
        10,\
        10,\
        3600000,\
        100,\
        lambda y : y[0] == 1,\
        500,\
        model,\
        2)
print test_score,train_score

