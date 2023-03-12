# mikraytrace

A weekend project to create a simple raytracer in C++. 

<div align="center">
    <img src="./sample.png" width="300" />
</div>

### Build instructions

Firstly, install the required tools. It may be that you already have
them installed. If not, in Debian or in a Debian-like Linux, you may do:

```
mikraytrace > apt-get install build-essential cmake
```

Secondly, you need some third party libraries. These are:
 * eigen for linear algebra
 * CLI11 for command line processing
 * cpptoml for reading configuration files
 * toojpeg for writing scenes in JPEG format
 * lodepng for loading textures and writing scenes in PNG format

Install them by updating the submodules:

```
mikraytrace > git submodule update --init --recursive
```

Thirdly, you may need example textures. I created a [texture pack](https://drive.google.com/file/d/1e9myBNpWHDAlyTtKmfqpKNdgcAeiuVQ_/view?usp=share_link) based on free textures from OpenGameArt.org.
Create a textures directory and unpack the textures there.

Finally, review CMakeLists.txt. If everything looks okay, create a build directory
and run CMake followed by make:

```
mikraytrace/build > cmake ..
mikraytrace/build > make
```

This should generate an executable file mrtp\_cli. In order to test the program, 
render the example scene:

```
mikraytrace > ./build/mrtp_cli bluemol.toml
```
