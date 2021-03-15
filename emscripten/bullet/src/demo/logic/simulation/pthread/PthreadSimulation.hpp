
#pragma once

#include "demo/logic/simulation/AbstactSimulation.hpp"

#include "./multithreading/Producer.hpp"

#include "demo/logic/simulation/machineLearning/NeuralNetwork.hpp"

#include "demo/logic/simulation/logic/Car.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "demo/logic/simulation/logic/physic/PhysicWorld.hpp"

#include <list>
#include <vector>
#include <array>

class PthreadSimulation
    : public AbstactSimulation
{
private:
    unsigned int _totalCores = 0;

    multithreading::Producer *_multithreadProducer;

    /**
     * need multiple physic worlds so that we can divide the
     * physical raycasting among the different threads
     */
    std::vector<PhysicWorld*> _physicWorlds;

    std::vector<AbstactSimulation::CoreState> _coreStates;

    unsigned int _genomesPerCore = 0;

    std::vector<Car> _cars;
    CarDatas _carsData;

    CircuitBuilder::StartTransform _startTransform;

    bool _isFirstGenerationFrame = true;

private:
    GeneticAlgorithm _geneticAlgorithm;

    struct t_callbacks
    {
        AbstactSimulation::SimpleCallback onResetAndProcess;
        AbstactSimulation::SimpleCallback onProcessStep;
        AbstactSimulation::GenomeDieCallback onGenomeDie;
        AbstactSimulation::GenerationEndCallback onGenerationEnd;
    } _callbacks;

public:
    PthreadSimulation() = default;
    virtual ~PthreadSimulation() = default;

public:
    virtual void initialise(const Definition &def) override;

public:
    virtual void update() override;

private:
    void _updateCarResult();

public:
    virtual unsigned int getTotalCores() const override;
    virtual const AbstactSimulation::CoreState &getCoreState(unsigned int index) const override;
    virtual const CarData &getCarResult(unsigned int index) const override;
    virtual unsigned int getTotalCars() const override;

public:
    virtual void setOnGenerationResetCallback(AbstactSimulation::SimpleCallback callback) override;
    virtual void setOnGenerationStepCallback(AbstactSimulation::SimpleCallback callback) override;
    virtual void setOnGenomeDieCallback(AbstactSimulation::GenomeDieCallback callback) override;
    virtual void setOnGenerationEndCallback(AbstactSimulation::GenerationEndCallback callback) override;

public:
    virtual const Genomes &getGenomes() const override;
    virtual const Genome &getBestGenome() const override;
    virtual unsigned int getGenerationNumber() const override;
    virtual const glm::vec3 &getStartPosition() const override;
};
