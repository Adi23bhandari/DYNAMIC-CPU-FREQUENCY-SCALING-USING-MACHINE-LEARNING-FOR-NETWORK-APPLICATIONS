import time
import random



import os 
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import warnings 
import pprint
from keras.models import Sequential
from keras.layers import LSTM
warnings.filterwarnings("ignore")
pp=pprint.PrettyPrinter(indent=4)


file_name="dataset_24hrs.csv"
df_path=os.path.join(os.getcwd(),file_name)
df=pd.read_csv(df_path)

times=[]
hrs='04'
mins='00'
for i in range(480):
    if int(hrs)==23 and int(mins)==60:
        hrs='00'
        mins='00'
        time=hrs+mins
        times.append(time)
        mins='03'
    elif int(mins)>59:
        mins='00'
        hrs=int(hrs)+1
        if hrs//10==0:
            hrs='0'+str(hrs)
        else:
            hrs=str(hrs)
        time=hrs+mins
        times.append(time)
        mins='03'

    else:
        time=hrs+mins
        times.append(time)
        
        mins=int(mins)+3
        if mins//10==0:
            mins='0'+str(mins)
        else:
            mins=str(mins)

df['Time'] = times


import datetime
import numpy

df_series=df['Throughput']

timeseries=np.array(df_series)

# data preprocessing 
def split_sequence(sequence, n_steps):
    X, y = list(), list()
    for i in range(len(sequence)):
        # find the end of this pattern
        end_ix = i + n_steps
        # check if we are beyond the sequence
        if end_ix > len(sequence)-1:
            break
        # gather input and output parts of the pattern
        seq_x, seq_y = sequence[i:end_ix], sequence[end_ix]
        X.append(seq_x)
        y.append(seq_y)
    return np.array(X), np.array(y)


n_steps = 3

# split into samples
X, y = split_sequence(timeseries, n_steps)

n_features = 1
X = X.reshape((X.shape[0], X.shape[1], n_features))
model = Sequential()
model.add(LSTM(50, activation='relu', input_shape=(n_steps, n_features)))
model.add(Dense(1))
model.compile(optimizer='adam', loss='mse')
model.fit(X, y, epochs=100, verbose=1)


predict_freq_file="dataset/predict.csv"

iterations=3
run_program=10
Hz=1000000000
file=open(predict_freq_file,'w')
file.close();

begin=time.time()
for i in range(iterations): 

    new_sequence = np.array([timeseries[i-2],timeseries[i-1],timeseries[i]])

    new_sequence = new_sequence.reshape((1, n_steps, 1))

    predicted_value = model.predict(new_sequence, verbose=0)

    predicted_value=predicted_value/Hz

    file1 =open(predict_freq_file, "a")

    line=str(predicted_value)+','
    line=line+'\n'
    file1.write(line)
    file1.close()
    end = time.time()
    time.sleep(run_program-(end-start));


end=time.time()
print(end-begin)