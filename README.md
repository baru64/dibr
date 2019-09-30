# Depth Image Based Rendering with simple editing

*Parts of code are based on opengl-tutorial.org*

Requirements:
- g++ libx11-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxrandr-dev libxext-dev libxcursor-dev libxinerama-dev libxi-dev libglfw3-dev

Tested on Debian 10/testing and Ubuntu 16.04.

Compilation:
```
cd dibr/
make
```

## Usage

```
./dibr u # draw flat image after transformations, uses sample image
./dibr n # draw sprites in 3d space without transormations, uses sample image

./dibr [u|n] [depth_scale] [bg_filter] [src_image] [src_depth_map] [rgb_key_color]
# depth_scale works only with 'n' flag
./dibr u 0.1 0.0 stilllife-tex.jpg stilllife-depth.jpg 0 0 0
# 0 0 0 stands for black key color
```
