#version 460 core

in vec2 texCoord;
in float heightPoint;

uniform sampler2D cracked_earth;
uniform sampler2D rock;
uniform sampler2D grass;
uniform sampler2D clover;

out vec4 FragColor;

vec4 CalcTexColor()
{
   vec4 TexColor;

   float Height = heightPoint;

   const float gHeight0 = 1.5f;
   const float gHeight1 = 3.0f;
   const float gHeight2 = 4.5f;
   const float gHeight3 = 6.0f;

   if (Height < gHeight0) 
   {
      TexColor = texture(cracked_earth, texCoord);
   } 
   else if (Height < gHeight1) 
   {
      vec4 Color0 = texture(cracked_earth, texCoord);
      vec4 Color1 = texture(rock, texCoord);
      float Delta = gHeight1 - gHeight0;
      float Factor = (Height - gHeight0) / Delta;
      TexColor = mix(Color0, Color1, Factor);
   } 
   else if (Height < gHeight2) 
   {
      vec4 Color0 = texture(rock, texCoord);
      vec4 Color1 = texture(grass, texCoord);
      float Delta = gHeight2 - gHeight1;
      float Factor = (Height - gHeight1) / Delta;
      TexColor = mix(Color0, Color1, Factor);
   } 
   else if (Height < gHeight3) 
   {
      vec4 Color0 = texture(grass, texCoord);
      vec4 Color1 = texture(clover, texCoord);
      float Delta = gHeight3 - gHeight2;
      float Factor = (Height - gHeight2) / Delta;
      TexColor = mix(Color0, Color1, Factor);
   } 
   else 
   {
      TexColor = texture(clover, texCoord);
   }

   return TexColor;
}


void main()
{
    vec4 TexColor = CalcTexColor();

    FragColor = TexColor;
}