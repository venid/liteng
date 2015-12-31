// Simplest GLSL vertex shader
#version 130

in vec3 in_vert;
//in vec3 in_norm;
//in vec2 in_coord;

// параметры преобразований
uniform struct Transform
 { mat4 PVM;
 } transform;

void main(void)
 { 
   gl_Position = transform.PVM * vec4(in_vert, 1.0);
 }
