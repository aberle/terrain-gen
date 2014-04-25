//  Checkers fragment shader

//  Model coordinates and light from vertex shader
varying float LightIntensity;
varying vec3  ModelPos;
varying vec2 texture_coordinate0;
uniform int moon;
uniform sampler2D rock_texture;
uniform sampler2D grass_texture;

void main()
{
   //  Divide by checker size for unit coordinates
   vec3 position = ModelPos;

   vec3 color = texture2D(grass_texture, texture_coordinate0.st).rgb;
   
   if (position.y > 50.0)
   {
      color = texture2D(rock_texture, texture_coordinate0.st).rgb;
   }

   float multiplier = 1.0;

   if (moon == 1)
   {
      color.z += .25;
      multiplier = 0.5;
   }

   color *= LightIntensity * multiplier;
   //  Pad color with alpha
   gl_FragColor = vec4(color,1.0);
}
