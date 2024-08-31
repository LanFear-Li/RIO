# Render In OpenGL

[![CMake Windows](https://github.com/LanFear-Li/RIO/actions/workflows/cmake_windows.yml/badge.svg)](https://github.com/LanFear-Li/RIO/actions/workflows/cmake_windows.yml)
[![CMake Linux](https://github.com/LanFear-Li/RIO/actions/workflows/cmake_linux.yml/badge.svg)](https://github.com/LanFear-Li/RIO/actions/workflows/cmake_linux.yml)

RIO is a real-time rasterization renderer with OpenGL backend. It aims to implement SOTA real-time rendering algorithms for personal education usage. More specifically, RIO will focus on Shading, Lighting & Shadowing, Anti-Aliasing techniques.

## Features

#### Basic

- [x] User Interface

#### Shading

- [x] Blinn-Phong Shading
- [x] Physically-Based Rendering
- [x] Image-Based Lighting / Environment Lighting

#### Lighting & Shadowing

- [x] Point Light
- [ ] Basic Shadow Mapping

## Gallery

**PBR & IBL**

<img src="screenshot\pbr\helmet_hotel.png" alt="helmet_hotel" style="zoom:50%;" />

<img src="screenshot\pbr\helmet_street.png" alt="helmet_street" style="zoom:50%;" />

**Blinn-Phong**

<img src="screenshot\blinn-phong\sponza.png" alt="sponza" style="zoom: 50%;" />

<img src="screenshot\blinn-phong\erato.png" alt="erato" style="zoom:50%;" />

<img src="screenshot\blinn-phong\robot.png" alt="robot" style="zoom:50%;" />

## Usage

**Build**

Since all dependencies are included with source code, you can build RIO from any IDE with cmake on windows. Other platforms are currently not supported.

**Control**

<img src="screenshot\rio_interface.png" alt="rio_interface"  />

Image above shows the Interface of RIO with ImGui. You can modify several **render option** like

* Show skybox / Enable IBL / Render Light Model, etc.
* Switch shading method / Switch skybox.

You can modify **scene option** with camera, all lights and models with

* Switch models / Change model position, rotation, scaling, etc.
* Set camera position, rotation, move speed, etc.
* Set light position, color, intensity, etc.

Additionally, keyboard control for camera and events are supported

* WASD: Travel with **FPS** camera.
* Up / Down: **Increase / Decrease** camera move speed.
* F: Switch from **F**ull screen.
* M: Open / Close GUI panel like **M**ap.
* P: **P**rint current framebuffer and save to project directory.

## Dependencies

**Following dependencies are built from source within RIO.**

- [pep-cprep](https://github.com/PepcyCh/pep-cprep)
- [assimp](https://github.com/assimp/assimp)
- [glfw](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui)
- [json](https://github.com/nlohmann/json)
- [stb](https://github.com/nothings/stb)

## Acknowledgements

- [LearnOpenGL](https://learnopengl.com)
- [renderer](https://github.com/zauonlok/renderer)
- [pep-cprep](https://github.com/PepcyCh/pep-cprep)
- [SoftGLRender](https://github.com/keith2018/SoftGLRender)
