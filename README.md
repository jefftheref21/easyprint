# EasyPrint for Clubs

This repository helps club organizers print a bunch of small flyers on a single piece of paper for distribution. No longer do you have to deal with extra margins or spacing issues. Currently works with image formats png, jpeg, and webp and also pdfs. Also, sometimes on Linux, printing multiple copies can be... inconveninet, so instead just print one copy with multiple pages.

### Build (Cmake)

```bash
mkdir build
cd build
cmake ..
make
```
OR

## Compile command (link libraries)

```bash
g++ main.cpp -o easyprint $(pkg-config --cflags --libs opencv4 cairo poppler-cpp)
```

## Usage
```bash
./easyprint [input.png|pdf] [output.pdf] [num_pages] [images_per_page]
```
