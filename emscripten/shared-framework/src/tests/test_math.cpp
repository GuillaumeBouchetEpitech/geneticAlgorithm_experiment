

#include "framework/math/circleCircleIntersectionPoints.hpp"

#include "framework/TraceLogger.hpp"

void test_math()
{

  D_MYLOG("#");
  D_MYLOG("##");
  D_MYLOG("###");



  Circle c1;
  c1.center.x = 0;
  c1.center.y = 0;
  c1.radius = 4;

  Circle c2;

  // c2.center.x = 8;
  // c2.center.y = 0;

  // c2.center.x = -8;
  // c2.center.y = 0;

  // c2.center.x = 4;
  // c2.center.y = 4;

  c2.center.x = -4;
  c2.center.y = -4;

  c2.radius = 4;

  glm::vec2 res1;
  glm::vec2 res2;

  D_MYLOG("c1 " << c1.center.x << " / " << c1.center.y << ", " << c1.radius);
  D_MYLOG("c2 " << c2.center.x << " / " << c2.center.y << ", " << c2.radius);

  bool result = circleCircleIntersectionPoints(c1, c2, res1, res2);

  assert(result == true);

  auto callback = [&c1, &c2](const glm::vec2& currRes)
  {
    D_MYLOG("currRes " << currRes.x << " / " << currRes.y);

    const glm::vec2 parent = currRes - c1.center;
    const float parent_angle = std::atan2(parent.y, parent.x);

    const glm::vec2 child = c2.center - currRes;
    // const float child_angle = std::atan2(child.y, child.x);
    const float child_angle = std::atan2(child.y, child.x) - parent_angle;

    D_MYLOG("parent " << parent.x << " / " << parent.y);
    D_MYLOG("parent_angle " << parent_angle << "   (" << glm::degrees(parent_angle) << ")");

    D_MYLOG("child " << child.x << " / " << child.y);
    D_MYLOG("child_angle  " << child_angle << "   (" << glm::degrees(child_angle) << ")");
  };

  D_MYLOG("====================");
  D_MYLOG("res1");
  callback(res1);

  D_MYLOG("====================");
  D_MYLOG("res2");
  callback(res2);



  D_MYLOG("###");
  D_MYLOG("##");
  D_MYLOG("#");


}

