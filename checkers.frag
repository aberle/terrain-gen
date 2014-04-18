//  Checkers fragment shader

//  Hardwire checker size
const vec3 CheckSize   = vec3(0.30,0.15, 0.0);

//  Model coordinates and light from vertex shader
varying float LightIntensity;
varying vec3  ModelPos;
uniform int party;

void main()
{
   //  Divide by checker size for unit coordinates
   vec3 position = ModelPos / CheckSize;

   vec3 color = vec3(1.0, 1.0, 0.0);

   vec3 green = vec3(0.0, 1.0, 0.0);
   vec3 yellow = vec3(1.0, 1.0, 0.0);

   if (party == 1)
   {
   	  green.x = 1.0;
   	  green.y = 0.0;
   	  yellow.x = 0.0;
   	  yellow.y = 0.0;
   	  yellow.z = 1.0;
   }

   
   if ((position.x > .05 && position.y > 0.05) || (position.x < .05 && position.y < 0.05))
   {
      if (position.z < 0.5)
      {
         color = green;
      }
      else
      {
         color = yellow;
      }
   }
   else
   {
      if (position.z < 0.5)
      {
         color = yellow;
      }
      else
      {
         color = green;
      }
   }

   color *= LightIntensity;
   //  Pad color with alpha
   gl_FragColor = vec4(color,1.0);
}
