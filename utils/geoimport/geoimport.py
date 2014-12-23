#!/usr/bin/python3
import sys
import gdal
import numpy as np
import math
import scipy
import zipfile
import scipy.ndimage
import os.path
import urllib3
import ftplib
import itertools
from PIL import Image
from math import cos, sqrt, pi

#https://gis.stackexchange.com/questions/37405/programmatically-download-seamless-ned-tiles

# As per http://www.usgs.gov/faq/categories/9852/5861, we are using the FTP
# service available at ftp://rockyftp.cr.usgs.gov/vdelivery/Datasets/Staged/NED/1/IMG/
# to download NED1 tiles.

def ftp_download(server, remote_filename, local_filename):
    if os.path.isfile(local_filename):
        return
    
    ftp = ftplib.FTP(server)
    ftp.login("anonymous", "password")
    with open(local_filename, 'wb') as file:
        ftp.retrbinary('RETR ' + remote_filename, file.write)

def show_preview(north, west, width, height):
    for x in range(width):
        for y in range(height):
            cstring = "n{0:02d}w{1:03d}".format(north+x, west+y)
            ftp_download('rockyftp.cr.usgs.gov', \
                         'vdelivery/Datasets/Staged/NED/1/IMG/img' + cstring + '_1_thumb.jpg', \
                         'download/ned1/preview/img' + cstring + '_1_thumb.jpg')

    images = [[Image.open('download/ned1/preview/imgn{0:02d}w{1:03d}_1_thumb.jpg'.format(north+x,west+y)) for x in range(width)] for y in range(height)]
    im_shape = images[0][0].size
    for im in itertools.chain(*images):
        assert im.size == im_shape

        
    preview_shape = im_shape[0] * width, im_shape[1] * height

    preview = Image.new("RGB", preview_shape)
    for x in range(width):
        for y in range(height):
            preview.paste(images[x][y], ((width-1-x) * im_shape[0], (height-1-y) * im_shape[1]))

    preview.thumbnail((512,512))
    preview.show()
        

def get_dataset(N, W):
    cstring = "n{0:02d}w{1:03d}".format(N,W)
    zip_filename = 'download/ned1/zip/'+ cstring + '.zip'
    img_filename = 'download/ned1/img/'+ cstring + '.img'

    zip_downloaded = os.path.isfile(zip_filename)
    img_extracted = os.path.isfile(img_filename)

    print(zip_filename + (": Already downloaded." if zip_downloaded else ": Downloading..."))
    if not zip_downloaded and not img_extracted:
#        url = 'ftp://rockyftp.cr.usgs.gov/vdelivery/Datasets/Staged/NED/1/IMG/' + cstring + '.zip'
        ftp = ftplib.FTP("rockyftp.cr.usgs.gov")
        n = 'vdelivery/Datasets/Staged/NED/1/IMG/' + cstring + '.zip'
        ftp.login("anonymous", "password")
        with open(zip_filename, 'wb') as file:
            ftp.retrbinary('RETR ' + n, file.write)

    if not img_extracted:
        archive = zipfile.ZipFile(zip_filename, 'r')
        imgdata = archive.extract('img' + cstring + '_1.img', 'download/ned1/img/')
        os.rename('download/ned1/img/img' + cstring + '_1.img', img_filename)

    
    #gdal.FileFromMemBuffer('/vsimem/img' + cstring + '_1.img', imgdata)

    dataset = gdal.Open(img_filename)
    if dataset is None:
        print("could not open dataset")
        sys.exit(1)

    return dataset


def arr_zoom(arr, step, final_shape):
    returny = np.empty(final_shape, arr.dtype)

    cx, cy = arr.shape[0]/2, arr.shape[1]/2
    w,h = final_shape
    for x in range(w):
        for y in range(h):
            returny[x,y] = arr[cx+(x-w/2)*step, cy+(y-h/2)*step]

    return returny


def write_level(datasets, filename):

    # Extract the geotransforms for each dataset
    geotransforms = [dataset.GetGeoTransform() for dataset in itertools.chain(*datasets)]

    # Validate that all geotransforms match
    for g in geotransforms:
        if g is None:
            print("Could not get geotransform!")
            return
        if not np.allclose(geotransforms[0][1], g[1]) or not np.allclose(geotransforms[0][5], g[5]):
            print("Geotransforms do not match!")
            return
        
    sizeX = geotransforms[0][1] * datasets[0][0].RasterXSize * 111319.5 * len(datasets[0])
    sizeY = geotransforms[0][5] * datasets[0][0].RasterYSize * 111319.5 * len(datasets)

    # Concatenate all the input datasets into a single large array.
    print("concatenating datasets...")
    arr = np.concatenate([np.concatenate([d.ReadAsArray() for d in row], axis=1) for row in datasets], axis=0)

    # Resample arr to have a resolution that is in the form 2^n+1. The final
    # resolution is selected by rounding the input resolution down to the
    # nearest value that meets our requirements.
    print(str(arr.nbytes >> 20)+ " MiB")
    assert arr.shape[0] == arr.shape[1]
    full_res = math.pow(2, math.floor(math.log(arr.shape[0]-1,2))) + 1; 
    print("Resampling... (from {0}x{0} to {1}x{1})".format(arr.shape[0], full_res))
    print("Scale={0}".format((full_res / arr.shape[0])))
    arr2 = scipy.ndimage.zoom(arr, (full_res / arr.shape[0]+1.e-6, full_res / arr.shape[1]+1.e-6), order=0)
    print("Done Resampling")
    exit()
#    arr *= 10
#    s =  geotransform[1] * dataset.RasterXSize / 1025 * pi / 180 * 3
#    for x in range(1025):
#        for y in range(1025):
#            arr[x,y] -= 6378100 * (1 - cos(sqrt((x-512)*(x-512)+(y-512)*(y-512))*s))

    amin = np.amin(arr)
    amax = np.amax(arr)

    arr = (arr - amin) * 65536 / (amax - amin)
    print("converting array type...")
    arr = arr.astype(np.uint16)
    print("done converting array type...")
    
    num_levels = math.log((full_res-1) / 256, 2) + 1
    arr_levels = [arr_zoom(arr, 1 << n, (257,257)) for n in range(num_levels)]
    print("Done computing zoom")
    
    print("Writing level file...")
    with zipfile.ZipFile('output/ned1/' + filename, mode='w') as zf:
        attributes = '[heightmap]\n'
        attributes += 'LOD=1\n'
        attributes += 'foliageDensity=0\n'
        attributes += 'minHeight=' + str(amin) + '\n'
        attributes += 'maxHeight=' + str(amax) + '\n'
        attributes += 'resolutionX=257\n'

        attributes += 'resolutionY=257\n'
        attributes += 'depth=4\n'
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
	spawnPos=(3000.0,20000.0,10000.0)
}
object
{
	type=PLAYER_PLANE
	team=1
	spawnPos=(0.0,2000.0,0.0)
}""")

        for n in range(len(arr_levels)):
            l = str((len(arr_levels) - 1) - n)
            if l == '0':
                l = ''
            zf.writestr('heightmap' + l + '.raw', arr_levels[n].tostring())


################################################################################


north = 44
west = 95
width = 8
height = 8

#show_preview(north, west, width, height)
dataset = [[get_dataset(north+x,west+y) for x in range(width)] for y in range(height)] 
write_level(dataset, 'n45_49w100_103')

################################################################################



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
