# EasyPrint for Clubs

### Installation

```
mkdir build
cd build
cmake ..
make
```

## Compiling

```
g++ main.cpp -o easyprint $(pkg-config --cflags --libs opencv4 cairo)
```

## Usage
```
./easyprint input.png [output.pdf] [num\_copies] [copies\_per\_page]
```
