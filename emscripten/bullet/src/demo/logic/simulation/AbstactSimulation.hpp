
#pragma once

#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "demo/logic/simulation/logic/CarData.hpp"

#include "demo/logic/simulation/machineLearning/GeneticAlgorithm.hpp"

#include "demo/utilities/NonCopyable.hpp"

#include "demo/defines.hpp"

#include <functional>

class AbstactSimulation
    : public NonCopyable
{
public:
    using SimpleCallback = std::function<void()>;
    using GenomeDieCallback = std::function<void(unsigned int)>;
    using GenerationEndCallback = std::function<void(bool)>;

    struct Definition
    {
        std::string filename;
        unsigned int genomesPerCore = 0;
        unsigned int totalCores = 0;
        NeuralNetworkTopology neuralNetworkTopology;
        CircuitBuilder::CallbackNoNormals onSkeletonPatch = nullptr;
        CircuitBuilder::CallbackNormals onNewGroundPatch = nullptr;
        CircuitBuilder::CallbackNormals onNewWallPatch = nullptr;

        Definition() = default;
    };

    struct CoreState
    {
        unsigned int delta = 0;
        unsigned int genomesAlive = 0;
    };

public:
    virtual ~AbstactSimulation() = default;

public:
    virtual void initialise(const Definition& def) = 0;

public:
    virtual void update(float elapsedTime, unsigned int totalSteps) = 0;

public:
    virtual unsigned int getTotalCores() const = 0;
    virtual const CoreState& getCoreState(unsigned int index) const = 0;
    virtual const CarData& getCarResult(unsigned int index) const = 0;
    virtual unsigned int getTotalCars() const = 0;

public:

#if defined D_WEB_WEBWORKER_BUILD
    virtual void setOnWorkersReadyCallback(SimpleCallback callback) = 0;
#endif

    virtual void setOnGenerationResetCallback(SimpleCallback callback) = 0;
    virtual void setOnGenerationStepCallback(SimpleCallback callback) = 0;
    virtual void setOnGenomeDieCallback(GenomeDieCallback callback) = 0;
    virtual void setOnGenerationEndCallback(GenerationEndCallback callback) = 0;

public:
    virtual const Genomes& getGenomes() const = 0;
    virtual const Genome& getBestGenome() const = 0;
    virtual unsigned int getGenerationNumber() const = 0;
    virtual const glm::vec3& getStartPosition() const = 0;
    virtual const GeneticAlgorithm& getGeneticAlgorithm() const = 0;
};
