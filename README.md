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
* Environment
 * First slider controls how quickly time elapses, set to slowest by default
   * Time scale influences the day/night cycle as well as cloud movement
 * Second slider controls the alpha value of the cloud sprites
 * Third slider controls the number of cloud sprites drawn
 * Fourth slider controls the height of the translucent water plane
* Noise
 * First text entry field controls turbulence (smoothing) passes (range: 2-256)
 * Second field controls noise octaves (range: 1.0-64.0)
 * Third field controls noise octave persistence (range: 1.0-64.0)
 * Fourth field controls noise amplitude (range: 1.0-64.0)
 * Press `Regenerate Terrain` to recalculate heightmap with new noise parameters
* Texturing
 * First slider controls how much each texture "bleeds" into the ones surrounding it
   * Set slider all the way to the left to disable this effect
 * Low / High Bound Sliders:
   * The span of each texture layer may be controlled by altering it's lower/upper height boundaries
   * Using this feature, textures may be reordered entirely
     * To get a better idea of where texture boundaries are, lower the blending level
     * Setting a texture's lower bound above the upper bound will disable the texture
   * To disable a texture, set both of its sliders all the way to the left
