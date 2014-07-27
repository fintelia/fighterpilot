#!/usr/bin/python3

# see
# http://outerra.blogspot.com/2011/02/ocean-rendering.html
# http://forum.outerra.com/index.php?topic=544.0

import numpy as np
from math import sin, cos, atan, pow, pi, sqrt
from PIL import Image

arr = np.zeros((256, 256,3))

for y in range(256):
    w = np.zeros((256,3))

    b = 0.00 # must be ~0.005 so that there aren't overhangs...
    for t in range(256):
        gamma = 1-y/256
        ang = pow(t/256, gamma) * 2*pi
        slope = 0
        if t != 0:
            slope = sin(ang)/(b*cos(ang) - 1/(2*pi*gamma*pow(t/256,gamma-1)))

        w[t] = (t - 256*b*sin(ang), 0.5+0.5*cos(ang), slope)

   # w[0,2] = w[255,2]

    lt = 0
    nt = 1
    for x in range(256):
        while nt < 255 and w[nt,0] < x:
            if(w[nt,0] < w[nt+1,0]):
                lt = nt
            nt = nt + 1

        dt = w[nt,0] - w[lt,0]
        height = (w[nt,0] - x)/dt * w[lt,1] + (1 - (w[nt,0]-x)/dt) * w[nt,1]
        slope = (w[nt,0] - x)/dt * w[lt,2] + (1 - (w[nt,0]-x)/dt) * w[nt,2]
        
        arr[y,x,0] = height
        arr[y,x,1] = 0.5 + round(slope) / 256
        arr[y,x,2] = 0.5 + 0.5 * (slope-round(slope))

        if y == 0:
            print(arr[y,x])
#        arr[y,x,2] = 0.5 + 0.5 * cos(atan(slope)) # y component of normal

im = Image.fromarray((arr * 255).astype(np.uint8))
im.save("waves.png")
