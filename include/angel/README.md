# AnGeL - Asset loader for nGL 
(Alternatively, AnGL or angel)

This is a library to easily load obj files into nGL for rendering on TI-Nspire CX / CX II systems. It is meant to be an extension to the preexisting nGL library, here https://github.com/Vogtinator/nGL/

Summary
--------
- AnGeL uses tar.gz files for storing video game assets, including Wavefront object files, textures, text, or possibly even sound.
This library was created with nGL compatiblity first, so it contains the framework to load your 3D models into a scene them on the fly.
- By storing files separate from the executable on a tarball, you can reduce the size of a program drastically (And it's compile time) compared to using nGL's python converter. (Sorry Vogt)
- Only OBJ files with PNG textures are supported for now. (JPG support coming later)


![full_demo](https://github.com/tobleroneaddict/angel/blob/main/resource/full-demo.gif)

Examples
--------
example.cpp in the root directory has some information on getting started with the library.
