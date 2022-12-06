
#pragma once

#include "demo/logic/simulation/AbstactSimulation.hpp"
#include "demo/logic/simulation/logic/CarAgent.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "machineLearning/NeuralNetwork.hpp"

// #include "demo/logic/simulation/logic/physic/PhysicWorld.hpp"

#include "framework/physic/PhysicWorld.hpp"

#include "framework/multithreading/Producer.hpp"

#include <array>
#include <list>
#include <memory>
#include <vector>

class PthreadSimulation : public AbstactSimulation {
private:
  unsigned int _totalCores = 0;

  multithreading::Producer _multithreadProducer;

  /**
   * need multiple physic worlds so that we can divide the
   * physical update and raycasting inside different threads
   */
  std::vector<std::unique_ptr<PhysicWorld>> _physicWorlds;

  std::vector<AbstactSimulation::CoreState> _coreStates;

  unsigned int _genomesPerCore = 0;

  CarAgents _carAgents;
  CarDatas _carsData;

  CircuitBuilder _circuitBuilder;

  CircuitBuilder::StartTransform _startTransform;

  bool _isFirstGenerationFrame = true;

private:
  GeneticAlgorithm _geneticAlgorithm;

  struct Callbacks {
    AbstactSimulation::SimpleCallback onResetAndProcess;
    AbstactSimulation::SimpleCallback onProcessStep;
    AbstactSimulation::GenomeDieCallback onGenomeDie;
    AbstactSimulation::GenerationEndCallback onGenerationEnd;
  } _callbacks;

public:
  PthreadSimulation() = default;
  virtual ~PthreadSimulation();

public:
  virtual void initialise(const Definition& def) override;

private:
  void _resetPhysic();

public:
  virtual void update(float elapsedTime, unsigned int totalSteps) override;
  virtual void breed() override;
  virtual bool isGenerationComplete() const override;

private:
  void _updateCarResult();

public:
  virtual unsigned int getTotalCores() const override;
  virtual const AbstactSimulation::CoreState&
  getCoreState(unsigned int index) const override;
  virtual const CarData& getCarResult(unsigned int index) const override;
  virtual unsigned int getTotalCars() const override;

public:
  virtual void setOnGenerationResetCallback(
    AbstactSimulation::SimpleCallback callback) override;
  virtual void setOnGenerationStepCallback(
    AbstactSimulation::SimpleCallback callback) override;
  virtual void setOnGenomeDieCallback(
    AbstactSimulation::GenomeDieCallback callback) override;
  virtual void setOnGenerationEndCallback(
    AbstactSimulation::GenerationEndCallback callback) override;

public:
  virtual const Genomes& getGenomes() const override;
  virtual const Genome& getBestGenome() const override;
  virtual unsigned int getGenerationNumber() const override;
  virtual const glm::vec3& getStartPosition() const override;
  virtual const GeneticAlgorithm& getGeneticAlgorithm() const override;
};
