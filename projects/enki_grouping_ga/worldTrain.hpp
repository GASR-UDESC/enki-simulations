#include "EPuckController.hpp"

using namespace Enki;
using namespace std;

class NoViewerMode {
	private:
		int batchRobotsSize;
    int totalRobots;
    int world_width;
    int world_height;
    int number;
		World world;

	protected:
    QVector<EPuckController*> epucks;

	public:
	NoViewerMode(int batchRobotsSize, int lowVelocity, int highVelocity, int world_width, int world_height):
		totalRobots(0),
		world_width(world_width),
		world_height(world_height),
		batchRobotsSize(batchRobotsSize),
		world(world_width, world_height)
	{
		initiazeEpucks(Color(0,0,1), batchRobotsSize, lowVelocity, highVelocity);
		initiazeEpucks(Color(1,0,0), batchRobotsSize, lowVelocity, highVelocity);
		initiazeEpucks(Color(0,0,1), batchRobotsSize, lowVelocity, highVelocity);
		initiazeEpucks(Color(1,1,0), batchRobotsSize, lowVelocity, highVelocity);
		initiazeEpucks(Color(1,0,0.8), batchRobotsSize, lowVelocity, highVelocity);

		// create_img("results/enki_grouping_ga/initial.ppm");
	}

	void initiazeEpucks(Color color, int n, int lowVelocity, int highVelocity) {
    cout << color << endl;
    for (int i = 0; i < n; ++i) {
      EPuckController *epuck = new EPuckController(lowVelocity, highVelocity, EPuckController::CAPABILITY_CAMERA);
      epuck->pos = Point(UniformRand(0, 500)(), UniformRand(0, 500)());
      epuck->setColor(color);

      epucks.push_back(epuck);
      world.addObject(epuck);
    }

    totalRobots += n;
	}

	void run() {
		for (unsigned i=0; i<5000; i++) {
			for (int j = 0; j < totalRobots; j++) {
        epucks[j]->move();
      }

			// values from viewer/Viewer.cpp
			world.step(30.0/1000.0, 3);
		}

    string name = "results/enki_grouping_ga/final_" + std::to_string(number) + ".ppm";
    int n = name.length();
    char *char_array;
    char_array = (char *) malloc(sizeof(char) * (n + 2));
    strcpy(char_array, name.c_str());
		create_img(char_array);
	}

  int fitness() {
    int n_robots_alt_of_range = 0;

    for (int i = 0; i < totalRobots; i++) {
      Color closer_color = Color::gray;
      double distance = 500;

      for (int j = 0; j < totalRobots; j++) {
        if (i!=j && (abs(epucks[i]->pos.x - epucks[j]->pos.x) + abs(epucks[i]->pos.y - epucks[j]->pos.y)) < distance) {
          distance = abs(epucks[i]->pos.x - epucks[j]->pos.x) + abs(epucks[i]->pos.y - epucks[j]->pos.y);
          closer_color = epucks[j]->getColor();
        }
      }

      if (epucks[i]->getColor() != closer_color) {
        n_robots_alt_of_range++;
      }
    }

    return n_robots_alt_of_range;
  }

  void reset(int lowVelocity, int highVelocity, int num) {
    number = num;

    for (int i = 0; i < totalRobots; i++) {
      epucks[i]->pos = Point(UniformRand(0, world_width)(), UniformRand(0, world_height)());
      epucks[i]->lowVelocity = lowVelocity;
      epucks[i]->highVelocity = highVelocity;
    }

    string name = "results/enki_grouping_ga/initial_" + std::to_string(number) + "_" + std::to_string(lowVelocity) + "_" + std::to_string(highVelocity) + ".ppm";
    int n = name.length();
    char *char_array;
    char_array = (char *) malloc(sizeof(char) * (n + 2));
    strcpy(char_array, name.c_str());
		create_img(char_array);
  }

	void create_img(char *fileName) {
		int matrix[world_width][world_height];
		for (int i = 0; i < world_width; i++) {
			for (int j = 0; j < world_width; j++) {
				matrix[i][j] = 0;
			}
		}

		for (int i = 0; i < totalRobots; i++) {
			if (epucks[i]->getColor() == Color(0,0,1)) {
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y)] = 1;
				matrix[int(epucks[i]->pos.x)+1][int(epucks[i]->pos.y)] = 1;
				matrix[int(epucks[i]->pos.x)-1][int(epucks[i]->pos.y)] = 1;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y+1)] = 1;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y-1)] = 1;
			} else if (epucks[i]->getColor() == Color(1,0,0)) {
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y)] = 2;
				matrix[int(epucks[i]->pos.x)+1][int(epucks[i]->pos.y)] = 2;
				matrix[int(epucks[i]->pos.x)-1][int(epucks[i]->pos.y)] = 2;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y+1)] = 2;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y-1)] = 2;
			} else if (epucks[i]->getColor() == Color(0,0,1)) {
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y)] = 3;
				matrix[int(epucks[i]->pos.x)+1][int(epucks[i]->pos.y)] = 3;
				matrix[int(epucks[i]->pos.x)-1][int(epucks[i]->pos.y)] = 3;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y+1)] = 3;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y-1)] = 3;
			} else if (epucks[i]->getColor() == Color(1,1,0)) {
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y)] = 4;
				matrix[int(epucks[i]->pos.x)+1][int(epucks[i]->pos.y)] = 4;
				matrix[int(epucks[i]->pos.x)-1][int(epucks[i]->pos.y)] = 4;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y+1)] = 4;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y-1)] = 4;
			} else if (epucks[i]->getColor() == Color(1,0,0.8)) {
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y)] = 5;
				matrix[int(epucks[i]->pos.x)+1][int(epucks[i]->pos.y)] = 5;
				matrix[int(epucks[i]->pos.x)-1][int(epucks[i]->pos.y)] = 5;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y+1)] = 5;
				matrix[int(epucks[i]->pos.x)][int(epucks[i]->pos.y-1)] = 5;
			}
		}

		FILE *imageFile;

		imageFile=fopen(fileName,"wb");
		if(imageFile==NULL) {
				perror("ERROR: Cannot open output file");
				exit(EXIT_FAILURE);
		}

		fprintf(imageFile,"P6\n");
		fprintf(imageFile,"%d %d\n", world_width, world_height);
		fprintf(imageFile,"255\n");

		unsigned char pix[world_width * world_height * 3];

		int index = 0;
		for (int i = 0; i < world_width; i++) {
			for (int j = 0; j < world_width; j++) {
				if (matrix[i][j] == 1) {
					pix[index++] = 0;
					pix[index++] = 150;
					pix[index++] = 0;
				} else if (matrix[i][j] == 2) {
					pix[index++] = 150;
					pix[index++] = 0;
					pix[index++] = 0;
				} else if (matrix[i][j] == 3) {
					pix[index++] = 0;
					pix[index++] = 0;
					pix[index++] = 150;
				} else if (matrix[i][j] == 4) {
					pix[index++] = 150;
					pix[index++] = 150;
					pix[index++] = 0;
				} else if (matrix[i][j] == 5) {
					pix[index++] = 150;
					pix[index++] = 0;
					pix[index++] = 100;
				} else {
					pix[index++] = 50;
					pix[index++] = 50;
					pix[index++] = 50;
				}
			}
		}

		fwrite(pix, 1, world_width * world_height * 3, imageFile);
		fclose(imageFile);
	}
};
