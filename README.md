# Render In OpenGL

[![CMake Windows](https://github.com/LanFear-Li/RIO/actions/workflows/cmake_windows.yml/badge.svg)](https://github.com/LanFear-Li/RIO/actions/workflows/cmake_windows.yml)
[![CMake Linux](https://github.com/LanFear-Li/RIO/actions/workflows/cmake_linux.yml/badge.svg)](https://github.com/LanFear-Li/RIO/actions/workflows/cmake_linux.yml)

RIO is a real-time rasterization renderer with OpenGL backend. It aims to implement SOTA real-time rendering algorithms for personal education usage. RIO will specifically focus on Shading, Lighting & Shadowing, and Anti-Aliasing techniques.

## Features

#### Basic

- [x] User Interface with Control Panel and Camera Operation

#### Shading

- [x] Classic Blinn-Phong Shading
- [x] Physically-Based Shading with Lambertian BRDF
- [x] Image-Based Lighting (IBL) / Environment Lighting

#### Lighting & Shadowing

- [x] Point / Directional / Spot lights
- [x] Shadow Mapping with Percentage Closer Filtering (PCF)
- [x] Percentage Closer Soft Shadow (PCSS)


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

Since all dependencies are included with source code, you can build RIO freely with CMake on Windows or Linux.

**Control**

<img src="screenshot\rio_interface.png" alt="rio_interface"  />

The image above shows the RIO interface with ImGui. You can modify several **render options** like

- Show skybox / Enable IBL / Render Light Model / Render shadow, etc.
- Switch shading method / Switch shadow method / Switch skybox.

You can modify **scene options** with a camera, all lights, and models with

- Switch models / Change model position, rotation, scaling, etc.
- Set camera position, rotation, move speed, etc.
- Set light position, color, intensity, etc.

Additionally, keyboard control for the camera and events is supported

- WASD: Travel with **FPS** camera.
- Hold Mouse Right: Rotate the camera.
- E / Q: Camera up and down.
- Up / Down: **Increase / Decrease** camera movement speed.
- F: Switch from **F**ull screen.
- M: Open / Close GUI panel like **M**ap.
- P: **P**rint the current framebuffer and save it to the project directory.

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

- [pep-cprep](https://github.com/PepcyCh/pep-cprep)
- [LearnOpenGL](https://learnopengl.com)
- [renderer](https://github.com/zauonlok/renderer)
- [SoftGLRender](https://github.com/keith2018/SoftGLRender)
