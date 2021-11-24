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
    int lowVelocity;
    int highVelocity;

		EPuckController(int lowVelocity, int highVelocity, unsigned capabilities = CAPABILITY_BASIC_SENSORS):
      EPuck(capabilities), lowVelocity(lowVelocity), highVelocity(highVelocity)
    {
      leftSpeed = -lowVelocity;
      rightSpeed = -highVelocity;
    }

    ~EPuckController() {}

    void move() {
      if (seeingAnotherRobot()) {
        leftSpeed = lowVelocity;
        rightSpeed = -lowVelocity;
      } else {
        leftSpeed = -lowVelocity;
        rightSpeed = -highVelocity;
      }
    }
	};
}

#endif
