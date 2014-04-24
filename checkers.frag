//  Checkers fragment shader

//  Hardwire checker size
const vec3 CheckSize   = vec3(0.30,0.15, 0.0);

//  Model coordinates and light from vertex shader
varying float LightIntensity;
varying vec3  ModelPos;
uniform int moon;

void main()
{
   //  Divide by checker size for unit coordinates
   vec3 position = ModelPos / CheckSize;

   vec3 green = vec3(0.0, 1.0, 0.0);
   vec3 yellow = vec3(1.0, 1.0, 0.0);
   vec3 red = vec3(1.0, 0.0, 0.0);

   vec3 color = yellow;

   if (position.y > 300.0)
   {
      color = red;
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
