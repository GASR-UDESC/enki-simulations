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
    float v_robot_left;
    float v_robot_right;
    float v_nothing_left;
    float v_nothing_right;
    int world_width;
    int world_height;

  protected:
    QVector<EPuckController*> epucks;

  public:
    EpuckGroupingGA(
      World *world,
      int batchRobotsSize,
      float v_robot_left,
      float v_robot_right,
      float v_nothing_left,
      float v_nothing_right,
      int world_width,
      int world_height,
      QWidget *parent = 0
    ):
      ViewerWidget(world, parent),
      totalRobots(0),
      v_robot_left(v_robot_left),
      v_robot_right(v_robot_right),
      v_nothing_left(v_nothing_left),
      v_nothing_right(v_nothing_right),
      world_width(world_width),
      world_height(world_height),
      batchRobotsSize(batchRobotsSize)
    {
      initiazeEpucks(Color::green, batchRobotsSize);
    }

    void initiazeEpucks(Color color, int n) {
      {
        for (int i = 0; i < n; ++i) {
          EPuckController *epuck = new EPuckController(v_robot_left, v_robot_right, v_nothing_left, v_nothing_right, EPuckController::CAPABILITY_CAMERA);
          epuck->pos = Point(UniformRand(0, world_width)(), UniformRand(0, world_height)());
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
