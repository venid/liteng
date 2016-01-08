//Simplest GLSL fragment shader
#version 130

in vec3 norm;
in vec3 ldir;
in vec2 coord;

uniform struct Material
 { sampler2D map1;
   sampler2D map2;
   vec4  diffuse;
   vec4  ambient;
   vec4  specular;
 } material;

void main(void)
 { vec3 l = normalize(ldir);

   vec3 n = normalize(texture2D(material.map2, coord).xyz * 2.0 - 1.0);
   vec3 v = normalize(norm);
   vec3 h = normalize(l + v);

   float nDotL = max(0.0, dot(n, l));
   float nDotH = max(0.0, dot(n, h));
   float power = (nDotL == 0.0) ? 0.0 : pow(nDotH, 90.0);

   vec4 ambient = material.ambient;
   vec4 diffuse = material.diffuse * nDotL;
   vec4 specular = material.specular * power;
   vec4 color = ambient + diffuse + specular;

   gl_FragColor = color * texture2D(material.map1, coord);

//   vec4 c = texture2D ( material.map1, coord );
//   float l = dot(normalize(norm), ldir);
//   c *= l;
//   vec4 color = clamp(l * (material.diffuse), 0.05, 1.0);
//   gl_FragColor = color * c;
 }
