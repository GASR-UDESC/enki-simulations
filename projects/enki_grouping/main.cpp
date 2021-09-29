#include <viewer/Viewer.h>
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>
#include <QApplication>
#include <QtGui>
#include <iostream>

using namespace Enki;
using namespace std;

int force = 5;
int lowVelocity = 2 * force;
int highVelocity = 5 * force;
int totalRobots = 200;

class EpuckTest : public ViewerWidget {
protected:
	QVector<EPuck*> epucks;

public:
	EpuckTest(World *world, QWidget *parent = 0) : ViewerWidget(world, parent) {
		initiazeEpucks(Color::green, totalRobots);
	}

	void initiazeEpucks(Color color, int n) {
		for (int i = 0; i < n; ++i) {
			EPuck *epuck = new EPuck(EPuck::CAPABILITY_CAMERA);
			epuck->pos = Point(UniformRand(0, 500)(), UniformRand(0, 500)());
			epuck->leftSpeed = -lowVelocity;
			epuck->rightSpeed = -highVelocity;
			epuck->setColor(color);

			epucks.push_back(epuck);
			world->addObject(epuck);
		}
	}

	~EpuckTest() {}

	virtual void timerEvent(QTimerEvent * event) {
		static int fireCounter = 0;

		bool seeingAnotherRobot = false;

		for (int i = 0; i < totalRobots; ++i) {
			seeingAnotherRobot = false;

			for (int j=0; !seeingAnotherRobot && j < epucks[i]->camera.image.size(); j++) {
				if (epucks[i]->camera.image[j] == Color::green || epucks[i]->camera.image[j] == Color::red) {
					seeingAnotherRobot = true;
				}
			}

			if (seeingAnotherRobot) {
				epucks[i]->setColor(Color::red);
				epucks[i]->leftSpeed = lowVelocity;
				epucks[i]->rightSpeed = -lowVelocity;
			} else {
				epucks[i]->setColor(Color::green);
				epucks[i]->leftSpeed = -lowVelocity;
				epucks[i]->rightSpeed = -highVelocity;
			}
		}

		ViewerWidget::timerEvent(event);
	}

	virtual void sceneCompletedHook() {}
};

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	World world(500, 500, Color::gray,  World::GroundTexture());
	EpuckTest viewer(&world);

	viewer.show();

	return app.exec();
}

