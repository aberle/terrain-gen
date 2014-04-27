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

void main()
{
   vec3 position = ModelPos;

   vec3 color = texture2D(grass_texture, texture_coordinate0.st).rgb;
   
   if (position.y > 50.0)
   {
      color = texture2D(rock_texture, texture_coordinate0.st).rgb;
   }

   vec3 sand  =(smoothstep(-30.0, 45.0, position.y) - smoothstep(25.0, 35.0, position.y)) * texture2D(sand_texture, texture_coordinate0.st).rgb;
   vec3 grass = (smoothstep(20.0, 50.0, position.y) - smoothstep(35.0, 50.0, position.y)) * texture2D(grass_texture, texture_coordinate0.st).rgb;
   vec3 rock  = (smoothstep(30.0, 50.0, position.y) - smoothstep(55.0, 90.0, position.y)) * texture2D(rock_texture, texture_coordinate0.st).rgb;
   vec3 snow  = (smoothstep(50.0, 80.0, position.y) - smoothstep(80.0, 150.0, position.y)) * texture2D(snow_texture, texture_coordinate0.st).rgb;


   color = vec3(0.0, 0.0, 0.0) + grass + rock + snow + sand;

   float multiplier = 1.0;

   if (moon == 1)
   {
      color.z += .10;
      multiplier = 0.5;
   }

   color *= LightIntensity * multiplier;
   //  Pad color with alpha
   gl_FragColor = vec4(color,1.0);
}
