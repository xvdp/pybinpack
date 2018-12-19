import os
import os.path as osp
import time
import numpy as np
import scipy
from PIL import Image
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

def readversion():
    print(os.getcwd())
    with open('version.py', 'r') as _fi:
        return _fi.read()
    # _file = osp.abspath('../src/main.cpp')
    # with open(_file, 'r') as _fi:
    #     return _fi.read().split('version__"')[1].split('"')[1]

def pack_to_canvas(pack):
    repack = pack.reshape(-1, 7)
    # shape: (num_imgs, (bin_idx, img_idx, rot, x, y, w, h))

    bin_info = repack[:, 0] # bin_idx
    _canvases = repack[:, 1:].reshape(-1, 3, 2) #((idx, rot), (x, y), (w, h))

    canvases = [] # stack as list, as each canvas has differnt nb images
    for i in range(bin_info.max() + 1):
        this_bin = np.ma.where(bin_info == i)
        canvases.append(_canvases[this_bin[0].min():this_bin[0].max()+1])

    #for all bins, find max size bin
    binsize = np.array([canvas[:, 1:3].sum(axis=1).max(axis=0) for canvas in canvases]).max(axis=0)
    return canvases, binsize

def greyscale(img):
    """
        convert 3 or 4 channel image to greyscale
    """
    assert len(img.shape) == 3  and img.shape[2] in [3,4], "expecting rgb or rgba, not "+str(img.shape)
    tog = np.array([0.2989, 0.5870, 0.1140])
    # flattens, multiplies, sums, clips to uint8, reshapes to (x,y)
    return np.ceil((img[:, :, :3].reshape(-1, 3)*tog).sum(axis=1)).astype(np.uint8).reshape(img.shape[:2])


def fix_channels(img, channels):
    """
    validate image has spec'd channels
    """
    assert len(img.shape) in [2, 3], "image has shape "+str(img.shape)
    assert channels in [1, 3, 4], "only rgb, rgba and greyscale conversions handled, requested channels "+str(channels)

    if channels == 1:
        if len(img.shape) == 2: # 1 to 1
            return img
        elif img.shape[2] == 1: # 1 to 1
            return img[:, :, 0]
        else:
            return greyscale(img) # 3 || 4 to 1
    else:
        if len(img.shape) == 2:
            img = np.expand_dims(img, 2)
        if img.shape[2] == 1: # 1 to 3 || 4
            return np.repeat(img, channels, axis=2)

        if channels == img.shape[2]: # 3 to 3 || 4 or 4
            return img
        elif channels == 3 and img.shape[2] == 4: # 4 to 3
            return img[:, :, :3]
        elif channels == 4 and img.shape[2] == 3: # 4 to 3
            return np.concatenate((img, np.zeros(img[:, :, :1].shape)), axis=2)

def build_canvas(canvas_size, canvas_data, file_list, channels=3, scale=1.0):
    """
        Returns a binned image
        Arguments
            canvas_size:    int list/ndarray shape(2)
            canvas_data:    int ndarray shape(-1,6) from pybinpack
            file_list:      list of files
            channels:       convert to nb of channels
            scale:          float
    """
    assert channels in [1, 3, 4], "only 1, 3 or 4 channel data handled, fix, input data has: "+str(channels)
    poss = canvas_data[:, 1]
    areas = canvas_data[:, 2]
    indices = canvas_data[:, 0, 0]

    canvas = np.zeros((canvas_size[0], canvas_size[1], channels), dtype=np.uint8)
    if channels == 1:
        canvas = canvas[:, :, 0]

    for j, index in enumerate(indices):
        img = np.array(Image.open(file_list[index]))
        img = fix_channels(img, channels)

        pos = poss[j]
        _to = poss[j] + areas[j]

        # incorrect rotation encoding in binpack: check and fix
        if tuple(areas[j]) != img.shape[:2]:
            canvas_data[j, 0, 1] = 1
            canvas[pos[0]:_to[0], pos[1]:_to[1]] = img.transpose(1, 0, 2)
        else:
            canvas_data[j, 0, 1] = 0
            canvas[pos[0]:_to[0], pos[1]:_to[1]] = img
    if scale < 1:
        return scipy.misc.imresize(canvas, scale)

    return canvas


def display_binned(canvases, binsize, file_list=[], figwidth=20, max_cols=2,
                   noimages=False, max_show_group=20, samples=None, scale=1.0, max_bins=None, verbose=False):
    """
    display binpacked data
    Arguments:
        canvases:   list of int ndarrays    # returned from pack_to_canvas() [index,rot,x,y,w,h]
        binsize:    ndarray/list len=2      # canvases max (x+w, y+h) <= binsize; no asserts
        file_list:  list of files paths     # img_file.shape == rect w,h; no asserts
        figwidth:   int, matplotlib width in inches
        max_cols:   columns for matplotlib display
        noimages:   bool, if True show bins as rect patches
        max_show_group: int, partition plt.show(), matplotlib chokes with large numbers
        samples:    int, show subsample of data, if None show all canvases
        scale:      scale resulting pixels
        max_bins:   show only max_bins bins
        verbose:
    Out:
        canvases cleaned up.

    Todo
        WebGL
        ffmpeg

        TSNE - HUE. S,V, Std, Shape, Feature, Category&,
        Zoomin repack

        SaveOut
            npy
            h5py
                index, roation,
                meanshift
        Scale

        tqdm
    """

    channels = 3

    # process only a subset of the data
    if samples is not None:
        _samples = np.linspace(0, len(canvases) - 1, samples).astype(np.int)
        canvases = [canvases[_s] for _s in _samples]

    if max_bins is not None and 0 < max_bins < len(canvases):
        canvases = canvases[:max_bins]

    start_time = time.time()

    # matplotlib chokes with large num plt.show(), cycle
    bins = len(canvases)
    if bins > max_show_group:
        cycles = int(np.ceil(bins/max_show_group))
        out_canvases = []
        for _c in range(cycles):
            if verbose:
                if _c == 0:
                    L = locals()
                    print('display_binned:')
                    print(' number of canvases:', len(canvases))
                    if samples is not None:
                        print(' chosen samples:', _samples)
                    for loco in L:
                        if loco in ["canvases", "file_list"]:
                            print("  len(", loco, ")", len(L(loco)))
                        else:
                            print('  ', loco, L[loco])
                #print('Show Group:', _c, 'Time:', int(start_time - time.time()))

            _oc = display_binned(canvases[_c*max_show_group: min((_c+1)*max_show_group, bins)],
                                 binsize=binsize, file_list=file_list, figwidth=figwidth,
                                 max_cols=max_cols, noimages=noimages, max_show_group=max_show_group,
                                 samples=None, scale=scale, max_bins=None, verbose=verbose)
            out_canvases += _oc
        return out_canvases

    cols = min(max_cols, bins)
    rows = int(np.ceil(bins/cols))
    fig, ax = plt.subplots(rows, cols, gridspec_kw={'wspace':0.01, 'hspace':0.01})
    imgratio = binsize[0]/binsize[1]
    figsize = (figwidth, 1.03*figwidth*imgratio*rows/cols)
    #plt.rcParams["figure.figsize"] = figsize # this un doesnt update first plot
    fig.set_size_inches(figsize) # use instead
    this_ax = None

    for i, canvas, in enumerate(canvases):
        #grr poor polymorphism in matplotlib
        if len(canvases) == 1:
            this_ax = ax
        elif cols == 1 or rows == 1:
            this_ax = ax[i]
        else:
            this_ax = ax[int(i/cols), i%cols]

        if file_list and not noimages:
            _canvas = build_canvas(binsize, canvas, file_list, channels, scale)
            #print('canvas',i,':', time.time() - start_time, _canvas.shape)
            this_ax.imshow(_canvas)
            #print('imshow',i,':', time.time() - start_time)
            this_ax.set_yticks([])
            this_ax.set_xticks([])

        else:
            poss = canvas[:, 1]
            areas = canvas[:, 2]
            indices = canvas[:, 0, 0]

            _r = Rectangle((0, 0), binsize[1], binsize[0], facecolor='none', alpha=1, edgecolor='k')
            this_ax.add_patch(_r)
            for j, _ in enumerate(indices):
                pos = np.roll(poss[j], 1)
                area = np.roll(areas[j], 1)
                rect = Rectangle(pos, area[0], area[1], facecolor='r', alpha=0.5, edgecolor='k')
                this_ax.add_patch(rect)

            this_ax.set_ylim(-10, binsize[1]+10)
            this_ax.set_xlim(-10, binsize[0]+10)
            this_ax.axis('equal')
    plt.show()
    print('show', i,':', time.time() - start_time)
    return canvases
