
#include "Genome.hpp"

unsigned int Genome::_currentId = 1;

// default ctor
Genome::Genome()
{
    id = Genome::_currentId++;
}

// copy ctor
Genome::Genome(const Genome& other)
{
    id = other.id;
    fitness = other.fitness;
    weights = other.weights;
}

// move ctor
Genome::Genome(Genome&& other)
{
    std::swap(id, other.id);
    std::swap(fitness, other.fitness);

    weights = std::move(other.weights);
}

// copy operator
Genome& Genome::operator=(const Genome& other)
{
    id = other.id;
    fitness = other.fitness;
    weights = other.weights;

    return *this;
}

// move operator
Genome& Genome::operator=(Genome&& other)
{
    if (this != &other)
    {
        std::swap(id, other.id);
        std::swap(fitness, other.fitness);

        weights = std::move(other.weights);
    }

    return *this;
}
