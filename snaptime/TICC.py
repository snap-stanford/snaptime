import pandas as pd
import numpy as np
from collections import Counter
from snaptime_helper import *
from sklearn.mixture import GaussianMixture
from datetime import datetime

def cluster_data(data_file,beta,K,rho,lamb,n,w,iterations=20,sweep_length=5,e_abs=1e-4,e_rel=1e-4,ADMM_iter=100,granularity=1000,timeslice=3600000):
    """data_file : file containing timesteps x sensors data in snaptime format
    beta - switching penalty
    K - number of clusters
    rho - regularisation penalty
    lamb - sparsity parameter
    n - number of sensors
    w - window size
    iterations - number of iterations of the algorithm
    sweep length - contiguous segment length assigned to an empty cluster
    e_abs,e_rel - ADMM parameters for convergence
    ADMM_iter - max ADMM iterations
    granularity - minimum difference between timestamps in milliseconds
    timeslice - total length of timeseries in milliseconds

    returns - 
    cluster assignments to all points
    array containing theta for each cluster
    """
    np.random.seed(1)
    obj = FillData()
    epoch = datetime.utcfromtimestamp(0)
    init_epoch = long((datetime.strptime('_'.join(data_file.split('/')[-1].split('_')[:2]),'%Y%m%d_%H') - epoch).total_seconds()*1000)
    data = obj.createAndFillData(data_file,init_epoch,timeslice,granularity)
    act = []
    for i in xrange(len(data)-w+1):
        act.append(np.hstack(data[i:i+w,:]))
    act = np.array(act)
    init_mu = NumpyList()
    init_theta = NumpyList()
    gmm = GaussianMixture(K)
    gmm.fit(act)
    for i in xrange(K):
        init_mu.push_back(gmm.means_[i])
        init_theta.push_back(np.linalg.inv(gmm.covariances_[i]))
    obj = Solver(sweep_length,K,beta,rho,act,lamb,n,w,init_mu,init_theta,e_abs,e_rel,ADMM_iter)
    obj.Solve(iterations)
    return obj.obtainAssignment(),[obj.obtainTheta(i) for i in range(K)]
