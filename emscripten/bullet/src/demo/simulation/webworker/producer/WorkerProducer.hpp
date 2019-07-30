
#pragma once

#include "demo/defines.hpp"

#if not defined D_WEB_WEBWORKER_BUILD
#	error "exclude this file to build natively or with multi thread support"
#endif

#include "demo/simulation/machineLearning/NeuralNetwork.hpp"

#include "demo/simulation/webworker/common.hpp"

#include "demo/utilities/message/MessageBuffer.hpp"
#include "demo/utilities/message/MessageView.hpp"

#include "demo/simulation/logic/t_carData.hpp"

#include "demo/simulation/AbstactSimulation.hpp"

#include "thirdparty/GLMath.hpp"

#include <bitset>

#include <emscripten/emscripten.h> // <= worker_handle

class WorkerProducer
{
public:
    struct t_def
    {
        std::string filename;
        unsigned int genomesPerCore = 0;
        NeuralNetworkTopology neuralNetworkTopology;

        t_def() = default;
    };

private:
    worker_handle	_workerHandle;

    enum e_Status
    {
        eWebWorkerLoaded = 0,
        eProcessing,
        eUpdated,
        eCount
    };

    std::bitset<e_Status::eCount>	_flags;

    AbstactSimulation::t_coreState  _coreState;

    t_carsData	_carsData;

    // typedef std::pair<glm::vec3, glm::vec3>	t_contact;
    // typedef std::vector<t_contact>			t_contacts;
    // t_contacts								_contacts;

    MessageBuffer	_message;

public:
    WorkerProducer(const t_def& def);

private:
    static void	onMessageCallback(char* pData, int size, void* arg);

private:
    void	processMessage(const char* pData, int dataLength);

private:
    void	send();

public:
    // void	loadWorker();
    void	resetAndProcessSimulation(const NeuralNetwork* pNeuralNetworks);
    void	processSimulation();

public:
    bool				isLoaded() const;
    bool				isProcessing() const;
    bool				isUpdated() const;
    const t_carsData&	getCarsData() const;

    const AbstactSimulation::t_coreState& getCoreState() const;

    // const t_contacts&	getContactsData() const;
    // void				clearContactsData();

};
