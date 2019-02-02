# xvdp 2018
#source activate abg
pip uninstall pybinpack -y
rm -r pybinpack.egg-info
rm -r dist
rm -r build
python setup.py install
cd test
python test0.py