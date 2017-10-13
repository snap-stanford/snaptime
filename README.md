snaptime
============
## INSTALLATION
To install SnapTime
In the same directory:
- clone the snap-dev-64 repository
    ```git clone https://github.com/snap-stanford/snap-dev-64.git```
- clone the snap-python-64 repository
    ```git clone https://github.com/snap-stanford/snap-python-64```
- compile the snap-python SWIG
    ```
    cd snap-python-64
    make swig
    cd ..
    ```
- clone the snaptime repositoy
    ```git clone https://github.com/snap-stanford/snaptime.git```
    
## To run SnapTime in C++
- build SnapTime
    ```
       make clean
       make SnapTime.o
    ```
- Include the SnapTime executable and directory in C++ script
## To run SnapTime in Python
- build SnapTime SWIG
    ```
    cd snaptime/src
    make clean
    make _SnapTime.so
    ```
- make sure SnapTime.py and \_SnapTime.so are accessible from python path

## Documentation
- See docs folder for documentation on C++/Python API, schema definition
