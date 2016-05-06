
#pragma once

#include <math.h>
#include <glm/glm.hpp>

namespace Noise
{

inline float bias( float x, float a )
 { return x / ((1/a - 2)*(1 - x) + 1); }

inline float gain( float x, float a )
 { if( x < 0.5 )
    return x / ((1/a - 2)*(1 - 2*x) + 1);
   else
    return ((1/a - 2)*(1 - 2*x) - x) / ((1/a - 2)*(1 - 2*x) - 1);
 }

inline float gaussian( float x, float stdDev )
 { return exp( -x*x / (2*stdDev*stdDev) ); }



#define SAMPLE_SIZE 512

class Perlin
 { private:
    int   m_octaves;
    float m_frequency;
    float m_amplitude;

    int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
    float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
    float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
    float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];

    void init(int seed);

    float noise(float);
    float noise(glm::vec2 &);
    float noise(glm::vec3 &);

    template <typename Type>
    float frac_noise(Type &arg)
     { float result = 0.f;
       float amp = m_amplitude;
       Type vec = arg * m_frequency;

       for(int i = 0; i < m_octaves; i++)
        { result += noise(vec) * amp;
          vec *= 2.0f;
          amp *= 0.5f;
        }
       return result;
     }

   public:
    Perlin(int seed);

    void setOctaves(int n)
     { m_octaves = n; }
    void setWave(float freq, float amp)
     { m_frequency = freq;
       m_amplitude = amp;
     }

    inline float get(float n)
     { return frac_noise(n); }
    inline float get(glm::vec2& v)
     { return frac_noise(v); }
    inline float get(glm::vec3& v)
     { return frac_noise(v); }

    inline float getClean(float n)
     { return noise(n); }
    inline float getClean(glm::vec2& v)
     { return noise(v); }
    inline float getClean(glm::vec3& v)
     { return noise(v); }
 };

// -----------------------------------------------

//float cell( float x );
//float cell( glm::vec2& x );
//float cell( glm::vec3& x );

//void  cellN( const glm::vec2& x, int n, float* values );
//void  cellN( const glm::vec3& x, int n, float* values );

//glm::vec2 cell2( float x );
//glm::vec2 cell2( glm::vec2& x );

//glm::vec3 cell3( float x );
//glm::vec3 cell3( glm::vec3& x );

}
