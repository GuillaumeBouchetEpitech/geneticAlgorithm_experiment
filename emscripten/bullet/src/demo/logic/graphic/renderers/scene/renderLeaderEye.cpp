
#include "renderLeaderEye.hpp"

#include "demo/logic/Context.hpp"

void renderLeaderEye(const glm::vec2& position, const glm::vec2& size) {
  auto& context = Context::get();
  auto& logic = context.logic;
  auto& stackRenderers = context.graphic.hud.stackRenderers;

  if (auto leaderData = logic.leaderCar.leaderData()) {
    const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);
    const glm::vec3 greenColor(0.0f, 1.0f, 0.0f);
    const glm::vec3 redColor(1.0f, 0.0f, 0.0f);

    stackRenderers.wireframes.pushRectangle(position, size, whiteColor);

    //
    //

    const unsigned int totalInputs = context.logic.annTopology.getInput();
    const unsigned int layerSize = 5; // <= hardcoded :(
    const unsigned int layerCount = totalInputs / layerSize;

    //
    //

    std::vector<glm::vec2> allPositions;
    allPositions.reserve(totalInputs); // pre-allocate
    for (unsigned int ii = 0; ii < layerCount; ++ii)
      for (unsigned int jj = 0; jj < layerSize; ++jj) {
        glm::vec2 currPos = position;

        currPos.y += size.y;

        currPos.x += size.x * ((float(jj) + 0.5f) / layerSize);
        currPos.y -= size.y * ((float(ii) + 0.5f) / layerCount);

        allPositions.push_back(currPos);
      }

    glm::vec2 eyeSize = {19, 19};

    for (std::size_t ii = 0; ii < allPositions.size(); ++ii) {
      const auto& position = allPositions.at(ii);

      stackRenderers.wireframes.pushRectangle(position - eyeSize * 0.5f,
                                              eyeSize, whiteColor);

      glm::vec3 color =
        glm::mix(redColor, greenColor, leaderData->eyeSensors.at(ii).value);

      stackRenderers.triangles.pushQuad(position, eyeSize,
                                        glm::vec4(color, 1.0f));
    }
  }
}