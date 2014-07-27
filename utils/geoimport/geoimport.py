#!/usr/bin/python3
import sys
import gdal
import numpy as np
import scipy
import requests
import zipfile
import scipy.ndimage
import os.path
from PIL import Image
from math import cos, sqrt, pi

def get_dataset(N, W):
    cstring = 'n'+ str(N) + 'w' + str(W)
    zip_filename = 'download/ned1/zip/n'+ str(N) + 'w' + str(W) + '.zip'
    img_filename = 'download/ned1/img/n'+ str(N) + 'w' + str(W) + '.img'

    if not os.path.isfile(zip_filename) and not os.path.isfile(img_filename):
        with open(zip_filename, 'wb') as fd:
            url = 'http://tdds3.cr.usgs.gov/Ortho9/ned/ned_1/img/' + cstring + '.zip'

            response = requests.get(url, stream=True)
        
            if not response.ok:
                print('Error downloading zip.')
                sys.exit(3)

            for block in response.iter_content(1024):
                if not block:
                    break

                fd.write(block)

    if not os.path.isfile(img_filename):
        archive = zipfile.ZipFile(zip_filename, 'r')
        imgdata = archive.extract(cstring + '/img' + cstring + '_1.img', \
                                  'download/ned1/img/')

        os.rename('download/ned1/img/' + cstring + '/img' + cstring + '_1.img', \
                  img_filename)

    
    #gdal.FileFromMemBuffer('/vsimem/img' + cstring + '_1.img', imgdata)

    dataset = gdal.Open(img_filename)
    if dataset is None:
        print("could not open dataset")
        sys.exit(1)

    return dataset

def write_level(dataset, N, W):
    geotransform = dataset.GetGeoTransform()
    if geotransform is None:
        print("Could not get geotransform.")
        return

    sizeX = geotransform[1] * dataset.RasterXSize * 111319.5
    sizeY = geotransform[5] * dataset.RasterYSize * 111319.5

    s =  geotransform[1] * dataset.RasterXSize / 1025 * pi / 180 * 3

    arr = dataset.ReadAsArray()
    arr = scipy.ndimage.zoom(arr, (1025.0 / arr.shape[0], 1025.0 / arr.shape[1]))
#    arr *= 10
#    for x in range(1025):
#        for y in range(1025):
#            arr[x,y] -= 6378100 * (1 - cos(sqrt((x-512)*(x-512)+(y-512)*(y-512))*s))

    amin = np.amin(arr)
    amax = np.amax(arr)
#    if amin > 400:
#        d = amin - 400
#        amin -= d
#        amax -= d
#        arr -= d
#    elif amin < 50:
#        d = 50 - amin 
#        amin += d
#        amax += d
#        arr += d

    arr = (arr - amin) * 65536 / (amax - amin)
    arr = arr.astype(np.uint16)
    cstring = 'n'+ str(N) + 'w' + str(W)

    with zipfile.ZipFile('output/ned1/' + cstring + '.lvl', mode='w') as zf:
        attributes = '[heightmap]\n'
        attributes += 'LOD=1\n'
        attributes += 'foliageDensity=0\n'
        attributes += 'minHeight=' + str(amin) + '\n'
        attributes += 'maxHeight=' + str(amax) + '\n'
        attributes += 'resolutionX=1025\n'
        attributes += 'resolutionY=1025\n'
        attributes += 'sizeX=' + str(sizeX) + '\n'
        attributes += 'sizeZ=' + str(sizeY) + '\n'
        attributes += '[level]\n'
        attributes += 'nextLevel=\n'
        attributes += 'night=false\n'
        attributes += '[shaders]\n'
        attributes += 'shaderType=ISLAND\n'
        zf.writestr('attributes.ini', attributes)
        zf.writestr('objects.txt', """object
{
	type=F22
	team=2
	spawnPos=(57609.6484,6238.03906,-46327.1602)
}
object
{
	type=PLAYER_PLANE
	team=1
	spawnPos=(56162.8008,6238.03906,-77242.0859)
}""")
        print(arr.shape)
        zf.writestr('heightmap.raw',arr.tostring())

dataset = get_dataset(45, 103)
write_level(dataset, 45, 103)

#gdalwarp NLCD2006_LC_N42W105_v1.tif outfile.tif -t_srs "+proj=longlat +ellps=WGS84"

#dataset = gdal.Open('outfile.tif')
#if dataset is None:
#    print("Failed to open dataset")

#geotransform = dataset.GetGeoTransform()
#if geotransform is None:
#    print("Could not get geotransform.")
    

#print('Driver: ', dataset.GetDriver().ShortName,'/', \
#    dataset.GetDriver().LongName)
#print('Size is ',dataset.RasterXSize,'x',dataset.RasterYSize, \
#    'x',dataset.RasterCount)
#print('Projection is ',dataset.GetProjection())

#print('Origin = (',geotransform[0], ',',geotransform[3],')')
#print('Pixel Size = (',geotransform[1], ',',geotransform[5],')')

#print('(',geotransform[0] + geotransform[1] * dataset.RasterXSize, ',',geotransform[3] + geotransform[5] * dataset.RasterYSize,')')
