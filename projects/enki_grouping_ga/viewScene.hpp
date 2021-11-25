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
    int v_robot_left;
    int v_robot_right;
    int v_nothing_left;
    int v_nothing_right;

  protected:
    QVector<EPuckController*> epucks;

  public:
    EpuckGroupingGA(
      World *world,
      int batchRobotsSize,
      int v_robot_left,
      int v_robot_right,
      int v_nothing_left,
      int v_nothing_right,
      QWidget *parent = 0
    ):
      ViewerWidget(world, parent),
      totalRobots(0),
      v_robot_left(v_robot_left),
      v_robot_right(v_robot_right),
      v_nothing_left(v_nothing_left),
      v_nothing_right(v_nothing_right),
      batchRobotsSize(batchRobotsSize)
    {
      initiazeEpucks(Color::green, batchRobotsSize);
    }

    void initiazeEpucks(Color color, int n) {
      {
        for (int i = 0; i < n; ++i) {
          EPuckController *epuck = new EPuckController(v_robot_left, v_robot_right, v_nothing_left, v_nothing_right, EPuckController::CAPABILITY_CAMERA);
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
