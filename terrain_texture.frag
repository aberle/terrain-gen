// Terrain texturing fragment shader

// Model coordinates and light from vertex shader
varying float LightIntensity;
varying vec3  ModelPos;
varying vec2 texture_coordinate0;

uniform int moon;
uniform sampler2D rock_texture;
uniform sampler2D grass_texture;
uniform sampler2D snow_texture;
uniform sampler2D sand_texture;

// Layer height values
uniform float blend;
uniform float sand_low;
uniform float sand_high;
uniform float grass_low;
uniform float grass_high;
uniform float rock_low;
uniform float rock_high;
uniform float snow_low;
uniform float snow_high;

void main()
{
   vec3 position = ModelPos;
   float blend_amt =  blend;

   // Calculate sand texturing
   vec3 sand  = (smoothstep(sand_low -blend_amt, sand_low +blend_amt, position.y) - 
                 smoothstep(sand_high-blend_amt, sand_high+blend_amt, position.y)) 
                 * texture2D(sand_texture, texture_coordinate0.st).rgb;

   // Calculate grass texturing
   vec3 grass = (smoothstep(grass_low -blend_amt, grass_low +blend_amt, position.y) - 
                 smoothstep(grass_high-blend_amt, grass_high+blend_amt, position.y)) 
                 * texture2D(grass_texture, texture_coordinate0.st).rgb;

   // Calculate rock texturing
   vec3 rock  = (smoothstep(rock_low -blend_amt, rock_low +blend_amt, position.y) - 
                 smoothstep(rock_high-blend_amt, rock_high+blend_amt, position.y)) 
                 * texture2D(rock_texture, texture_coordinate0.st).rgb;
   
   // Calculate snow texturing
   vec3 snow  = (smoothstep(snow_low -blend_amt, snow_low +blend_amt, position.y) - 
                 smoothstep(snow_high-blend_amt, snow_high+blend_amt, position.y)) 
                 * texture2D(snow_texture, texture_coordinate0.st).rgb;


   // Add each texture's contribution
   vec3 color = vec3(0.0, 0.0, 0.0) + grass + rock + snow + sand;

   // Lighting
   float multiplier = 1.0;
   if (moon == 1)
   {
      color.z += .20;
      multiplier = 0.75;
   }

   color *= LightIntensity * multiplier;
   //  Pad color with alpha
   gl_FragColor = vec4(color,1.0);
}
