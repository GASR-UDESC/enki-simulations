#include "EPuckController.hpp"

using namespace Enki;
using namespace std;

class NoViewerMode {
	private:
		int batchRobotsSize;
    int totalRobots;
    int time_solution;
    int world_width;
    int world_height;
    int number;
		World world;

	protected:
    QVector<EPuckController*> epucks;

	public:
	NoViewerMode(int batchRobotsSize, int lowVelocity, int highVelocity, int world_width, int world_height):
		totalRobots(0),
		time_solution(0),
		world_width(world_width),
		world_height(world_height),
		batchRobotsSize(batchRobotsSize),
		world(world_width, world_height)
	{
		initiazeEpucks(Color(0,0,1), batchRobotsSize, lowVelocity, highVelocity);
	}

	void initiazeEpucks(Color color, int n, int lowVelocity, int highVelocity) {
    for (int i = 0; i < n; ++i) {
      EPuckController *epuck = new EPuckController(lowVelocity, highVelocity, EPuckController::CAPABILITY_CAMERA);
      epuck->pos = Point(UniformRand(0, world_width)(), UniformRand(0, world_height)());
      epuck->setColor(color);

      epucks.push_back(epuck);
      world.addObject(epuck);
    }

    totalRobots += n;
	}

	void run() {
		int p_fitness = 10000;
		int p_fitness_aux = 0;
		int count_stable = 0;

		for (time_solution = 0; time_solution<10000 && count_stable < 100; time_solution++) {
			for (int j = 0; j < totalRobots; j++) {
        epucks[j]->move();
      }

			// values from viewer/Viewer.cpp
			world.step(30.0/1000.0, 3);
			p_fitness_aux = fitness_partial();

			if (p_fitness_aux < p_fitness) {
				p_fitness = p_fitness_aux;
				count_stable = 0;
			} else {
				count_stable++;
			}
		}

    string name = "results/enki_grouping_ga/final_" + std::to_string(number) + ".ppm";
    int n = name.length();
    char *char_array;
    char_array = (char *) malloc(sizeof(char) * (n + 2));
    strcpy(char_array, name.c_str());
		create_img(char_array);
	}

  int fitness_partial_2() {
		int d_max = 0;
		int value = 0;

    for (int i = 0; i < totalRobots; i++) {
      for (int j = 0; j < totalRobots; j++) {
				if (i == j) continue;

				value = sqrt(
					(epucks[i]->pos.x - epucks[j]->pos.x) * (epucks[i]->pos.x - epucks[j]->pos.x) +
					(epucks[i]->pos.y - epucks[j]->pos.y) * (epucks[i]->pos.y - epucks[j]->pos.y)
				);

				if (value > d_max) {
					d_max = value;
				}
      }
    }

    return d_max;
  }

	int fitness_partial() {
		int d_max = 0;
		int value = 0;

    for (int i = 0; i < totalRobots; i++) {
      for (int j = 0; j < totalRobots; j++) {
				if (i == j) continue;

				value = sqrt(
					(epucks[i]->pos.x - epucks[j]->pos.x) * (epucks[i]->pos.x - epucks[j]->pos.x) +
					(epucks[i]->pos.y - epucks[j]->pos.y) * (epucks[i]->pos.y - epucks[j]->pos.y)
				);

				d_max += value;
      }
    }

    return d_max / totalRobots;
  }

	int fitness() {
		return (fitness_partial_2() * 90 + time_solution * 10) / 100;
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
		int border = 5;
		int matrix[world_width+border*2][world_height+border*2];
		for (int i = 0; i < world_width + border*2; i++) {
			for (int j = 0; j < world_width + border*2; j++) {
				matrix[i][j] = -1;
			}
		}

		for (int i = 0; i < world_width; i++) {
			for (int j = 0; j < world_width; j++) {
				matrix[i+border][j+border] = 0;
			}
		}

		for (int i = 0; i < totalRobots; i++) {
			if (epucks[i]->getColor() == Color(0,0,1)) {
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y)] = 1;
				matrix[border+int(epucks[i]->pos.x)+1][border+int(epucks[i]->pos.y)] = 1;
				matrix[border+int(epucks[i]->pos.x)-1][border+int(epucks[i]->pos.y)] = 1;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y+1)] = 1;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y-1)] = 1;
			} else if (epucks[i]->getColor() == Color(1,0,0)) {
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y)] = 2;
				matrix[border+int(epucks[i]->pos.x)+1][border+int(epucks[i]->pos.y)] = 2;
				matrix[border+int(epucks[i]->pos.x)-1][border+int(epucks[i]->pos.y)] = 2;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y+1)] = 2;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y-1)] = 2;
			} else if (epucks[i]->getColor() == Color(0,0,1)) {
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y)] = 3;
				matrix[border+int(epucks[i]->pos.x)+1][border+int(epucks[i]->pos.y)] = 3;
				matrix[border+int(epucks[i]->pos.x)-1][border+int(epucks[i]->pos.y)] = 3;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y+1)] = 3;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y-1)] = 3;
			} else if (epucks[i]->getColor() == Color(1,1,0)) {
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y)] = 4;
				matrix[border+int(epucks[i]->pos.x)+1][border+int(epucks[i]->pos.y)] = 4;
				matrix[border+int(epucks[i]->pos.x)-1][border+int(epucks[i]->pos.y)] = 4;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y+1)] = 4;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y-1)] = 4;
			} else if (epucks[i]->getColor() == Color(1,0,0.8)) {
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y)] = 5;
				matrix[border+int(epucks[i]->pos.x)+1][border+int(epucks[i]->pos.y)] = 5;
				matrix[border+int(epucks[i]->pos.x)-1][border+int(epucks[i]->pos.y)] = 5;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y+1)] = 5;
				matrix[border+int(epucks[i]->pos.x)][border+int(epucks[i]->pos.y-1)] = 5;
			}
		}

		FILE *imageFile;

		imageFile=fopen(fileName,"wb");
		if(imageFile==NULL) {
				perror("ERROR: Cannot open output file");
				exit(EXIT_FAILURE);
		}

		fprintf(imageFile,"P6\n");
		fprintf(imageFile,"%d %d\n", world_width + 2*border, world_height + 2*border);
		fprintf(imageFile,"255\n");

		unsigned char pix[world_width + 2*border * world_height + 2*border * 3];

		int index = 0;
		for (int i = 0; i < world_width + 2*border; i++) {
			for (int j = 0; j < world_height + 2*border; j++) {
				if (matrix[i][j] == 1) {
					pix[index++] = 0;
					pix[index++] = 150;
					pix[index++] = 0;
				} else if (matrix[i][j] == -1) {
					pix[index++] = 150;
					pix[index++] = 150;
					pix[index++] = 150;
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
