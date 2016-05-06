
#pragma once

#include "object.h"

class Data;
class Image;

class Texture : public Object
 { private:
    unsigned int m_target;
    unsigned int m_gl;
    unsigned int m_init;

    Image* image;

   public:
    Texture(const char *theName);
    ~Texture ();

    bool init();
    void clear();

    void addImage(Image *img);

    bool isInit() { return m_init; }

    unsigned int getTarget() const { return m_target; }
    unsigned int getGL() const { return m_gl; }

    static Meta::Base Instance;
 };
