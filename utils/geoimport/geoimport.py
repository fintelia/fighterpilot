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
import subprocess
from PIL import Image
from math import cos, sqrt, pi

# FTP access to nation map data:
# ftp://rockyftp.cr.usgs.gov/vdelivery/Datasets/Staged/


# NLCD Sample Download link:
# http://gisdata.usgs.gov/tdds/downloadfile.php?TYPE=nlcd2011_lc_3x3&ORIG=SBDDG&FNAME=NLCD2011_LC_N69W147.zip

#https://gis.stackexchange.com/questions/37405/programmatically-download-seamless-ned-tiles

# As per http://www.usgs.gov/faq/categories/9852/5861, we are using the FTP
# service available at ftp://rockyftp.cr.usgs.gov/vdelivery/Datasets/Staged/NED/1/IMG/
# to download NED1 tiles.

def ftp_download(server, remote_filename, local_filename, force=False):
    if not force and os.path.isfile(local_filename):
        return True

    try:
        ftp = ftplib.FTP(server)
        ftp.login("anonymous", "password")
        with open(local_filename, 'wb') as file:
            print("Downloading " + local_filename + "...")
            ftp.retrbinary('RETR ' + remote_filename, file.write)
        return True
    except:
        return False

def try_open_image(filename):
    try:
        return Image.open(filename)
    except:
        return None
    
def show_preview(north, west, size):
    # Download any preview images we don't yet have
    for x in range(size):
        for y in range(size):
            cstring = "n{0:02d}w{1:03d}".format(north+x, west+y)
            ftp_download('rockyftp.cr.usgs.gov', \
                         'vdelivery/Datasets/Staged/NED/1/IMG/img' + cstring + '_1_thumb.jpg', \
                         'download/ned1/preview/img' + cstring + '_1_thumb.jpg')

    # Open images
    images = [[try_open_image('download/ned1/preview/imgn{0:02d}w{1:03d}_1_thumb.jpg'.format(north+x,west+y)) for x in range(size)] for y in range(size)]

    # Make sure all images are the same shape, and find out what that is
    im_shape = None
    for im in itertools.chain(*images):
        if im is None:
            continue
        elif im_shape is None:
            im_shape = im.size
        else:
            assert im.size == im_shape

    # Make sure we got at least one preview image
    if im_shape is None:
        print("Preview display failed")

    # Combine all images into one
    preview_shape = im_shape[0] * size, im_shape[1] * size
    preview = Image.new("RGB", preview_shape, "blue")
    for x in range(size):
        for y in range(size):
            if images[x][y] is not None:
                preview.paste(images[x][y], ((size-1-x) * im_shape[0], (size-1-y) * im_shape[1]))

    # Downsample and display the result
    preview.thumbnail((512,512))
    preview.show()
        

def get_dataset(N, W):
    cstring = "n{0:02d}w{1:03d}".format(N,W)
    zip_filename = 'download/ned1/zip/'+ cstring + '.zip'
    img_filename = 'download/ned1/img/'+ cstring + '.img'
    
    if os.path.isfile(img_filename):
        return img_filename

    zip_downloaded = ftp_download('rockyftp.cr.usgs.gov', \
                                  'vdelivery/Datasets/Staged/NED/1/IMG/' + cstring + '.zip', \
                                  zip_filename)
    
    if not zip_downloaded:
        return ""

    archive = zipfile.ZipFile(zip_filename, 'r')
    imgdata = archive.extract('img' + cstring + '_1.img', 'download/ned1/img/')
    os.rename('download/ned1/img/img' + cstring + '_1.img', img_filename)

    return img_filename
    
    #dataset = gdal.Open(img_filename)
    #if dataset is None:
    #    print("could not open dataset")
    #    sys.exit(1)
    #
    #return dataset

    
def arr_zoom(band, center, step, final_shape):
    returny = np.empty(final_shape, band.ReadAsArray(0,0,1,1).dtype)

    cx, cy = center
    w2 = int(final_shape[0]/2)
    h2 = int(final_shape[1]/2)
    print("cx, cy: {0} {1}".format(cx, cy))
    print("w2, h2: {0} {1}".format(w2, h2))
    print("step {0}".format(step))
    for x in range(final_shape[0]):
        for y in range(final_shape[1]):
            data = band.ReadAsArray(int(cx+(x-w2)*step), \
                                    int(cy+(y-h2)*step), 1, 1)
            returny[x,y] = data[0,0]

    return returny


def write_level(dataset, filename):
    geotransforms = dataset.GetGeoTransform()
    # TODO: Account for these sizes being negative (by flipping the arrays)
    sizeX = math.fabs(geotransforms[1] * dataset.RasterXSize * 111319.5)
    sizeY = math.fabs(geotransforms[5] * dataset.RasterYSize * 111319.5)
    
    assert dataset.RasterXSize == dataset.RasterYSize
    assert dataset.RasterCount > 0
    
    #arr = dataset.ReadAsArray()
    band = dataset.GetRasterBand(1)
    print('Computing statistics...')

    band.ComputeStatistics(False, gdal.TermProgress) #, lambda x,msg,data : print()
    amin = band.GetMinimum()
    amax = band.GetMaximum()
#    arr *= 10
#    s =  geotransform[1] * dataset.RasterXSize / 1025 * pi / 180 * 3
#    for x in range(1025):
#        for y in range(1025):
#            arr[x,y] -= 6378100 * (1 - cos(sqrt((x-512)*(x-512)+(y-512)*(y-512))*s))

    print(amax)
    print(amin)
    #arr = (arr - amin) * 65536 / (amax - amin)
    #arr = arr.astype(np.uint16)


    out_resolution = 2048
    num_levels = int(math.log(dataset.RasterXSize / out_resolution, 2) + 1)

    arr_levels = []
    center = int(dataset.RasterXSize/2), int(dataset.RasterYSize/2)
    for n in range(num_levels):
        level = arr_zoom(band, center, 1 << n, (out_resolution,out_resolution))
        level = (level - amin) * 65536 / (amax - amin)
        level = level.astype(np.uint16)
        arr_levels.append(level)
    
    print("Writing level file...")
    with zipfile.ZipFile('output/ned1/' + filename, mode='w') as zf:
        attributes = '[heightmap]\n'
        attributes += 'minHeight=' + str(amin) + '\n'
        attributes += 'maxHeight=' + str(amax) + '\n'
        attributes += 'resolution=' + str(out_resolution) + '\n'
        attributes += 'numLayers=' + str(num_levels) + '\n'
        attributes += 'sideLength=' + str(sizeX) + '\n'
        attributes += 'foliageDensity=0\n'
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
            zf.writestr('heightmap' + l + '.raw', arr_levels[n].tostring())

def combine_datasets(datasets, name):
    datasets_string = ' '.join(map(str, itertools.chain(*datasets)))
    filename = "tmp/" + name + ".vrt"
#    d = gdal.Open(datasets[0][0])
#    assert d is not None
#    assert d.RasterXSize == d.RasterYSize
#    full_size = math.pow(2, math.floor(math.log(d.RasterXSize*size-1,2))) + 1; 

    if not os.path.isfile(filename):#and not os.path.isfile(name + ".tif"):
        print("Combining datasets...")
        subprocess.call("gdalbuildvrt {0} {1}".format(filename, datasets_string), shell=True)
#    if not os.path.isfile(name + ".tif"):
#        print("Resampling...")
#        subprocess.call("gdalwarp -ts {0} {0} -r cubic -overwrite {1}.vrt {1}.tif".format(full_size, name), shell=True)

#    print(name + ".tif")
    ret =  gdal.Open(filename)
    assert ret is not None
    return ret

################################################################################

north = 41
west = 102
size = 10
name = "n{0}_{1}w{2}_{3}".format(north, north+size, west, west+size)
    
show_preview(north, west, size)
datasets = [[get_dataset(north+x,west+y) for x in range(size)] for y in range(size)]
dataset = combine_datasets(datasets, name)
write_level(dataset, name + ".lvl")

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
