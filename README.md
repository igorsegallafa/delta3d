# Delta3D

Project of a basic Game Engine that I created for learn 3D stuff. The library was made using Directx9 with Shaders and C++.

## License
Licensed under the MIT License.

## Features
* Support for Pixel Shader 2.0 and 3.0.
* Support for Hardware and Software Skinning.
* Support for Lightning Map (Self Illumination map).
* Support for old devices.
* Support for material overlay.
* Support for Vertex Color.
* Supports up to 128 bones in Skinning.
* Support for SMD File Format from Priston Tale game.
* Use of SSE2 for floating optimization.
* Static Quad Tree for Terrain rendering.
* Particle Engine.
* Mesh Rendering Sort (transparent meshes renders last).
* Distance Fade at Pixel Shader.
* Initial implementation of reflection plane.
* Dynamic Lightning.
* Material Transformation (Scroll).
* Material with Animated Texture (sequence of frames).
* Camera implementation.
* Dynamic Event and Timer implementation.
* Frustum Culling.
* Value Animation with Easing.
* Debug Renderer.
* Good performance for scene with a lot of Skinned Meshes and big Terrains.
* Log system.

## Implemented Graphic Classes
* Camera
* Font
* Model
* Mesh
* MeshPart
* Material
* MaterialCollection
* QuadTree
* Terrain
* Texture
* Sprite
* Shader

## Implemented Math Classes
* Bounding Box
* Color
* Easing
* Frustum
* Matrix
* Plane
* Quad
* Quaternion
* Rectangle
* Sphere
* Vector

## References
* [Value Animation](https://github.com/igorsegallafa/ValueAnimation)

## Dependencies
* [Effekseer](https://github.com/effekseer/Effekseer)
* [pugixml](https://github.com/zeux/pugixml)

## Documentation
You can generate the library documentation using Doxygen.

## Credits
* [Urho3D](https://github.com/urho3d/Urho3D) for some Math classes.
* Sander Van Der Poel for support and some Directx code.
