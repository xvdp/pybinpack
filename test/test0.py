"""
xvdp 2018
"""
import sys
import time
import os
import numpy as np

class Colors:
    AU = '\033[0m'
    RB = '\033[91m\033[1m'
    RB = '\033[91m\033[1m'
    GB = '\033[92m\033[1m'
    YB = '\033[93m\033[1m'


try:
    import pybinpack as pb
except:
    print(Colors.RB,"compile failed!? pybinpack not found", Colors.AU)
    raise 


def check_version():
    print('\n--------------')
    print('python', sys.version_info[0:3])
    print('numpy ', np.__version__)
    print(Colors.GB, 'pybinpack', pb.__version__, Colors.AU)

    pf = pb.__file__
    age = (time.time() - os.stat(pf).st_mtime)
    print('\tage: %02d:%02d:%02d'%((age//3600), ((age//60)%60), age%60))
    #pd = time.ctime(os.stat(pf).st_mtime
    print(pf)
    print('--------------\n')

def print_test(res):
    if len(res):
        print("output: array of [Bin, index, x, y, width, height ]:\n", res)
        print(Colors.GB, "\tTest Success", Colors.AU)
    else:
        print(Colors.RB, "\tTest Fail", Colors.AU)

def run_test():
    """
    simple tests, not comprehensive, but should check that a basic functionality of pybinpack works
    """
    ar = [[120, 120], [134, 162], [120, 120], [134, 162], [120, 120],[134, 162], [120, 120],
          [134, 162], [134, 162], [120, 120], [134, 162], [5, 5], [150, 150], [5, 5]]
    print(Colors.YB, " Testing pybinpack: ", Colors.AU)
    print('input: array of [[width, height]]', ar)

    print(Colors.YB, " Test 1: ", Colors.AU)
    b = pb.Binnit(0,0)
    out = b.Pack(ar, method=0, heuristic=0, verbose=True)
    print_test(out)

    print(Colors.YB, "\n Test 2: ", Colors.AU)
    b = pb.Binnit(0,0)
    out = b.Pack(ar, method=1, heuristic=0, verbose=True)
    print_test(out)

    print(Colors.YB, "\n Test 3: ", Colors.AU)
    b = pb.Binnit(500, 500)
    out = b.Pack(ar, method=0, heuristic=0, verbose=True)
    print_test(out)

if __name__ == "__main__": 
    run_test()
    check_version()
