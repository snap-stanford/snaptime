snaptime
============

**The description and documentation of SnapVX are at ?.**

#### INSTALLATION
1. Download the latest release [here](http://snap.stanford.edu/time/release/) for your environment. For example, for the linux environment, the file for version 0.0.1 would be snaptime-0.0.1-dev-centosrelease-x64-py2.7.tar.gz

2. Uncompress the `snaptime-*.tar.gz` file.

    tar zxf snaptime-0.0.1-dev-centosrelease-x64-py2.7.tar.gz

3. Run `setup.py install` from the top level directory

    python `setup.py install`

4. Test if the installation succeeded

    ```
    from snaptime import *
    ```

#### DEVELOPMENT
1. Fork the git repository from [here](https://github.com/snap-stanford/snaptime)

2. Make sure you set the path to SNAP directory in the Makefile by changing the `$SNAPDIR` variable.

3. Make sure that your compiler supports C++11

4. Run the makefile.

    ```
    make all
    ```

5. For installing, make with the target install
    ```
    make install
    ```

6. For generating a distribution, make with the target dist
    ```
    make dist
    ```

#### CONTRIBUTING TO THE CPP REPOSITORY
1. Fork the git repository from [here](https://github.com/snap-stanford/snaptime)

2. Go to the `cpp` directory.

    cd cpp

3. For maintaining consistency, define classes in `.hpp` files and functions in `.cpp` files.

4. We use swig for integrating c++ with python. Define your swig module in a `.i` file. For example, if your wish to contribute through a class `mycontribution`, interface it through `mycontribution.i` .

5. Add the `.i` file to `snaptime_helper.i`

6. Follow instructions for making the build

### CONTRIBUTING TO THE PYTHON REPOSITORY
1. Fork the git repository from [here](https://github.com/snap-stanford/snaptime)

2. Go to the `snaptime` directory.

    cd snaptime

2. Write your definitions inside a separate `.py` file in the directory.

2. Modify the `__init__.py` file inside the same directory.

3. Follow instructions for making the build
