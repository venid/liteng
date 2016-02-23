// Simplest GLSL vertex shader
#version 130

in vec3 in_vert;
in vec4 in_color;

out vec4 color;

// параметры преобразований
uniform struct Transform
 { mat4 PVM;
   mat4 model;
 } transform;

void main(void)
 { color = in_color;
   vec4 vertex = transform.model * vec4(in_vert, 1.0);
   gl_Position = transform.PVM * vertex;
 }
