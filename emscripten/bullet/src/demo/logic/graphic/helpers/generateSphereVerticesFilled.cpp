
#include "generateSphereVerticesFilled.hpp"

#include <array>

namespace
{

  void drawSpherePatch(
    std::vector<glm::vec3>& vertices,
    uint32_t quality,
    float radius,
    const glm::vec3& v01,
    const glm::vec3& v02,
    const glm::vec3& v03)
  {
    if (quality <= 0)
    {
      vertices.push_back({ v01 * radius });
      vertices.push_back({ v03 * radius });
      vertices.push_back({ v02 * radius });
    }
    else
    {
      const glm::vec3 v12 = glm::normalize(v01 + (v02 - v01) * 0.5f);
      const glm::vec3 v23 = glm::normalize(v02 + (v03 - v02) * 0.5f);
      const glm::vec3 v31 = glm::normalize(v03 + (v01 - v03) * 0.5f);

      quality -= 1;

      drawSpherePatch(vertices, quality, radius, v01, v12, v31);
      drawSpherePatch(vertices, quality, radius, v12, v02, v23);
      drawSpherePatch(vertices, quality, radius, v31, v23, v03);
      drawSpherePatch(vertices, quality, radius, v12, v23, v31);
    }
  }

}

void generateSphereVerticesFilled(
  float radius,
  unsigned int quality,
  std::vector<glm::vec3>& vertices)
{
  constexpr float	k_icx =	0.525731112119133606f;
  constexpr float	k_icz	= 0.850650808352039932f;

  std::array<glm::vec3, 12> positions =
  {{
    { -k_icx, 0.0f, +k_icz },
    { +k_icx, 0.0f, +k_icz },
    { -k_icx, 0.0f, -k_icz },
    { +k_icx, 0.0f, -k_icz },
    { 0.0f, +k_icz, +k_icx },
    { 0.0f, +k_icz, -k_icx },
    { 0.0f, -k_icz, +k_icx },
    { 0.0f, -k_icz, -k_icx },
    { +k_icz, +k_icx, 0.0f },
    { -k_icz, +k_icx, 0.0f },
    { +k_icz, -k_icx, 0.0f },
    { -k_icz, -k_icx, 0.0f },
  }};

  std::array<glm::ivec3, 20> indices =
  {{
    {  0,  4,  1 },
    {  0,  9,  4 },
    {  9,  5,  4 },
    {  4,  5,  8 },
    {  4,  8,  1 },
    {  8, 10,  1 },
    {  8,  3, 10 },
    {  5,  3,  8 },
    {  5,  2,  3 },
    {  2,  7,  3 },
    {  7, 10,  3 },
    {  7,  6, 10 },
    {  7, 11,  6 },
    { 11,  0,  6 },
    {  0,  1,  6 },
    {  6,  1, 10 },
    {  9,  0, 11 },
    {  9, 11,  2 },
    {  9,  2,  5 },
    {  7,  2, 11 },
  }};

  vertices.clear();
  vertices.reserve(indices.size() * 3); // pre-allocate
  for (const glm::ivec3& index : indices)
  {
    drawSpherePatch(
      vertices,
      quality,
      radius,
      positions.at(index.x),
      positions.at(index.y),
      positions.at(index.z));
  }
}
