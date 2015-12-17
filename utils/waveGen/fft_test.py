#!/usr/bin/python2

import numpy as np
import sympy
import math
import cmath

def ifft_1D(N):
    A = np.zeros(N)
    for x in range(N):
        A[x] = np.random.normal()

    
    a2 = np.zeros(N, dtype=complex)
    for m in range(N):
        for k in range(N):
            a2[m] += A[k] * cmath.exp(2.0j*math.pi*m*k/N)
        a2[m] /= N
    
    a = np.fft.ifft((A))
    print a.real
    print a2.real

    
def ifft_2D(N):
    A = np.zeros((N,N))
    for x in range(N):
        for y in range(N):
            A[x,y] = np.random.normal()

    
    a2 = np.zeros((N,N), dtype=complex)
    for mx in range(N):
        for my in range(N):
            for kx in range(N):
                for ky in range(N):
                    a2[mx,my] += A[kx,ky] * cmath.exp(2.0j*math.pi*(mx*kx+my*ky)/N)
            a2[mx,my] /= N**2
    
    a = np.fft.ifft((A))
    print a.real
    print a2.real

def fft_2D(N):
    a = np.zeros((N,N))
    for x in range(N):
        for y in range(N):
            a[x,y] = np.random.normal()

    
    A2 = np.zeros((N,N), dtype=complex)
    for kx in range(N):
        for ky in range(N):
            for mx in range(N):
                for my in range(N):
                    ex = -2.0j*math.pi*(mx*kx+my*ky)/N
                    A2[kx,ky] += a[mx,my] * cmath.exp(ex)
    
    A = np.fft.fft2(a)
    print A.real
    print A2.real


fft_2D(2)
