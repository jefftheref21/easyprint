# EasyPrint for Clubs

This repository helps club organizers print a bunch of small flyers on a single piece of paper for distribution. No longer do you have to deal with extra margins or spacing issues. Currently works with image formats png, jpeg, and webp and also pdfs. Also, sometimes on Linux, printing multiple copies can be... inconvenient, so instead, just print one copy with multiple pages.

## Installing Libraries

Make sure to install OpenCV, Cairo, and Poppler depending on your OS. Just search up your OS install and then the packages below.

## Build (Cmake)
Needs pkg-config.

```bash
mkdir build
cd build
cmake ..
make
```
OR

## Compile command (link libraries)
This might change based on OS.

```bash
g++ main.cpp -o easyprint $(pkg-config --cflags --libs opencv4 cairo poppler-cpp)
```

## Usage
```bash
./easyprint [input.png|pdf] [output.pdf] [num_pages] [images_per_page]
```
