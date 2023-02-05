# mikraytrace

A weekend project to create a simple raytracer in C++. 

<img src="./sample.png" alt="Sample image" width="300" />


### Build instructions

Firstly, install the required tools. It may be that you already have
them installed. If not, in Debian or in a Debian-like Linux, you may do:

```
apt-get install build-essential cmake
```

Secondly, you need some third party libraries. These are:
 * eigen for linear algebra
 * CLI11 for command line processing
 * cpptoml for reading configuration files
 * toojpeg for writing scenes in JPEG format
 * lodepng for loading textures and writing scenes in PNG format

Install them by updating the submodules:

```
cd mikraytrace/
git submodule update --init --recursive
```

Thirdly, you may need example textures. Enter the textures directory 
and download them automatically from OpenGameArt.org website. Note that some
downloaded textures are in formats other than PNG and have to be converted
before they can be used. This is done automatically too, but make sure that
you have ImageMagick installed. Otherwise, convert them to PNG format manually.

```
cd mikrayrace/textures/
bash gettex.sh
```

Now you may want to review CMakeLists.txt. If everything looks okay, create
a build directory, and run CMake followed by make:

```
cd mikraytrace/
mkdir build/
cd build/
cmake ..
make
```

This should generate an executable file mrtp\_cli. In order to test the program, 
render the example scene:

```
cd mikraytrace/
./build/mrtp_cli bluemol.toml
```

There are some optional dependencies: Open Babel for handling of MOL2 files, 
lib3ds for reading models in 3D Studio format, and Easylogging++ for having more 
fancy logs. The latter one is already in the submodules. At the moment, Open Babel 
and lib3ds come from the system. You may install them like so:

```
apt-get install libopenbabel-dev lib3ds-dev
```

If you want to use any of the optional dependencies, you should add options to CMake:

```
cmake -DUSE_OPENBABEL=ON -DUSE_LIB3DS=ON -DUSE_EASYLOGGING=ON ..
```
