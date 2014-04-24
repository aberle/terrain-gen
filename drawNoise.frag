//  drawNoise fragment shader

//  Hardwire checker size
const vec3 CheckSize   = vec3(0.30,0.15, 0.0);
uniform float noise[512];


void main()
{
   vec4 position = gl_FragCoord;

   vec3 color = vec3(0.0, 0.0, 0.0);

   color.x = noise[position.x]/512.0;
   color.y = noise[position.y]/512.0;

   if (noise[464] == 465.0)
   {
      color.z = 1.0;
   }

   //  Pad color with alpha
   gl_FragColor = vec4(color,1.0);
}
