import pandas as pd
import numpy as np
from collections import Counter
from snaptime_helper import *

def cluster_data(data_file,timeinterval,beta,K,rho,lamb,n,w):
    data = pd.read_csv(data_file,header=None).values
    act = []
    for i in xrange(len(data)-timeinterval+1):
        act.append(np.hstack(data[i:i+timeinterval,:]))
    act = np.array(act)
    #beta = 20
    #K = 4
    #rho = 1
    #lamb = np.full((25,25),0.01)
    #n = 5
    #w = 5
    init_mu = NumpyList()
    init_theta = NumpyList()
    d2 = act.copy()
    np.random.shuffle(d2)
    for i in xrange(K):
        arr = []
        #if i == 0:
            #arr = np.concatenate((np.arange(200),np.arange(400,600),np.arange(800,1000),np.arange(1200,1400)))
        #    arr = np.concatenate((np.arange(200),np.arange(400,585)))
        #else:
            #arr = np.concatenate((np.arange(200,400),np.arange(600,800),np.arange(1000,1200),np.arange(1400,1596)))
        #    arr = np.arange(200,400)
        arr = np.concatenate((np.arange(400*i,400*(i+1)),np.arange(1600+400*i,min(1600+400*(i+1),len(d2)))))
        init_mu.push_back(np.mean(d2[arr,:],axis=0).reshape(1,25))
        init_theta.push_back(np.linalg.inv(np.cov(d2[arr,:],rowvar=False)))
    obj = Solver(25,K,beta,rho,act,lamb,n,w,init_mu,init_theta)
    obj.Solve(20)
    return obj.obtainAssignment()


