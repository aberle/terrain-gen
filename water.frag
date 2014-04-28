// Water fragment shader

// Model coordinates and light from vertex shader
varying float LightIntensity;
uniform int moon;

void main()
{
   vec3 color = vec3(0.00, 0.75, 1.00);
   
   float multiplier = 1.0;
   if (moon == 1)
   {
      multiplier = 0.5;
   }

   color *= LightIntensity * multiplier;
   // Pad color with alpha
   gl_FragColor = vec4(color,0.55);
}
