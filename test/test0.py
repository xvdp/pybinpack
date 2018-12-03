"""
xvdp 2018
"""
import sys
import time
import os
import numpy as np
import pybinpack as pb


def check_version():
    print('\n--------------')
    print('python', sys.version_info[0:3])
    print('numpy ', np.__version__)
    print('pybinpack', pb.__version__)

    pf = pb.__file__
    age = (time.time() - os.stat(pf).st_mtime)
    print('\tage: %02d:%02d:%02d'%((age//3600) ,((age//60)%60), age%60))
    #pd = time.ctime(os.stat(pf).st_mtime
    print(pf)
    print('--------------\n')

def run_test():
    #ar = np.array([[1,2],[3,4],[20,120],[30,40],[20,120],[30,40],[20,120]])
    ar=[[120,120],[134,162],[120,120],[134,162],[120,120],[134,162],[120,120],[134,162],[134,162],[120,120],[134,162],[5,5],[150,150],[5,5]]

    print('numpy array', ar)
    pb.simple_pack(ar, 500, 500)

    b = pb.Binnit(0,0)
    out = b.pack(ar)
    print(out)

if __name__ == "__main__":
    check_version()
    run_test()
    