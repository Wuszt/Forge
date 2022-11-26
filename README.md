# Forge
Engine whose purpose is to create a sandbox for various algorithms.

### Goals:
- As less external code as possible (as long as it makes sense/it's not boring to implement).
- OOP and DOP hybrid - ECS where it makes sense.
- Long term project with slow, but constant progress.

#

Unit Tests have been moved to separate, private submodule. They're obviously not needed to run an application.

### Current state:

# 11.2022
[![](https://markdown-videos.deta.dev/youtube/gvsBKjrnRLk)](https://youtu.be/gvsBKjrnRLk)
- Spot lights and directional lights
- Orthographic camera
- Shadows for point, spot and directional lights
- Alpha blending
- Self made RTTI
- FBXes loading
- Skeletal animations

# 03.2022
![](Journal/040322/state.gif)
- Deferred shading
- IMGUI debug with G-Buffer preview
- Smooth runtime switch between Forward<->Deferred mode thanks to one, macro based Uber shader
- Separate rendering passes for Opaque, Transparent (not implented yet) and Non-lit (for drawing in-world debugs) objects

# 12.2021
![](Journal/301221/state.gif)
- ECS Systems, Components and Entities
- Forward rendering
- Ambient + point light with blinn-phong
- Loading obj models (using tinyobj), materials and diffuse textures
- Self-implemented math
- Imgui integration
- Tracy integration
- D3D11 API completely decoupled from engine
