#include <viewer/Viewer.h>
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>
#include <QApplication>
#include <QtGui>
#include <iostream>

using namespace Enki;
using namespace std;

int force = 5;
int low_velocity = 2 * force;
int higth_velocity = 5 * force;
int diff_robost = 1;
int n_robots = 200;
int n_partial_robots = n_robots / diff_robost;

class EpuckTest : public ViewerWidget {
protected:
	QVector<EPuck*> epucks;

public:
	EpuckTest(World *world, QWidget *parent = 0) : ViewerWidget(world, parent) {
		initiaze_epucks(Color::green, n_partial_robots);
	}

	void initiaze_epucks(Color color, int n) {
		for (int i = 0; i < n; ++i) {
			EPuck *epuck = new EPuck(EPuck::CAPABILITY_CAMERA);
			epuck->pos = Point(UniformRand(0, 500)(), UniformRand(0, 500)());
			epuck->leftSpeed = -low_velocity;
			epuck->rightSpeed = -higth_velocity;
			epuck->setColor(color);

			epucks.push_back(epuck);
			world->addObject(epuck);
		}
	}

	~EpuckTest() {}

	virtual void timerEvent(QTimerEvent * event) {
		static int fireCounter = 0;

		bool see_robot = false;

		for (int i = 0; i < n_robots; ++i) {
			see_robot = false;

			for (int j=0; !see_robot && j < epucks[i]->camera.image.size(); j++) {
				if (epucks[i]->camera.image[j] == Color::green || epucks[i]->camera.image[j] == Color::red) {
					see_robot = true;
				}
			}

			if (see_robot) {
				epucks[i]->setColor(Color::red);
				epucks[i]->leftSpeed = low_velocity;
				epucks[i]->rightSpeed = -low_velocity;
			} else {
				epucks[i]->setColor(Color::green);
				epucks[i]->leftSpeed = -low_velocity;
				epucks[i]->rightSpeed = -higth_velocity;
			}
		}

		ViewerWidget::timerEvent(event);
	}

	virtual void sceneCompletedHook() {}
};

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	World world(500, 500, Color(30.0/255, 50.0/255, 70.0/255),  World::GroundTexture());
	EpuckTest viewer(&world);

	viewer.show();

	return app.exec();
}

