# Glasses

This is a program for viewing images "interactively": click and drag to move the image, and scroll to zoom. See `--keybinds` for more. Should support whatever images stb_image.h supports, as long as it can be converted to RGBA.  
&nbsp;&nbsp;The program uses OpenGL hardware acceleration, thus it will not work without a video card that supports OpenGL and appopriate drivers. If you are unsure whether you have that, you probably do.  
&nbsp;&nbsp;I made this for two reasons: I wanted a [feh](https://github.com/derf/feh) that you could use with the mouse (though feh has *far* more uses than just viewing images); and I wanted the ability to change how the images are sampled with a keybind. Also, I wanted to use the Lanczos algorithm, as I'd heard it was the "best" technique.  
&nbsp;&nbsp;However, using the Lanczos sampling method uses significantly more computing power, as it samples the texture **16** times per fragment(!), whereas the others sample only once per fragment. Also, it does some other calculations besides. **Related: see the [fragment shader](/shaders/image.frag) for original author credit/Copyright (it has expired, but still).**

---

## Usage

```shell
$ glasses [switches] [path_to_image]
```

---

## Building

```shell
$ make clean # for subsequent builds
$ make
```

### Dependencies

 - OpenGL 4.6 (you can change the version, if you want) supported video card and drivers.
 - Linux.
 - GLFW3.

---

## Switches

### Short  

|Switch|Effect|
|:---|:---|
|`-l`|shorthand for `--bilinear`|
|`-n`|shorthand for `--nearest`|
|`-h`|shorthand for `--lanczos`|

### Long

|Switch|Effect|
|:---|:---|
|`--bilinear`|start with bilinear sampling|
|`--nearest`|start with nearest neighbour sampling|
|`--lanczos`|start with lanczos sampling|
|`--help`|print the help message|
|`--keybinds`|print the keybinds|

## Keybinds

|Key|Effect|
|:---|:---|
|`Left click and drag`|move image|
|`Scroll wheel`/<kbd>+</kbd>/<kbd>-</kbd>|zoom in and out|
|<kbd>r</kbd>|reset position and zoom|
|<kbd>n</kbd>|set sampling mode to nearest neighbour|
|<kbd>l</kbd>|set sampling mode to bilinear|
|<kbd>h</kbd>|set sampling mode to lanczos|
|<kbd>ESC</kbd>/<kbd>q</kbd>|quit

---

### License/Copying

**[GPL-3.0](/LICENSE)**
