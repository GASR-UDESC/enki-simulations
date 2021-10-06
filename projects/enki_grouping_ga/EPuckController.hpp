#ifndef EPUCK_CONTROLLER_H
#define EPUCK_CONTROLLER_H

#include <enki/robots/e-puck/EPuck.h>

namespace Enki {
	class EPuckController : public EPuck {
  private:
    bool seeingAnotherRobot() {
      {
        for (int i=0; i < camera.image.size(); i++) {
          if (camera.image[i] == getColor()) {
            return true;
          }
        }

        return false;
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
