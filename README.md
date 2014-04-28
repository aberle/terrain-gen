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
 * Cloud sprite drop-down menu controls which cloud sprite will be used (pointy or round)
 * Fourth slider controls the height of the translucent water plane
* Noise
 * First text entry field controls turbulence (smoothing) passes (range: 2-512)
 * Second field controls noise octaves (range: 1.0-16.0)
 * Third field controls noise octave persistence (range: 1.0-64.0)
 * Fourth field controls noise amplitude (range: 1.0-64.0)
 * Press `Regenerate Terrain` to recalculate heightmap with new noise parameters
* Texturing
 * First slider controls how much each texture "bleeds" into the ones surrounding it
   * Set slider all the way to the left to disable this effect
 * Low / High Bound Sliders:
   * The span of each texture layer may be controlled by altering it's lower/upper height boundaries
   * Using this feature, textures may be reordered entirely
     * To get a better idea of where each texture boundary is, briefly lower the blending level
   * To disable a texture, set both of its sliders all the way to the left
     * Setting a texture's lower bound above the upper bound will also disable the texture


### Features

#### Terrain Generation

#### Terrain Texturing

#### Clouds
To draw the clouds, the first step is to calculate the location of each cloud point. The (x,z) coordinates are determined by a simple bounded random float function, seeded by the current time. The (y) coordinates are calculated using a 2D simplex noise function, taking the vertex's (i,j) array indices as inputs.
Once the (x,y,z) locations for each cloud sprite have been calcuated, they may be drawn. There are two shaders that influence the appearance of the clouds:
* cloud.vert: Changes the (x) position of each cloud sprite in relation to elasped time, to simulate the effect of the clouds blowing across the landscape. Also sets the base color of each sprite.
* cloud.frag: Sets the sprite using a uniform sampler2D passed from the main C++ application. The main application loads two sprite images that may be swapped on the fly from the user interface. Also sets the alpha value of the sprites, which grows and shrinks based on the position of the light in the scene (brighter when the light is high in the sky, dimmer when light is on the horizons).

#### Lighting
