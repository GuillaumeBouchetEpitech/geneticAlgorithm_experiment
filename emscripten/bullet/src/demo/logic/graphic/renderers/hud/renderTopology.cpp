
#include "renderTopology.hpp"

#include "demo/logic/Data.hpp"

void renderTopology(
  const glm::vec2& position,
  const glm::vec2& size)
{
  auto& data = Data::get();
  auto& logic = data.logic;
  auto& graphic = data.graphic;

  const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
  const glm::vec4 redColor(1.0f,0.0f,0.0f,0.85f);
  const glm::vec4 blueColor(0.5f,0.5f,1.0f,0.85f);

  graphic.stackRenderer.pushQuad(glm::vec3(position + size * 0.5f, -0.1f), size, glm::vec4(0,0,0, 0.75f));
  graphic.stackRenderer.pushRectangle(position, size, whiteColor);

  std::vector<unsigned int> topologyArray;
  topologyArray.push_back(logic.annTopology.getInput());
  for (const auto& hidden : logic.annTopology.getHiddens())
    topologyArray.push_back(hidden);
  topologyArray.push_back(logic.annTopology.getOutput());

  const NeuralNetworks& neuralNetworks = logic.simulation->getGeneticAlgorithm().getNeuralNetworks();

  std::vector<float> connectionsWeights;
  const auto leaderNnPtr = neuralNetworks[logic.leaderCar.index];
  leaderNnPtr->getWeights(connectionsWeights);

  struct NeuronData
  {
    glm::vec2 position;
    float value;
  };
  using Layer = std::vector<NeuronData>;

  std::vector<Layer> layersData;

  { // setup layersData container

    layersData.resize(topologyArray.size());

    std::vector<float> neuronsValues;
    leaderNnPtr->getNeuronsValues(neuronsValues);

    int neuronIndex = 0;
    for (std::size_t ii = 0; ii < topologyArray.size(); ++ii)
    {
      const unsigned int actualSize = topologyArray[ii];

      glm::vec2 currPos = position;
      currPos.y += size.y - size.y / topologyArray.size() * (float(ii) + 0.5f);

      layersData[ii].reserve(actualSize); // pre-allocate
      for (unsigned int jj = 0; jj < actualSize; ++jj)
      {
        currPos.x += size.x / (actualSize + 1);

        const float neuronsValue = glm::clamp(neuronsValues[neuronIndex++], 0.0f, 1.0f);

        layersData[ii].push_back({ currPos, neuronsValue });
      }
    }

  } // setup layersData container

  { // show input/output values

    constexpr float valueHeight = 12.0f;;

    const auto& inputlayer = layersData.front();
    for (auto neuron : inputlayer)
    {
      neuron.position.y += 3.0f;

      const glm::vec2 start(neuron.position.x, neuron.position.y + valueHeight);
      constexpr float thickness = 10.0f;
      const glm::vec4 color = glm::mix(redColor, glm::vec4(0,1,0,1), neuron.value);

      graphic.stackRenderer.pushThickTriangleLine(
        start,
        neuron.position,
        thickness,
        1.0f,
        color,
        +0.2f);
    }

    const auto& outputlayer = layersData.back();
    for (auto neuron : outputlayer)
    {
      neuron.position.y -= 3.0f;

      glm::vec2 start = glm::vec2(neuron.position.x, neuron.position.y - valueHeight);

      if (neuron.value > 0.0f)
      {
        const float thickness = 2.0f + neuron.value * +10.0f;
        graphic.stackRenderer.pushThickTriangleLine(
          start,
          neuron.position,
          thickness,
          12.0f,
          redColor
          +0.2f);
      }
      else
      {
        const float thickness = 2.0f + neuron.value * -10.0f;
        graphic.stackRenderer.pushThickTriangleLine(
          start,
          neuron.position,
          thickness,
          12.0f,
          blueColor,
          +0.2f);
      }
    }

  } // show input/output values

  { // draw neurons

    const glm::vec4 whiteColor(1,1,1,1);

    for (const auto& layer : layersData)
    {
      for (const auto& neuron : layer)
      {
        const float coef = glm::clamp(neuron.value, 0.0f, 1.0f);
        const float radius = 2.0f + coef * 2.0f;

        graphic.stackRenderer.pushCircle(glm::vec3(neuron.position, +0.2f), radius, whiteColor);
      }
    }

  } // draw neurons

  { // draw connections

    struct ValueRange
    {
      float min;
      float max;
      float alpha;
    };

    std::array<ValueRange, 4> valueRanges
    {{
      { 0.75f, 1.00f, 1.00f },
      { 0.50f, 0.75f, 0.85f },
      { 0.25f, 0.50f, 0.70f },
      { 0.00f, 0.25f, 0.55f },
    }};

    for (ValueRange range : valueRanges)
    {
      const glm::vec4 positiveColor = glm::vec4(glm::vec3(redColor), range.alpha);
      const glm::vec4 negativeColor = glm::vec4(glm::vec3(blueColor), range.alpha);

      int connectionIndex = 0;
      for (std::size_t ii = 1; ii < layersData.size(); ++ii)
      {
        const auto& prevLayer = layersData[ii - 1];
        const auto& currLayer = layersData[ii];

        for (const auto& prevNeuron : prevLayer)
        {
          for (const auto& currNeuron : currLayer)
          {
            const float weight = connectionsWeights[connectionIndex++];

            if (!(prevNeuron.value >= range.min && prevNeuron.value <= range.max))
              continue;

            constexpr float initialThickness = 1.0f;
            constexpr float extraThickness = 2.5f;

            const float targetThickness = initialThickness + weight * (weight > 0.0f
              ? +extraThickness
              : -extraThickness);

            const glm::vec4 targetColor (weight > 0.0f ? positiveColor : negativeColor);

            graphic.stackRenderer.pushThickTriangleLine(
              prevNeuron.position,
              currNeuron.position,
              targetThickness,
              targetThickness,
              targetColor,
              +0.0f);
          }
        }
      }
    }

  } // draw connections

}

