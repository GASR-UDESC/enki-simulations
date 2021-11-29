#ifndef EPUCK_CONTROLLER_H
#define EPUCK_CONTROLLER_H

#include <enki/robots/e-puck/EPuck.h>

namespace Enki {
	class EPuckController : public EPuck {
  private:
    bool seeingAnotherRobot() {
      {
        return camera.image[29] == getColor() || camera.image[30] == getColor();
      }
    }

	public:
    float v_robot_left;
    float v_robot_right;
    float v_nothing_left;
    float v_nothing_right;

		EPuckController(
      float v_robot_left,
      float v_robot_right,
      float v_nothing_left,
      float v_nothing_right,
      unsigned capabilities = CAPABILITY_BASIC_SENSORS
    ):
      EPuck(capabilities),
      v_robot_left(v_robot_left),
      v_robot_right(v_robot_right),
      v_nothing_left(v_nothing_left),
      v_nothing_right(v_nothing_right)
    {
      leftSpeed = v_nothing_left;
      rightSpeed = v_nothing_right;
    }

    ~EPuckController() {}

    void move() {
      if (seeingAnotherRobot()) {
        leftSpeed = v_robot_left;
        rightSpeed = v_robot_right;
      } else {
        leftSpeed = v_nothing_left;
        rightSpeed = v_nothing_right;
      }
    }
	};
}

#endif
