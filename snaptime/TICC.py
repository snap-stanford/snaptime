import pandas as pd
import numpy as np
from collections import Counter
from snaptime_helper import *
from sklearn.mixture import GaussianMixture

def cluster_data(data_file,timeinterval,beta,K,rho,lamb,n,w,iterations=20,sweep_length=5,e_abs=1e-4,e_rel=1e-4,ADMM_iter=100):
    data = pd.read_csv(data_file,header=None).values
    act = []
    for i in xrange(len(data)-timeinterval+1):
        act.append(np.hstack(data[i:i+timeinterval,:]))
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
    return obj.obtainAssignment()


