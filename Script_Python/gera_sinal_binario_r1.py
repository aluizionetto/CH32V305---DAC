# -*- coding: utf-8 -*-
"""
Created on Sun Feb  2 17:23:27 2025

@author: aluiz
"""

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

fsample = 48000  #frequencia de amostragem
f1 = 60  #frequencia do sianl 1
f2 = 58 #frequencia do sinal 2
Tsample = 1 #tempo segundos do sinal

t = np.arange(0,Tsample,1/fsample)
sig1 =((1+np.sin(2*np.pi*f1*t))*4095/2).astype(int)
sig2 =((1+np.sin(2*np.pi*f2*t))*4095/2).astype(int)

plt.plot(t,sig1,t,sig2)

with open('sig.bin','w+b') as siggen :
    for k in range(len(t)):
        sig_g = (sig1[k]*(2**16)+sig2[k]).tobytes()
        siggen.write(sig_g)


