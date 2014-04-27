//  Clouds

#version 120

void main(void)
{
   // Initial position
   vec4 vert = gl_Vertex;
   vert.y += 0.75;
   vert.x += 0.0;
   vert.z += 0.0;

   // Color
   gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);

   // Transform particle location
   gl_Position = gl_ModelViewProjectionMatrix*vert;
}
