## Nicholas Aberle: Simplex Noise Terrain Generation

### To compile and run
```
qmake
make
./tGen
```

### Controls
* **View**
 * Click on the scene and drag mouse to change view orientation
 * Scroll mouse wheel up and down to change zoom level
* **Environment**
 * First slider controls how quickly time elapses, set to slowest by default
    * Slide all the way to the left to freeze time
    * Time scale influences the day/night cycle as well as cloud movement
 * Second slider controls the alpha value of the cloud sprites
 * Third slider controls the number of cloud sprites drawn
 * Cloud sprite drop-down menu controls which cloud sprite will be used (pointy or round)
 * Fourth slider controls the height of the translucent water plane
* **Noise**
 * First text entry field controls turbulence (smoothing) passes (range: 2-512)
 * Second field controls noise octaves (range: 1.0-16.0)
 * Third field controls noise octave persistence (range: 1.0-64.0)
 * Fourth field controls noise amplitude (range: 1.0-64.0)
 * Press `Regenerate Terrain` to recalculate heightmap with new noise parameters
* **Texturing**
 * First slider controls how much each texture "bleeds" into the ones surrounding it
   * Set slider all the way to the left to disable this effect
 * Low / High Bound Sliders:
    * The span of each texture layer may be controlled by altering its lower/upper height boundaries
    * Using this feature, textures may be reordered entirely
    * To get a better idea of where each texture boundary is, briefly lower the blending level
    * To disable a texture, set both of its sliders all the way to the left
      * Setting a texture's lower bound above the upper bound will also disable the texture


### Program Features


#### Terrain Generation


#### Terrain Texturing
The texturing of the terrain is done entirely by shaders. There are three shaders that control the texturing:
* **light.vert**: Calculates Phong lighting intensity used by the other two texturing shaders. The final intensity value is modified by a "light factor" which is simply a multiplier derived from the light's position in the sky. When the light is higher in the sky, the light intensity is greater.
* **water.frag**: This shader controls the appearance of the water plane. It sets the color of the plane based on the current day/night cycle of the scene. During the night sequence, the color's brightness is dimmed. This shader also sets the water's alpha value, making it appear slightly translucent. This effect is made possible by enabling alpha blending in the main C++ application.
* **terrain_texture.frag**: This shader does most of the heavy lifting by texturing the actual terrain itself. Four different textures are passed into the shader via uniform variables: sand, grass, rock, and snow. These textures are derived from JPG images that have been loaded by the main C++ application and stored in the available GL_TEXTURE slots. Also passed to the shader are uniform variables that define the upper and lower boundaries of each segment's "area of influence." If the height of the terrain falls in that area, the accompanying texture is sampled to create the color value of that area. The shader uses additive sampling, meaning that if two areas of influence overlap, the color will be the sum of each texture sample. In addition, I am using the `smoothstep()` function to create smooth transitions between each texturing layer. If the height of the terrain falls within the `smoothstep()` blending interval, a Hermite interpolation value between 0 and 1 is calculated and multiplied by the sample value (the closer to the lower edge, the lesser the influence of the texture on the color, opposite for the upper edge). The upper and lower bounds for each texture layer, as well as the size of the `smoothstep()` blending regions are modifiable by the user using the application's Qt GUI via sliders. Like the water shader, this texturing shader also has a notion of the day/night cycle (again, via a uniform variable). During the night, the overall color brightness is dimmed and given a slightly blue hue to emulate moonlight.


#### Clouds
To draw the clouds, the first step is calculating the location of each cloud point. The (x,z) coordinates are determined by a simple bounded random float function, seeded by the current `time()`. The (y) coordinates are calculated using a 2D simplex noise function, taking the vertex's (i,j) array indices as inputs.
Once the (x,y,z) locations for each cloud sprite have been calcuated, they may be drawn. There are two shaders that influence the appearance and behavior of the clouds:
* **cloud.vert**: Changes the (x) position of each cloud sprite in relation to elasped time, to simulate the effect of the clouds being blown by wind across the landscape. Also sets the base color of each sprite.
* **cloud.frag**: Sets the sprite using a uniform sampler2D passed from the main C++ application. The main application loads two sprite images that may be swapped on the fly from the user interface. Also sets the alpha value of the sprites, which grows and shrinks based on the position of the light in the scene (brighter when the light is high in the sky, dimmer when light is on the horizon).


#### Special Effects
Two more features that have not yet been discussed are the lighting on the water and the color of the skybox. 

To light the water plane, the same light source is used as the terrain's lighting, but with modified settings. To get more accurate-looking water lighting, I reduced the amount of diffuse light slightly and increased both specular and ambient lighting. The specular light especially adds to the "shininess" of the water.

Like many other aspects of this application, the skybox is also affected by the position of the light. Rather than using images to texture the skybox, I opted to use plain colors instead. The color of the skybox changes as the sun rises and sets in the sky. During sunrise and sunset, the sky has a more reddish hue, while at midday it is a clear blue. Since no moonrise or moonset effects are typically observed in the real world, I opted to color the sky a dark blue color throughout the night cycle. In addition, the brightness of the skybox color (both at night and during the day) dims when the light is low on the horizon and is at its brightest when the light is high in the sky.
