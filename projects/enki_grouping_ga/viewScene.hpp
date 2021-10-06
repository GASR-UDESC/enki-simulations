#ifndef VIEW_SCENE_H
#define VIEW_SCENE_H

#include <viewer/Viewer.h>
#include <viewer/EPuckModel.h>
#include <enki/PhysicalEngine.h>
#include "EPuckController.hpp"

namespace Enki {
  class EpuckGroupingGA : public ViewerWidget {
  private:
    int batchRobotsSize;
    int totalRobots;
    int lowVelocity;
    int highVelocity;

  protected:
    QVector<EPuckController*> epucks;

  public:
    EpuckGroupingGA(World *world, int batchRobotsSize, int lowVelocity, int highVelocity, QWidget *parent = 0):
      ViewerWidget(world, parent),
      totalRobots(0),
      lowVelocity(lowVelocity),
      highVelocity(highVelocity),
      batchRobotsSize(batchRobotsSize)
    {
      initiazeEpucks(Color::green, batchRobotsSize);
      initiazeEpucks(Color::red, batchRobotsSize);
      initiazeEpucks(Color::blue, batchRobotsSize);
      initiazeEpucks(Color::yellow, batchRobotsSize);
      initiazeEpucks(Color::pink, batchRobotsSize);
    }

    void initiazeEpucks(Color color, int n) {
      {
        for (int i = 0; i < n; ++i) {
          EPuckController *epuck = new EPuckController(lowVelocity, highVelocity, EPuckController::CAPABILITY_CAMERA);
          epuck->pos = Point(UniformRand(0, 500)(), UniformRand(0, 500)());
          epuck->setColor(color);

          epucks.push_back(epuck);
          world->addObject(epuck);
        }

        totalRobots += n;
      }
    }

    ~EpuckGroupingGA() {}

    virtual void timerEvent(QTimerEvent * event) {
      for (int i = 0; i < totalRobots; i++) {
        epucks[i]->move();
      }

      ViewerWidget::timerEvent(event);
    }

    virtual void sceneCompletedHook() {}

    void initializeGL() {
      // add texture to EPuck extended class
      managedObjects[&typeid(EPuckController)] = new EPuckModel(this);

      ViewerWidget::initializeGL();
    }
  };
}

#endif
