//Simplest GLSL fragment shader
#version 130

in vec3 norm;
in vec3 ldir;

uniform struct Material
 { vec4  diffuse;
 } material;

void main(void)
 { float l = dot(normalize(norm), ldir);
   vec4 color = clamp(l * (material.diffuse), 0.1, 1.0);
   gl_FragColor = color;
 }
