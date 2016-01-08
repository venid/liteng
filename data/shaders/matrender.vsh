// Simplest GLSL vertex shader
#version 130

in vec3 in_vert;
in vec3 in_norm;
in vec2 in_coord;
in vec3 in_tangent;
in vec3 in_binormal;

out vec3 norm;
out vec3 ldir;
out vec2 coord;

// параметры преобразований
uniform struct Transform
 { mat4 model;
   mat4 PVM;
 } transform;

void main(void)
 { vec3 vertexPos = vec3(transform.model * vec4(in_vert, 1.0));

   vec3 n = normalize(vec3(transform.model * vec4(in_norm, 0.0)));
   vec3 t = normalize(vec3(transform.model * vec4(in_tangent, 1.0)));
   vec3 b = normalize(vec3(transform.model * vec4(in_binormal, 1.0)));

   mat3 tbnMatrix = mat3(t.x, b.x, n.x,
                         t.y, b.y, n.y,
                         t.z, b.z, n.z);

   ldir = normalize(vec3(-0.3, 0.5, -0.6));
   ldir = tbnMatrix * ldir;

   norm = -vertexPos;
   norm = tbnMatrix * norm;

   coord = in_coord;
   vec4 vertex = transform.model * vec4(in_vert, 1.0);
   gl_Position = transform.PVM * vertex;
 }
