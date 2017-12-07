# pdf-outline

A simple tool based on [poppler](https://poppler.freedesktop.org/) to extract outline from pdf file.
The output format is the same as that of toc file described in [this](https://github.com/armaab/pdfmark) project.

# Build

```
git clone git@github.com:armaab/pdf-outline.git
cd pdf-outline
mkdir build && cd build
cmake ..
make
```

# Usage
```
./pdf-outline <pdf-file> [> <output-file>]
```
