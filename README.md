# Project info
Implementation of TI-DBSCAN and VP-tree-DBSCAN.

# Setup

First time compilation:
``` console
mkdir build
cmake -B build .
cd build
make
./DBSCAN
```

If `build` folder exists and cmake files were created:
``` console
cd build
make
./DBSCAN
```

# Testing
When testing algorithms, to speed the calculations up, remember to change `CMAKE_BUILD_TYPE` from `Debug` to `Release` in the `CMakeLists.txt` file.

# Python visualizer
To make visualization of 2d points easier for testing purposes a python script was added. To run it go to the `scripts` folder and use following commands (on Linux):
``` console
python3 -m venv venv
source venv/bin/activate
python -m pip install -r requirements.txt
python 2d_visualizer.py -f ../data/simple2D.csv -i 2
```
