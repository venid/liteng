//Simplest GLSL fragment shader
#version 130

in vec3 norm;
in vec3 ldir;
in vec2 coord;

uniform struct Material
 { //vec4  ambient;
   sampler2D map1;

   vec4  diffuse;
 } material;

void main(void)
 { vec4 c = texture2D ( material.map1, coord );
   float l = dot(normalize(norm), ldir);
   c *= l;
   vec4 color = clamp(l * (material.diffuse), 0.1, 1.0);
   gl_FragColor = color; // * c;
 }
