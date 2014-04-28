// Large point texture

#version 120

uniform sampler2D sprite;

void main()
{
   // Sample texture
   vec4 color = texture2D(sprite,gl_PointCoord.st);

   //  Set alpha
   color.a = 0.2;

   //  Blend point color with texture
   gl_FragColor = gl_Color*color;
}
