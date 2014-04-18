## Nicholas Aberle: Simplex Noise Terrain Generation

### To compile and run
```
qmake
make
./tGen
```

### Controls
* View
 * Drag first slider to change theta angle of the terrain
 * Drag second slider to change phi angle of the terrain
 * Drag third slider to change scale of the terrain
* Noise
 * First text entry field controls turbulence passes (range: 2-256)
 * Second field controls noise octaves (range: 1.0-64.0)
 * Third field controls noise octave persistence (range: 1.0-64.0)
 * Fourth field controls noise amplitude (range: 1.0-64.0)

### Features (both completed and to-be completed)
* Completed
 * Noise computations implemented
 * Use 2D noise to generate untextured terrain
 * Implement user control over noise parameters using Qt GUI elements
 * Lighting
* **What remains**
 * Add keyboard/mouse controls to control view (maybe the Qt sliders are sufficient?)
 * Display generated noise texture in separate viewing window (should be easy now that I have the noise data in a 2D array)
 * Add shadows
 * Use 3D noise to generate cloud particles
 * Create shaders to programmatically texture the landscape
 * User control over texturing options using Qt GUI elements
