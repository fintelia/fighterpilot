#!/usr/bin/python3
# -*- coding: utf-8 -*-

# See http://www.keithlantz.net/2011/10/ocean-simulation-part-one-using-the-discrete-fourier-transform/

import numpy as np
import math
import cmath
import scipy
import scipy.misc
import sys

g = 9.8

def genFrequencies(N, Lx, U10, w):
    arr = np.zeros((N,N), dtype=complex)

    A = 1.0
    L = U10**2 / g
    l = Lx / 1000.0

    max_P = 0

    for x in range(N):
        for y in range(N):
            if x == N/2 and y == N/2:
                continue
            
            kx = 2.0*math.pi * (x - N/2) / Lx
            ky = 2.0*math.pi * (y - N/2) / Lx
            k = math.sqrt(kx**2 + ky**2)

            crand = np.random.normal() + 1j*np.random.normal()
            P = A * math.exp(-1.0 / (k*L)**2) / k**4
            P *= math.exp(-(k**2 * l**2))

            #ω = math.sqrt(k*g)
            #α = 0.0081
            #ωp = 0.855 * g / U10
            #S = α * g**2 / ω**5 * math.exp(-1.25 * (ωp / ω)**4)
            arr[x,y] = crand * math.sqrt(P / 2 * (kx/k*w[0] + ky/k*w[1])**2) 
    return arr

def computeHeights(N,Lx,t,w0,tilde_h0):
    tilde_h = np.zeros((N,N), dtype=complex)
#    ikx_tilde_h = np.zeros((N,N), dtype=complex)
#    iky_tilde_h = np.zeros((N,N), dtype=complex)
    for x in range(N):
        for y in range(N):
            kx = 2.0*math.pi * (x - N/2) / Lx
            ky = 2.0*math.pi * (y - N/2) / Lx
            k = math.sqrt(kx**2 + ky**2)
            w = math.ceil(math.sqrt(g * k)/w0)*w0
            jwt = 1j * w * t
            tilde_h[x,y] = tilde_h0[x,y] * cmath.exp(jwt) # + \
#                           tilde_h0[N-1-x, N-1-y] * cmath.exp(-jwt)

#            ikx_tilde_h[x,y] = 1j * kx * tilde_h[x,y]
#            iky_tilde_h[x,y] = 1j * ky * tilde_h[x,y]

    h = np.fft.fft2(np.fft.ifftshift(tilde_h))
#    ex = np.fft.fft2(np.fft.ifftshift(ikx_tilde_h))
#    ey = np.fft.fft2(np.fft.ifftshift(iky_tilde_h))
    return h


 #   img[0:N, t*N:(t+1)*N] = h.real
 #   img2[0:N, t*N:(t+1)*N,0] = ex.real
 #   img2[0:N, t*N:(t+1)*N,1] = ey.real

def computeNormals(heights):
    W,H = heights.shape
    normals = np.zeros((W,H,3))
    for x in range(W):
        for y in range(H):
            dx = heights[(x+1)%W, y] - heights[(x-1)%W, y]
            dy = heights[x, (y+1)%H] - heights[x, (y-1)%H]
            s = 2 / math.sqrt(dx**2 + dy**2 + 4)
            normals[x,y,:] = (dx*s*0.5+0.5, dy*s*0.5 + 0.5, s)
    return normals

def generateOcean(N, Lx, V, T, f):
    w0 = 2*math.pi / T
    tilde_h0 = genFrequencies(N, Lx, V, (0,1))
    heights = np.zeros((N,N*T*f))

    for tf in range(T*f):
        heights[:,tf*N:(tf+1)*N] = computeHeights(N,Lx,tf/f,w0,tilde_h0).real
        print( "heightmap", tf)

    print( "Scale = ", (heights.max() - heights.min()))
    heights = (heights - heights.min()) / (heights.max() - heights.min())

    normals = np.zeros((N,N*T*f,3))
    for tf in range(T*f):
        normals[:,tf*N:(tf+1)*N] = computeNormals(heights[:,tf*N:(tf+1)*N])
        print( "normalmap", tf)

    heights = (heights*255).astype(np.uint8)
    normals = (normals*255).astype(np.uint8)
    return heights, normals

def saveImages(heights, normals, prefix):
    scipy.misc.imsave(prefix+'Heights.png', heights.transpose())
    scipy.misc.imsave(prefix+'Normals.png', normals.transpose())


#print("=== FFT Ocean 1 ===")
#h,n = generateOcean(512, 2000, 15, 16, 2)
#saveImages(h,n, 'fftOcean')
#print("Done")

print("=== FFT Ocean 2 ===")
h,n = generateOcean(1024, 105, 15, 4, 2)
saveImages(h,n, 'fftOcean2')
print("Done")
#for x in range(N):
#    for y in range(N):
#        for kx in range(-N/2, N/2):
#            for ky in range(-N/2, N/2):
#                kdotx = 2.0*math.pi * (float(x)/N * kx + float(y)/N * ky)
#                th = tilde_h[N/2+kx, N/2+ky]
#                img[x,y] += (th * cmath.exp(-1j * kdotx)).real
