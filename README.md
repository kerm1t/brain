# br [ AI ] n

engine for a RAG system

## How to build

```
git clone --recurse-submodules git@github.com:kerm1t/brain.git
mkdir lib
get latest SDL2, e.g. https://github.com/libsdl-org/SDL/releases/download/release-2.32.6/SDL2-devel-2.32.6-VC.zip
  extract to lib
  update line.23 in CMakeLists "set (SDL2_DIR ./lib/SDL2-2.32.6/cmake)"
mkdir build
cd build
cmake ..
```

## Retrieval-Augmented Generation
