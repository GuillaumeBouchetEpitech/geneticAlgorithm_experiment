
#pragma once

#include "demo/defines.hpp"

#include "logic/CircuitBuilder.hpp"

#include "machineLearning/GeneticAlgorithm.hpp"

#include "demo/simulation/logic/t_carData.hpp"

#include <functional>

class AbstactSimulation
{
public:
    typedef	std::function<void()>	    t_callback;
    typedef	std::function<void(bool)>	t_generationEndCallback;

    struct t_def
    {
        std::string filename;
        unsigned int genomesPerCore = 0;
        unsigned int totalCores = 0;
        NeuralNetworkTopology neuralNetworkTopology;
        CircuitBuilder::t_callbackNoNormals onSkeletonPatch = nullptr;
        CircuitBuilder::t_callbackNormals onNewGroundPatch = nullptr;
        CircuitBuilder::t_callbackNormals onNewWallPatch = nullptr;

        t_def() = default;
    };

    struct t_coreState
    {
        unsigned int delta = 0;
        unsigned int genomesAlive = 0;
    };

public:
    virtual ~AbstactSimulation() {};

public:

    virtual void    initialise(const t_def& def) = 0;

public:
    virtual void	update() = 0;

public:
    virtual unsigned int        getTotalCores() const = 0;
    virtual const t_coreState&  getCoreState(unsigned int index) const = 0;
    virtual const t_carData&    getCarResult(unsigned int index) const = 0;
    virtual unsigned int        getTotalCars() const = 0;

public:

#if defined D_WEB_WEBWORKER_BUILD

    virtual void	setOnWorkersReadyCallback(t_callback callback) = 0;

#endif

    virtual void	setOnGenerationResetCallback(t_callback callback) = 0;
    virtual void	setOnGenerationStepCallback(t_callback callback) = 0;
    virtual void	setOnGenerationEndCallback(t_generationEndCallback callback) = 0;

public:
    virtual const GeneticAlgorithm::t_genomes&	getGenomes() const = 0;
    virtual const GeneticAlgorithm::t_genome&	getBestGenome() const = 0;
    virtual unsigned int                        getGenerationNumber() const = 0;

};
