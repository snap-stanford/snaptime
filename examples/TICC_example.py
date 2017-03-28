from snaptime import TICC
import numpy as np
from collections import Counter
data_file = '../data/TICC/ABCDABCD_400.csv'
#cluster_data(data_file,timeinterval,beta,K,rho,lamb,n,w,iterations=20,sweep_length=5,e_abs=1e-4,e_rel=1e-4,ADMM_iter=100):
order = TICC.cluster_data(data_file,5,25,4,1,np.full((25,25),0.01) ,5,5,sweep_length=50)
print Counter(order)
