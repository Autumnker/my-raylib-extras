# my-raylib-extras

A collection of single-header utility libraries for [raylib](https://github.com/raysan5/raylib).

## Libraries

| Library | Description |
|---------|-------------|
| **[rmouse](src/rmouse/)** | Mouse click / double-click / drag detection |

## Structure

```
my-raylib-extras/
├── CMakeLists.txt              # builds all examples
├── src/                        # header-only libraries
│   ├── rmouse/
│   │   ├── rmouse.h
│   │   └── README.md
│   └── <future-lib>/
│       ├── <future-lib>.h
│       └── README.md
└── examples/                   # one subdirectory per library
    ├── rmouse/
    │   ├── main.cpp
    │   └── build.bat
    └── <future-lib>/
        ├── main.cpp
        └── build.bat
```

## Adding a new library

1. Create `src/<name>/<name>.h` and `src/<name>/README.md`
2. Create `examples/<name>/main.cpp`
3. Add `add_raylib_example(<name>)` to `CMakeLists.txt`

That's it — the CMake helper handles include paths and linking automatically.

## License

zlib/libpng — same as raylib. See [LICENSE](LICENSE).
