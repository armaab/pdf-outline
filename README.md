# pdf-outline

A simple tool based on [poppler](https://poppler.freedesktop.org/) to extract outline from pdf file.
The output format is the same as that of toc file described in [this](https://github.com/armaab/pdfmark) project.

# Requirements
* g++ or clang++ that supports c++17
* Poppler 24.04.0
* 
On macos use the following command to install it:
```
brew install poppler
```

# Build
If you are using MacOS and poppler is installed using homebrew, run the following commands first:
```
export CPLUS_INCLUDE_PATH=/opt/homebrew/include/
export LIBRARY_PATH=/opt/homebrew/lib
```

Then run
```
cd pdf-outline
mkdir build && cd build
cmake ..
make
```

# Usage
```
./pdf-outline <pdf-file> [> <output-file>]
```
