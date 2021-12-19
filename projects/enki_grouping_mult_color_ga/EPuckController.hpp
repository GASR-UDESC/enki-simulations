#ifndef EPUCK_CONTROLLER_H
#define EPUCK_CONTROLLER_H

#include <enki/robots/e-puck/EPuck.h>

namespace Enki {
	class EPuckController : public EPuck {
  private:
    bool seeingSimilarRobot() {
      {
        return camera.image[29] == getColor() || camera.image[30] == getColor();
      }
    }

    bool seeingDiferentRobot() {
      {
        return camera.image[29] != getColor() && camera.image[30] != getColor() && camera.image[29] != Color(0,0,0,0) && camera.image[30] != Color(0,0,0,0);
      }
    }

	public:
    float v_similar_robot_left;
    float v_similar_robot_right;
    float v_diferent_robot_left;
    float v_diferent_robot_right;
    float v_nothing_left;
    float v_nothing_right;

		EPuckController(
      float v_similar_robot_left,
      float v_similar_robot_right,
      float v_diferent_robot_left,
      float v_diferent_robot_right,
      float v_nothing_left,
      float v_nothing_right,
      unsigned capabilities = CAPABILITY_BASIC_SENSORS
    ):
      EPuck(capabilities),
      v_similar_robot_left(v_similar_robot_left),
      v_similar_robot_right(v_similar_robot_right),
      v_diferent_robot_left(v_diferent_robot_left),
      v_diferent_robot_right(v_diferent_robot_right),
      v_nothing_left(v_nothing_left),
      v_nothing_right(v_nothing_right)
    {
      leftSpeed = v_nothing_left;
      rightSpeed = v_nothing_right;
    }

    ~EPuckController() {}

    void move() {
      if (seeingSimilarRobot()) {
        leftSpeed = v_similar_robot_left;
        rightSpeed = v_similar_robot_right;
      } else if (seeingDiferentRobot()) {
        leftSpeed = v_diferent_robot_left;
        rightSpeed = v_diferent_robot_right;
      } else {
        leftSpeed = v_nothing_left;
        rightSpeed = v_nothing_right;
      }
    }
	};
}

#endif
