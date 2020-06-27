
#pragma once

#include "demo/simulation/AbstactSimulation.hpp"

#include "demo/utilities/threading/Producer.hpp"

#include "demo/simulation/machineLearning/NeuralNetwork.hpp"

#include "demo/simulation/logic/Car.hpp"
#include "demo/simulation/logic/CircuitBuilder.hpp"
#include "demo/simulation/logic/physic/PhysicWorld.hpp"

#include <list>
#include <vector>
#include <array>

class PthreadSimulation
    : public AbstactSimulation
{
private:
    unsigned int    _totalCores = 0;

    Producer*   _multithreadProducer;

    std::vector<PhysicWorld>    _physicWorlds;
    std::vector<AbstactSimulation::t_coreState>    _coreStates;

    unsigned int    _genomesPerCore = 0;

    std::vector<Car>    _cars;
    t_carsData          _carsData;

    CircuitBuilder::t_startTransform    _startTransform;

    bool _isFirstFrame = true;

private:
    GeneticAlgorithm    _geneticAlgorithm;

    struct t_callbacks
    {
        AbstactSimulation::t_callback onResetAndProcess;
        AbstactSimulation::t_callback onProcessStep;
        AbstactSimulation::t_generationEndCallback onGenerationEnd;
    }
    _callbacks;

public:
    PthreadSimulation() = default;
    virtual ~PthreadSimulation();

public:
    virtual void initialise(const t_def& def) override;

public:
    virtual void update() override;

private:
    void updateCarResult();

public:
    virtual unsigned int getTotalCores() const override;
    virtual const AbstactSimulation::t_coreState&  getCoreState(unsigned int index) const override;
    virtual const t_carData& getCarResult(unsigned int index) const override;
    virtual unsigned int getTotalCars() const override;

public:

    virtual void setOnGenerationResetCallback(AbstactSimulation::t_callback callback) override;
    virtual void setOnGenerationStepCallback(AbstactSimulation::t_callback callback) override;
    virtual void setOnGenerationEndCallback(AbstactSimulation::t_generationEndCallback callback) override;

public:
    virtual const GeneticAlgorithm::t_genomes& getGenomes() const override;
    virtual const GeneticAlgorithm::t_genome& getBestGenome() const override;
    virtual unsigned int getGenerationNumber() const override;

};
