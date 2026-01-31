# EasyPrint for Clubs

This repository helps club organizers print a bunch of small flyers on a single piece of paper for distribution. No longer do you have to deal with extra margins or spacing issues. Currently works with image formats png, jpeg, and webp.

### Installation

```bash
mkdir build
cd build
cmake ..
make
```

## Compiling

```bash
g++ main.cpp -o easyprint $(pkg-config --cflags --libs opencv4 cairo)
```

## Usage
```bash
./easyprint [input.png] [output.pdf] [num_copies] [copies_per_page]
```
