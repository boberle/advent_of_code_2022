# Advent of Code 2022 - Day 9

![animation](video_e.gif)

You can find the animation on [youtube](https://youtu.be/H-LK9nO9uiU).

First and second parts: `day09.c`.

Solutions to test file: 13 (first) and 36 (second)

To make the animation:

- use `day09_animation.c`
- set the macro `WRITE_IMG` to `0`
- according to your input, change the value of the macros `IMG_WIDTH`, `IMG_HEIGHT`, `START_X`, `START_Y`. Start with large values, and reduce as you run the script and look at the line "min x, max x, min y, max y". This way, you can center the the image exactly. Or, if you prefer, you can just select something like `600` for width and height, and recenter with a video editing software.
- create an `images` directory
- set the macro `WRITE_IMG` to `1`
- run the script

Now you will have a lot of images in the `images` directory. You can combine them into a video and a gif with:

```bash
ffmpeg -r 30 -f image2 -i images/img-%05d.ppm -vcodec libx264 -crf 0  -pix_fmt yuv420p video.mp4
ffmpeg -i video.mp4 -f gif video.gif
```
