// Large point texture

#version 120

uniform sampler2D sprite;
varying float shareTime;

void main()
{
   // Sample texture
   vec4 color = texture2D(sprite,gl_PointCoord.st);

   //  Set alpha as 1/3 the color
   color.a = 1.0; //0.3*length(color.rgb);
   //  Blend point color with texture
   gl_FragColor = gl_Color*color;
}
