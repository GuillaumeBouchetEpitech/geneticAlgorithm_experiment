
#pragma once

#include <vector>

class Genome
{
private:
    static unsigned int _currentId;

public:
    Genome();
    Genome(const Genome& other);
    Genome(Genome&& other);
    Genome& operator=(const Genome& other);
    Genome& operator=(Genome&& other);

public:
    unsigned int id = 0;
    float fitness = 0.0f;
    std::vector<float> weights;
};

typedef std::vector<Genome> t_genomes;
