// Simplest GLSL vertex shader
#version 130

in vec3 in_vert;
in vec3 in_norm;
in vec2 in_coord;

out vec3 norm;
out vec3 ldir;

// параметры преобразований
uniform struct Transform
 { mat4 model;
   mat4 PVM;
 } transform;
 
void main(void)
 { vec4 tmp;
   ldir = normalize(vec3(-0.3, 0.5, -0.6));
   tmp = transform.model * vec4(in_norm, 0.0);
   norm = vec3(tmp.x, tmp.y, tmp.z); //in_norm;
   vec4 vertex = transform.model * vec4(in_vert, 1.0);
   gl_Position = transform.PVM * vertex;
 }
