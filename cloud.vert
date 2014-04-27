//  Clouds

#version 120

uniform float time;

void main(void)
{
   // Initial position
   vec4 vert = gl_Vertex;
   vert.y += 0;
   vert.x += 0;
   vert.z -= time;

   // Wrap over
   if (vert.z < -2.5)
   {
      vert.z += 5.0;
   }

   // Color
   gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);

   // Transform particle location
   gl_Position = gl_ModelViewProjectionMatrix*vert;
}
