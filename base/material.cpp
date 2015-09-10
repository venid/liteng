
#include "material.h"
#include "shader.h"
#include "texture.h"
#include <GL/gl.h>
#include <GL/glext.h>

#define MAT_DIFFUSE   0x00000001
#define MAT_AMBIENT   0x00000002
#define MAT_SPECULAR  0x00000004
#define MAT_EMISSION  0x00000008
#define MAT_SHININESS 0x00000010
#define MAT_MAP_1     0x00000020
#define MAT_MAP_2     0x00000040
#define MAT_MAP_3     0x00000080
#define MAT_MAP_4     0x00000100
#define MAT_MAP_5     0x00000200
#define MAT_MAP_6     0x00000400
#define MAT_MAP_7     0x00000800
#define MAT_MAP_8     0x00001000

int flag_map[] { MAT_MAP_1, MAT_MAP_2, MAT_MAP_3, MAT_MAP_4,
                 MAT_MAP_5, MAT_MAP_6, MAT_MAP_7, MAT_MAP_8 };


META_METHODS(Material)
META_PROPERTY(Material)
META_OBJECT(Material, Material, &Object::Instance)

Material :: Material(const char* theName) : Object(theName),
                                            diffuse(0.f, 0.f, 0.f, 1.f),
                                            ambient(0.f, 0.f, 0.f, 1.f),
                                            specular(0.f, 0.f, 0.f, 1.f),
                                            emission(0.f, 0.f, 0.f, 1.f),
                                            shininess(0.f)
 { flags = 0;
   for(int i = 0; i < MAP_MAX; i++)
    map[i] = nullptr;

   metaClass = &Instance;
 }

Material:: ~Material()
 { for(int i = 0; i < MAP_MAX; i++)
    if(map[i] != nullptr) map[i]->release();
 }

void Material :: setDiffuse(glm::vec4 &diff)
 { diffuse = diff;
   flags |= MAT_DIFFUSE;
 }

void Material :: setAmbient(glm::vec4 &amb)
 { ambient = amb;
   flags |= MAT_AMBIENT;
 }

void Material :: setSpecular(glm::vec4 &spec)
 { specular = spec;
   flags |= MAT_SPECULAR;
 }

void Material :: setEmission(glm::vec4 &em)
 { emission = em;
   flags |= MAT_EMISSION;
 }
void Material :: setShininess(float shin)
 { shininess = shin;
   flags |= MAT_SHININESS;
 }


void Material :: setMap(int no, Texture *tx)
 { if(map[no] != nullptr)
    { map[no]->clear();
      map[no]->release();
    }
   map[no] = (Texture*)tx->retain();
   flags |= flag_map[no];
 }


bool Material :: init()
 { for(int i = 0; i < MAP_MAX; i++)
    if(map[i] != nullptr) map[i]->init();

   return true;
 }

void Material :: clear()
 { for(int i = 0; i < MAP_MAX; i++)
    if(map[i] != nullptr) map[i]->clear();
   flags = 0;
 }

void Material :: bind(Shader* shd)
 { if((flags & MAT_MAP_1) == MAT_MAP_1)
    { glActiveTexture(GL_TEXTURE0);
      glBindTexture(map[0]->getTarget(), map[0]->getGL());
      shd->setTexture("material.map1", 0);
    }
   if((flags & MAT_MAP_2) == MAT_MAP_2)
    { glActiveTexture(GL_TEXTURE1);
      glBindTexture(map[1]->getTarget(), map[1]->getGL());
      shd->setTexture("material.map2", 1);
    }
   if((flags & MAT_MAP_3) == MAT_MAP_3)
    { glActiveTexture(GL_TEXTURE2);
      glBindTexture(map[2]->getTarget(), map[2]->getGL());
      shd->setTexture("material.map3", 2);
    }
   if((flags & MAT_MAP_4) == MAT_MAP_4)
    { glActiveTexture(GL_TEXTURE3);
      glBindTexture(map[3]->getTarget(), map[3]->getGL());
      shd->setTexture("material.map4", 3);
    }

   if((flags & MAT_DIFFUSE) == MAT_DIFFUSE)
    shd->setUniformVec("material.diffuse", diffuse);
   if((flags & MAT_AMBIENT) == MAT_AMBIENT)
    shd->setUniformVec("material.ambient", ambient);
   if((flags &  MAT_SPECULAR) ==  MAT_SPECULAR)
    shd->setUniformVec("material.specular", specular);
   if((flags & MAT_EMISSION) == MAT_EMISSION)
    shd->setUniformVec("material.emission", emission);
   if((flags & MAT_SHININESS) == MAT_SHININESS)
    shd->setUniform("material.shininess", shininess);
 }
