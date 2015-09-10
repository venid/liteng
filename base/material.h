
#pragma once

#include "object.h"
#include <glm/glm.hpp>

#define MAP_MAX 4


class Shader;
class Texture;

class Material : public Object
 { private:
    int flags;
    glm::vec4 diffuse;
    glm::vec4 ambient;
    glm::vec4 specular;
    glm::vec4 emission;
    float shininess;

    Texture *map[MAP_MAX];

   public:
    Material(const char* theName);
    ~Material();

    void setDiffuse(glm::vec4 &diff);
    void setAmbient(glm::vec4 &amb);
    void setSpecular(glm::vec4 &spec);
    void setEmission(glm::vec4 &em);
    void setShininess(float shin);

    void setMap(int no, Texture *tx);

    bool init();
    void clear();
    void bind(Shader*);

    static Meta::Base Instance;
 };

