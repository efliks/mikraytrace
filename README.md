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

For reading models in 3D Studio format, install lib3ds and pass the following 
option to CMake:

```
apt-get install lib3ds-dev
cmake -DUSE_LIB3DS=ON ..
```
