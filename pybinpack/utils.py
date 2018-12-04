import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from PIL import Image



def pack_to_canvas(pack):
    repacked = pack.reshape(-1, 3, 2)
    posmax = (repacked[:, 1] + repacked[:, 2]).max(axis=0)
    return repacked, posmax

def plot_bin(repacked, posmax): # moved to pb.utils.plot_bin(pack)
#     repacked = pack_array.reshape(-1, 3, 2)
#     posmax = (repacked[:, 1] + repacked[:, 2]).max(axis=0)

    fig, ax = plt.subplots(1)
    _r = Rectangle((0, 0), posmax[1], posmax[0], facecolor='none', alpha=1, edgecolor='k')
    ax.add_patch(_r)
    for i, r in enumerate(repacked):
        index = r[0][1]
        pos = np.roll(r[1], 1)
        area = np.roll(r[2], 1)
        rect = Rectangle(pos, area[0], area[1], facecolor='r', alpha=0.5, edgecolor='k')
        ax.add_patch(rect)

    plt.ylim(-10, posmax[1]+10)
    plt.xlim(-10, posmax[0]+10)
    plt.axis('equal')
    plt.show()

def show_bin(repacked, posmax, file_list): #H.file_list
    plt.figure(figsize=(20, 15))
    _canvas = np.zeros((posmax[0], posmax[1], 3), dtype=np.uint8)

    for i, r in enumerate(repacked):
        index = r[0][1]
        pos = r[1]
        to = r[2]+r[1]
        img = np.array(Image.open(file_list[index]))

        #rotate
        if tuple(r[2]) != img.shape[:2]:
            _canvas[pos[0]:to[0], pos[1]:to[1]] = img.copy().transpose(1, 0, 2)
        else:
            _canvas[pos[0]:to[0], pos[1]:to[1]] = img.copy()

    plt.imshow(_canvas)
    plt.show()
