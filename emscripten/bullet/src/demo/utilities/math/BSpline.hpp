
#pragma once

#include <vector>
#include <functional>

class BSpline
{
public:
    struct t_def
    {
        const float* knotsData;
        std::size_t knotsLength;
        unsigned int dimension;
        unsigned int degree;
    };

public:
    void initialise(const t_def& def);
    float calcAt(float coef, unsigned int dimension) const;

private:
    typedef float (*t_baseFunc)(float);

    t_def _def;
    t_baseFunc _baseFunc = nullptr;
    unsigned int _rangeInterval;
};
