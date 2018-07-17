

#pragma once


#include "Circuit/CircuitBuilder.hpp"

#include "AI/GeneticAlgorithm/GeneticAlgorithm.hpp"


#include "Worker/Client/WorkerClient.hpp"
#include "Worker/workerMessages.hpp"


#include <list>
#include <vector>
#include <functional>


typedef workerServerMessages::t_SimulationResultMessage::t_carResult	t_carResult;


class Simulation
{
public:
	typedef	std::function<void()>	t_callback;

private:
	enum class e_WorkerRequest
	{
		eNone,
		eInitialise,
		eResetAndProcess,
		eProcess,
	}
	m_currentRequest = e_WorkerRequest::eNone;

private:
	NeuralNetworkTopology		m_NeuralNetworkTopology;
	GeneticAlgorithm			m_GeneticAlgorithm;

	std::vector<float>			m_best_weights;

	std::vector<WorkerClient*>	m_pWorkerClients;

	t_callback	m_onResetAndProcessCallback;
	t_callback	m_onProcessCallback;
	t_callback	m_onSmarterCallback;

public:
	Simulation();
	~Simulation();

public:
	void	initialise(
		const std::string& filename,
		CircuitBuilder::t_callback skeleton,
		CircuitBuilder::t_callback ground,
		CircuitBuilder::t_callback wall
	);

public:
	void	update();

private:
	void	initialiseSimulation();
	void	processSimulation();
	void	resetAndProcessSimulation();

public:
	inline void	setOnResetAndProcessCallback(t_callback callback)
	{
		m_onResetAndProcessCallback = callback;
	}

	inline void	setOnProcessCallback(t_callback callback)
	{
		m_onProcessCallback = callback;
	}

	inline void	setOnSmarterCallback(t_callback callback)
	{
		m_onSmarterCallback = callback;
	}

public:
	inline const GeneticAlgorithm::t_genomes&	getGenomes() const
	{
		return m_GeneticAlgorithm.getGenomes();
	}

	inline const GeneticAlgorithm::t_genome&	getBestGenome() const
	{
		return m_GeneticAlgorithm.getBestGenome();
	}

	const t_carResult&	getCarResult(unsigned int index) const;
};
