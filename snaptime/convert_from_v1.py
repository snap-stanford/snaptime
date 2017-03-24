import pandas as pd
import datetime
import numpy as np
import sys

"""the input file is assumed to be in the format : datetime driver_id signal_name....
signals_with_dtype has the datatype for each VW signal"""
epoch = datetime.datetime.utcfromtimestamp(0)
filename = sys.argv[1]
init_epoch = (datetime.datetime.strptime('_'.join(filename.split('/')[-1].split('_')[:2]),'%Y%m%d_%H') - epoch).total_seconds()*1000
#print init_epoch
d = pd.read_csv(filename,sep='\t',low_memory=False,header=None)
#d = pd.read_csv(filename,sep='\t',low_memory=False,nrows=10000)
epochs = pd.to_datetime(d[0],format='%Y-%m-%d %H:%M:%S.%f').apply(lambda x : (x-epoch).total_seconds()*1000).values
#epochs = pd.to_datetime(d['Date']+' '+d['Time'],format='%d.%m.%Y %H:%M:%S').apply(lambda x : (x-epoch).total_seconds()*1000).values
#dtypes = ['float' for i in xrange(2,len(d.columns))]
dtypes = []
with open('signals_with_dtype') as fp:
    for line in fp:
        line = line.strip().split()
        dtypes.append(line[1])
#d.columns = [i for i in xrange(len(d.columns))]
for i in xrange(2,len(d.columns)):
    if dtypes[i-2] == 'float' or dtypes[i-2] == 'bool':
        s = dtypes[i-2] + '\t' + str(i-2)
        dat = d[i].values
        idx = np.where(pd.isnull(d[i])==False)[0]
        #idx = np.where(np.isnan(dat) == False)[0]
        for ix in idx:
            s += '\t' + str(int(epochs[ix]))+','+str(dat[ix])
        print s

