
#pragma once

#include <vector>
#include <functional>

class BSpline
{
public:
    struct Definition
    {
        const float* knotsData;
        std::size_t knotsLength;
        unsigned int dimension;
        unsigned int degree;
    };

public:
    void initialise(const Definition& def);
    float calcAt(float coef, unsigned int dimension) const;

private:
    using BaseFunc = float (*)(float);

    Definition _def;
    BaseFunc _baseFunc = nullptr;
    unsigned int _rangeInterval;
};
