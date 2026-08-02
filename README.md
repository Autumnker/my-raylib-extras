# my-raylib-extras

A collection of single-header utility libraries for [raylib](https://github.com/raysan5/raylib).

## Libraries

| Language | Library | Description |
|----------|---------|-------------|
| C / C++ | **[rmouse](src/rmouse/)** | Mouse click / double-click / drag detection |

## Structure

```
my-raylib-extras/
├── CMakeLists.txt                  # builds all examples (C / C++)
├── src/
│   └── rmouse/
│       ├── rmouse.hpp             # C++ library
│       ├── rmouse.h               # C library
│       └── README.md
└── examples/
    └── rmouse/
        ├── example_rmouse.cpp      # C++ example
        ├── example_rmouse.c        # C example
        └── build.bat
```

## Adding a new library

1. Create `src/<name>/<name>.hpp` (C++) or `src/<name>/<name>.h` (C) and `src/<name>/README.md`
2. Create `examples/<name>/example_<name>.cpp` (or `.c`) and optionally `build.bat`
3. Add `add_cpp_example(<name>)` or `add_c_example(<name>)` to `CMakeLists.txt`

That's it — the CMake helpers handle include paths and linking automatically.

## License

zlib/libpng — same as raylib. See [LICENSE](LICENSE).
