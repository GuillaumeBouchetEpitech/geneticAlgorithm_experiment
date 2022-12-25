
#include "FitnessDataRenderer.hpp"

#include "demo/logic/Context.hpp"

#include "framework/system/math/clamp.hpp"
#include "framework/system/math/easingFunctions.hpp"

#include <iomanip>

namespace {

constexpr float k_faceInX = +8.0f;
constexpr float k_faceOutX = -400.0f;

} // namespace

FitnessDataRenderer::FitnessDataRenderer() {
  _position.x = k_faceOutX;
  _position.y = 160;

  _size = {150, 75};
}

void FitnessDataRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = GenericEasing<2>()
                  .push(0.0f, _position.x, easing::easeOutCubic)
                  .push(1.0f, k_faceInX);
  _alphaEasing = GenericEasing<2>()
                   .push(0.0f, _alpha, easing::easeOutCubic)
                   .push(1.0f, 1.0f);

  _isVisible = true;
}

void FitnessDataRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = GenericEasing<2>()
                  .push(0.0f, _position.x, easing::easeInCubic)
                  .push(1.0f, k_faceOutX);
  _alphaEasing =
    GenericEasing<2>().push(0.0f, _alpha, easing::easeInCubic).push(1.0f, 0.0f);

  _isVisible = false;
}

void FitnessDataRenderer::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);
    _position.x = _moveEasing.get(_timer.getCoefElapsed());
    _alpha = _alphaEasing.get(_timer.getCoefElapsed());
  }
}

void FitnessDataRenderer::resize() {
  if (_isVisible)
    fadeIn(0.0f, 0.2f);
  else
    fadeOut(0.0f, 0.2f);
}

void FitnessDataRenderer::renderWireframe() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;

  auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

  stackRenderers.triangles.pushQuad(glm::vec2(_position + _size * 0.5f), _size,
                                    glm::vec4(0, 0, 0, 0.75f), -0.1f);
  stackRenderers.wireframes.pushRectangle(_position, _size, whiteColor, -0.1f);

  const float maxFitness = logic.fitnessStats.max();
  float stepWidth = _size.x / (logic.fitnessStats.size() - 1);

  for (std::size_t ii = 1; ii < logic.fitnessStats.size(); ++ii) {
    const float prevData = logic.fitnessStats.get(ii - 1);
    const float currData = logic.fitnessStats.get(ii);

    const glm::vec2 prevPos = {
      stepWidth * (ii - 1),
      (prevData / maxFitness) * _size.y,
    };
    const glm::vec2 currPos = {
      stepWidth * ii,
      (currData / maxFitness) * _size.y,
    };

    stackRenderers.wireframes.pushLine(glm::vec3(_position + prevPos, 0.0f),
                                       glm::vec3(_position + currPos, 0.0f),
                                       whiteColor);
  }
}

void FitnessDataRenderer::renderHudText() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;
  auto& simulation = *logic.simulation;
  auto& textRenderer = graphic.hud.textRenderer;
  auto& stackRenderers = graphic.hud.stackRenderers;

  const unsigned int totalCars = logic.cores.totalCars;
  unsigned int carsLeft = 0;
  float localBestFitness = 0.0f;

  for (unsigned int ii = 0; ii < totalCars; ++ii) {
    const auto& carData = simulation.getCarResult(ii);

    if (carData.isAlive)
      ++carsLeft;

    if (localBestFitness < carData.fitness)
      localBestFitness = carData.fitness;
  }

  const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _alpha);
  const glm::vec4 colorRed = glm::vec4(0.5f, 0.0f, 0.0f, _alpha);
  const glm::vec4 colorGreen = glm::vec4(0.0f, 0.5f, 0.0f, _alpha);
  const glm::vec4 bgColor = glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f);
  const float scale = 1.0f;
  const float depth = 0.25f;
  const float bgDepth = depth - 0.1f;
  std::vector<TextRenderer::Rectangle> outRectangles;

  {
    const glm::vec2 textPos = {8, 8 + 2 * 16};

    std::stringstream sstr;
    sstr << "Generation: " << simulation.getGenerationNumber();
    const std::string str = sstr.str();

    textRenderer.push(textPos, str, color, scale, depth);

    textRenderer.getSizes(outRectangles, textPos, str, scale);
    for (const auto& rec : outRectangles)
      stackRenderers.triangles.pushQuad(rec.pos + rec.size * 0.5f, rec.size, bgColor, bgDepth);
  }

  {
    const glm::vec2 textPos = {8, 8 + 1 * 16};

    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(1);
    sstr << "Fitness: " << localBestFitness << "/"
         << simulation.getBestGenome().fitness;
    const std::string str = sstr.str();

    const float bestFitness = simulation.getBestGenome().fitness;
    const float coef = bestFitness == 0.0f
                         ? 0.0f
                         : clamp(localBestFitness / bestFitness, 0.0f, 1.0f);

    const glm::vec4 textColor = glm::mix(colorRed, colorGreen, coef);

    textRenderer.push(textPos, str, color, scale, depth, textColor);

    textRenderer.getSizes(outRectangles, textPos, str, scale);
    for (const auto& rec : outRectangles)
      stackRenderers.triangles.pushQuad(rec.pos + rec.size * 0.5f, rec.size, bgColor, bgDepth);
  }

  {
    const glm::vec2 textPos = {8, 8 + 0 * 16};

    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(1);
    sstr << "Cars: " << carsLeft << "/" << totalCars;
    const std::string str = sstr.str();

    const float coef = 1.0f - clamp(float(carsLeft) / totalCars, 0.0f, 1.0f);

    const glm::vec4 textColor = glm::mix(colorGreen, colorRed, coef);

    textRenderer.push(textPos, str, color, scale, depth, textColor);

    textRenderer.getSizes(outRectangles, textPos, str, scale);
    for (const auto& rec : outRectangles)
      stackRenderers.triangles.pushQuad(rec.pos + rec.size * 0.5f, rec.size, bgColor, bgDepth);
  }
}
