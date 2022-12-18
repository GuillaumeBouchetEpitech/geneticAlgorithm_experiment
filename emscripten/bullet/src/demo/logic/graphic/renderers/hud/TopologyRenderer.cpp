
#include "TopologyRenderer.hpp"

#include "demo/logic/Context.hpp"

#include "framework/math/DeterministicRng.hpp"

void TopologyRenderer::update(float elapsedTime) {
  _animationTime += elapsedTime * 2.0f;
  while (_animationTime > 1.0f)
    _animationTime -= 1.0f;
}

void TopologyRenderer::render(const glm::vec2& position,
                              const glm::vec2& size) {
  auto& context = Context::get();
  auto& logic = context.logic;
  auto& graphic = context.graphic;

  if (!logic.leaderCar.hasLeader())
    return;

  if (logic.leaderCar.totalTimeAsLeader() < 0.25f)
    return;

  const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
  const glm::vec4 redColor(1.0f, 0.0f, 0.0f, 0.85f);
  const glm::vec4 blueColor(0.5f, 0.5f, 1.0f, 0.85f);

  auto& stackRenderer = graphic.hud.stackRenderers.triangles;
  stackRenderer.pushQuad(glm::vec3(position + size * 0.5f, -0.1f), size,
                         glm::vec4(0, 0, 0, 0.75f));
  graphic.hud.stackRenderers.wireframes.pushRectangle(position, size,
                                                      whiteColor);

  std::vector<unsigned int> topologyArray;
  topologyArray.reserve(logic.annTopology.getInput() +
                        logic.annTopology.getHiddens().size() +
                        logic.annTopology.getOutput());
  topologyArray.push_back(logic.annTopology.getInput());
  for (const auto& hidden : logic.annTopology.getHiddens())
    topologyArray.push_back(hidden);
  topologyArray.push_back(logic.annTopology.getOutput());

  const NeuralNetworks& neuralNetworks =
    logic.simulation->getGeneticAlgorithm().getNeuralNetworks();

  std::vector<float> connectionsWeights;
  const auto leaderNnPtr = neuralNetworks.at(logic.leaderCar.leaderIndex());
  leaderNnPtr->getWeights(connectionsWeights);

  struct NeuronData {
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
    for (std::size_t ii = 0; ii < topologyArray.size(); ++ii) {
      const unsigned int actualSize = topologyArray.at(ii);

      glm::vec2 currPos = position;
      currPos.y += size.y - size.y / topologyArray.size() * (float(ii) + 0.5f);

      layersData.at(ii).reserve(actualSize); // pre-allocate
      for (unsigned int jj = 0; jj < actualSize; ++jj) {
        currPos.x += size.x / (actualSize + 1);

        const float neuronsValue =
          glm::clamp(neuronsValues.at(neuronIndex++), 0.0f, 1.0f);

        layersData.at(ii).push_back({currPos, neuronsValue});
      }
    }

  } // setup layersData container

  { // show input/output values

    constexpr float valueHeight = 12.0f;
    ;

    const auto& inputlayer = layersData.front();
    for (auto neuron : inputlayer) {
      neuron.position.y += 3.0f;

      const glm::vec2 start(neuron.position.x, neuron.position.y + valueHeight);
      constexpr float thickness = 10.0f;
      const glm::vec4 color =
        glm::mix(redColor, glm::vec4(0, 1, 0, 1), neuron.value);

      stackRenderer.pushThickTriangle2dLine(start, neuron.position, thickness,
                                            1.0f, color, color, +0.2f);
    }

    const auto& outputlayer = layersData.back();
    for (auto neuron : outputlayer) {
      neuron.position.y -= 3.0f;

      glm::vec2 start =
        glm::vec2(neuron.position.x, neuron.position.y - valueHeight);

      if (neuron.value > 0.0f) {
        const float thickness = 2.0f + neuron.value * +10.0f;
        stackRenderer.pushThickTriangle2dLine(start, neuron.position, thickness,
                                              12.0f, redColor, redColor, +0.2f);
      } else {
        const float thickness = 2.0f + neuron.value * -10.0f;
        stackRenderer.pushThickTriangle2dLine(start, neuron.position, thickness,
                                              12.0f, blueColor, blueColor,
                                              +0.2f);
      }
    }

  } // show input/output values

  { // draw neurons

    const glm::vec4 whiteColor(1, 1, 1, 1);

    for (const auto& layer : layersData) {
      for (const auto& neuron : layer) {
        const float coef = glm::clamp(neuron.value, 0.0f, 1.0f);
        const float radius = 2.0f + coef * 2.0f;

        stackRenderer.pushCircle(glm::vec3(neuron.position, +0.2f), radius,
                                 whiteColor);
      }
    }

  } // draw neurons

  { // draw connections

    struct ValueRange {
      float min;
      float max;
      float alpha;
      float depth;
    };

    std::array<ValueRange, 4> valueRanges{{
      {0.75f, 1.00f, 1.00f, +0.4f},
      {0.50f, 0.75f, 0.85f, +0.3f},
      {0.25f, 0.50f, 0.70f, +0.2f},
      {0.00f, 0.25f, 0.55f, +0.1f},
    }};

    DeterministicRng rng;
    rng.setSeed(logic.leaderCar.leaderIndex());

    std::vector<float> connectionValues;
    connectionValues.reserve(512);
    for (std::size_t ii = 1; ii < layersData.size(); ++ii) {
      const auto& prevLayer = layersData.at(ii - 1);
      const auto& currLayer = layersData.at(ii);
      for (std::size_t plIndex = 0; plIndex < prevLayer.size(); ++plIndex)
        for (std::size_t clIndex = 0; clIndex < currLayer.size(); ++clIndex)
          connectionValues.push_back(rng.getRangedValue(0.0f, 1.0f));
    }

    for (ValueRange range : valueRanges) {
      const glm::vec4 positiveColor =
        glm::vec4(glm::vec3(redColor), range.alpha);
      const glm::vec4 negativeColor =
        glm::vec4(glm::vec3(blueColor), range.alpha);
      // const glm::vec4 impulseColor = glm::vec4(whiteColor, range.alpha);

      int connectionIndex = 0;
      for (std::size_t ii = 1; ii < layersData.size(); ++ii) {
        const auto& prevLayer = layersData.at(ii - 1);
        const auto& currLayer = layersData.at(ii);

        for (const auto& prevNeuron : prevLayer) {
          for (const auto& currNeuron : currLayer) {
            const int currConnectionIndex = connectionIndex++;
            const float weight = connectionsWeights.at(currConnectionIndex);

            if (!(prevNeuron.value >= range.min &&
                  prevNeuron.value <= range.max))
              continue;

            constexpr float initialThickness = 1.0f;
            constexpr float extraThickness = 2.5f;

            const float targetThickness =
              initialThickness +
              weight * (weight > 0.0f ? +extraThickness : -extraThickness);

            const glm::vec4 targetColor(weight > 0.0f ? positiveColor
                                                      : negativeColor);

            stackRenderer.pushThickTriangle2dLine(
              prevNeuron.position, currNeuron.position, targetThickness,
              targetColor, range.depth);

            float tmpCoef =
              _animationTime + connectionValues.at(currConnectionIndex);
            while (tmpCoef > 1.0f)
              tmpCoef -= 1.0f;

            const glm::vec2 diff = currNeuron.position - prevNeuron.position;

            stackRenderer.pushThickTriangle2dLine(
              prevNeuron.position + diff * (tmpCoef - 0.05f),
              prevNeuron.position + diff * (tmpCoef + 0.05f),
              targetThickness * 1.5f, targetColor, range.depth + 0.01f);
          }
        }
      }
    }

  } // draw connections
}
