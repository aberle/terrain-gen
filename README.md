## Nicholas Aberle: Simplex Noise Terrain Generation

### To compile and run
```
qmake
make
./tGen
```

### Controls
* View
 * Click on the scene and drag mouse to change view orientation
 * Scroll mouse wheel up and down to change zoom level
* Noise
 * First text entry field controls turbulence (smoothing) passes (range: 2-256)
 * Second field controls noise octaves (range: 1.0-64.0)
 * Third field controls noise octave persistence (range: 1.0-64.0)
 * Fourth field controls noise amplitude (range: 1.0-64.0)

### Features (both completed and to-be completed)
* Completed
 * Noise computations implemented
 * Use 2D noise to generate untextured terrain
 * Implement user control over noise parameters using Qt GUI elements
 * Lighting
 * Display generated noise texture in separate viewing window


* **What remains**
 * Texturing using sampling based on terrain height
 * Texture layers selectable by user
   + Water areas more reflective or have a flat surface or both
 * Particle clouds using 3D noise sampled by shader
 * 3D noise parameters selectable by user
   + Moving clouds
 * ~~Light moves like the sun~~
    + User controlled rate
    + ~~Moonlight phase at "night"~~ 
 * ~~Fix zooming controls for "god mode"~~
 * Fix skybox
 * Shadows (lol)
