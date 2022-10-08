# mikraytrace

A weekend project to build a simple raytracer in C++. 

<img src="./sample.png" alt="Sample image" width="300" />


### Installation instructions

Firstly, install the required libraries and tools. In Debian, or in 
a Debian-like Linux, this can be done like so:

```
apt-get install build-essential cmake libpng++-dev libeigen3-dev libeasyloggingpp-dev libopenbabel-dev lib3ds-dev
```

Secondly, you need some third party libraries. These are "CLI11" for 
command line processing, "cpptoml" for reading configuration files, 
and "toojpeg" for writing images in JPEG format. 

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
a build directory, and run cmake followed by make:

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

