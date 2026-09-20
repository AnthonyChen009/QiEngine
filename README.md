# QiEngine

A custom game engine built on Vulkan, with a hardware ray tracing path tracer, an ECS-driven scene model, and cross-platform support for Linux and Windows.

## Features

### Rendering
- **Hardware ray tracing pipeline**
- **Forward rasterization pipeline with directional lighting**
- **Dual 2D / 3D rendering pipeline**
- **OBJ mesh loading via tinyobjloader**

### Engine architecture
- **ECS powered by EnTT, with ECS-facing mesh and material systems**
- **Resource management via ResourceCache, ResourceLoader, and PrimitiveMeshLibrary**
- **Abstract GPU buffer interfaces**
- **Event dispatcher and input server**
- **Cross-platform crash handler with stack traces via cpptrace**

## Getting Started
QiEngine has been tested and confirmed working on Windows using MSVC and Linux (Fedora) using GCC. Other platforms and compilers may work but are untested.

### Prerequisites
#### Linux
- A C++20 compiler (GCC)
- CMake 3.21+
- A Vulkan-capable GPU (ray tracing support needed for the RT pipeline)


#### Windows
- A C++20 compiler (MSVC)
- CMake 3.21+
- A Vulkan-capable GPU (ray tracing support needed for the RT pipeline)

### Building From Source
Clone the repo
```bash
git clone https://github.com/crypticdevyt/QiEngine.git
```
> **Note:** Fedora: building SDL3 from source needs the X11 development headers. If configuration fails on missing X11 extension headers, install:

```bash
sudo dnf install libXScrnSaver-devel libXtst-devel
```

#### Linux
```bash
cd QiEngine

# Debug build
cmake --preset linux-gcc-debug
cmake --build --preset linux-gcc-debug

# Release build
cmake --preset linux-gcc-release
cmake --build --preset linux-gcc-release
```

#### Windows
```bash
cd QiEngine

# Debug build
cmake --preset windows-msvc-debug
cmake --build --preset windows-msvc-debug

# Release build
cmake --preset windows-msvc-release
cmake --build --preset windows-msvc-release
```

### Running build
```Bash
#linux
./build/linux-gcc-debug/app/QiApp
./build/linux-gcc-release/app/QiApp

#Windows
./build/windows-msvc-debug/app/Debug/QiApp.exe
./build/windows-msvc-release/app/Release/QiApp.exe
```
> **Note:** QiEngine is a learning project and is very incomplete. There is no editor and no formal documentation, though you can modify the app under /app. In theory you could build a full game with it, but it isn't a practical choice compared to established engines.

## Roadmap
#### Done
- [x] Vulkan renderer with forward rasterization
- [x] Hardware ray tracing path tracer with accumulation
- [x] ECS via EnTT
- [x] SDL3 windowing and input
- [x] Cross-platform crash handler
- [x] Bindless textures and OBJ mesh loading

#### Rendering
- [ ] Better denoising for the path tracer
- [ ] Better material support
- [ ] Shadow mapping for the raster pipeline
- [ ] More mesh formats (glTF)
- [ ] OBJ automatic texture loading

#### Engine
- [ ] Scene serialization (save / load)
- [ ] Physics engine
- [ ] Audio
- [ ] Engine editor
- [ ] Scripting support

#### Tooling and docs
- [ ] Engine editor
- [ ] Engine Documentation


## Contributing

Contributions are welcome. Feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License.
