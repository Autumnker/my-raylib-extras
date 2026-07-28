# my-raylib-extras

A collection of single-header utility libraries for [raylib](https://github.com/raysan5/raylib).

## Libraries

| Language | Library | Description |
|----------|---------|-------------|
| C++ | **[rmouse](src/cpp/rmouse/)** | Mouse click / double-click / drag detection |

## Structure

```
my-raylib-extras/
├── CMakeLists.txt                  # builds all examples (C / C++)
├── src/
│   ├── c/                          # ★ C header-only libraries
│   │   └── <future-c-lib>/
│   │       ├── <lib>.h
│   │       └── README.md
│   └── cpp/                        # ★ C++ header-only libraries
│       ├── rmouse/
│       │   ├── rmouse.h
│       │   └── README.md
│       └── <future-cpp-lib>/
│           ├── <lib>.h
│           └── README.md
└── examples/
    ├── c/                          # ★ C examples (example_<name>.c)
    │   └── <future-c-example>/
    │       ├── example_<name>.c
    │       └── build.bat
    └── cpp/                        # ★ C++ examples (example_<name>.cpp)
        ├── rmouse/
        │   ├── example_rmouse.cpp
        │   └── build.bat
        └── <future-cpp-example>/
            ├── example_<name>.cpp
            └── build.bat
```

## Adding a new library

### C library

1. Create `src/c/<name>/<name>.h` and `src/c/<name>/README.md`
2. Create `examples/c/<name>/example_<name>.c`
3. Add `add_c_example(<name>)` to `CMakeLists.txt`

### C++ library

1. Create `src/cpp/<name>/<name>.h` and `src/cpp/<name>/README.md`
2. Create `examples/cpp/<name>/example_<name>.cpp`
3. Add `add_cpp_example(<name>)` to `CMakeLists.txt`

That's it — the CMake helpers handle include paths and linking automatically.

## License

zlib/libpng — same as raylib. See [LICENSE](LICENSE).
