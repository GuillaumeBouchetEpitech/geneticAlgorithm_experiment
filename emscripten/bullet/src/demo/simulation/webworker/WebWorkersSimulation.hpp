
#pragma once

#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#   error "exclude this file to build natively or with multi thread support"
#endif

#include "demo/simulation/AbstactSimulation.hpp"

#include "demo/simulation/logic/CircuitBuilder.hpp"

#include "demo/simulation/machineLearning/GeneticAlgorithm.hpp"

#include "producer/WorkerProducer.hpp"
#include "common.hpp"

#include <list>
#include <vector>
#include <array>

class WebWorkersSimulation
    : public AbstactSimulation
{
private:
    enum class WorkerRequest
    {
        eNone,
        eWorkersLoading,
        eResetAndProcess,
        eProcess,
    }
    _currentRequest = WorkerRequest::eNone;

    std::vector<WorkerProducer*> _workerProducers;

    unsigned int _totalCores = 0;
    unsigned int _genomesPerCore = 0;
    unsigned int _totalGenomes = 0;

    std::vector<bool> _carLiveStatus;

private:
    NeuralNetworkTopology _neuralNetworkTopology;
    GeneticAlgorithm _geneticAlgorithm;

    struct t_callbacks
    {
        AbstactSimulation::t_callback onWorkersReady;
        AbstactSimulation::t_callback onGenerationReset;
        AbstactSimulation::t_callback onGenerationStep;
        AbstactSimulation::t_genomeDieCallback onGenomeDie;
        AbstactSimulation::t_generationEndCallback onGenerationEnd;
    }
    _callbacks;

public:
    WebWorkersSimulation() = default;
    virtual ~WebWorkersSimulation() = default;

public:
    virtual void initialise(const t_def& def) override;

public:
    virtual void update() override;

private:
    void _processSimulation();
    void _resetAndProcessSimulation();

public:
    virtual unsigned int getTotalCores() const override;
    virtual const AbstactSimulation::t_coreState& getCoreState(unsigned int index) const override;
    virtual const t_carData& getCarResult(unsigned int index) const override;
    virtual unsigned int getTotalCars() const override;

public:
    virtual void setOnWorkersReadyCallback(AbstactSimulation::t_callback callback) override;
    virtual void setOnGenerationResetCallback(AbstactSimulation::t_callback callback) override;
    virtual void setOnGenerationStepCallback(AbstactSimulation::t_callback callback) override;
    virtual void setOnGenomeDieCallback(AbstactSimulation::t_genomeDieCallback callback) override;
    virtual void setOnGenerationEndCallback(AbstactSimulation::t_generationEndCallback callback) override;

public:
    virtual const GeneticAlgorithm::t_genomes& getGenomes() const override;
    virtual const GeneticAlgorithm::t_genome& getBestGenome() const override;
    virtual unsigned int getGenerationNumber() const override;

};
