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
		int max_i;
    int number;
		int border;
		int img_height;
		int img_width;
		int current_frame_y;
		int current_frame_x;
		int img_v_frames;
		int img_h_frames;
		int **matrix_img;

		World world;

	protected:
    QVector<EPuckController*> epucks;

	public:
	NoViewerMode(
		int batchRobotsSize,
		float v_robot_left,
		float v_robot_right,
		float v_nothing_left,
		float v_nothing_right,
		int world_width,
		int world_height
	):
		totalRobots(0),
		time_solution(0),
		max_i(10000),
		border(10),
		img_v_frames(3),
		img_h_frames(3),
		current_frame_y(0),
		current_frame_x(0),
		world_width(world_width),
		world_height(world_height),
		batchRobotsSize(batchRobotsSize),
		world(world_width, world_height)
	{
		initiazeEpucks(Color(0,0,1), batchRobotsSize, v_robot_left, v_robot_right, v_nothing_left, v_nothing_right);
		initialize_img();
	}

	void initiazeEpucks(Color color, int n, float v_robot_left, float v_robot_right, float v_nothing_left, float v_nothing_right) {
    for (int i = 0; i < n; ++i) {
      EPuckController *epuck = new EPuckController(v_robot_left, v_robot_right, v_nothing_left, v_nothing_right, EPuckController::CAPABILITY_CAMERA);
      epuck->pos = Point(UniformRand(0, world_width)(), UniformRand(0, world_height)());
      epuck->setColor(color);

      epucks.push_back(epuck);
      world.addObject(epuck);
    }

    totalRobots += n;
	}

	void run(int f_img) {
		for (time_solution = 0; time_solution<max_i; time_solution++) {
			for (int j = 0; j < totalRobots; j++) {
        epucks[j]->move();
      }

			// values from viewer/Viewer.cpp
			world.step(30.0/1000.0, 3);

			if (f_img) {
				if (time_solution % (max_i / (img_v_frames * img_h_frames - 1)) == 0 && time_solution != 0) {
					add_frame_to_img();
				}
			}
		}

		if (f_img) {
			add_frame_to_img();
			create_img();
		}
	}

  float diameter() {
		float d_max = 0;
		float value = 0;

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

	int fitness() {
		// parametro de regularização
		// return (diameter() * 90 + time_solution * 10) / 100;
		return diameter();
	}

  void reset(float v_robot_left, float v_robot_right, float v_nothing_left, float v_nothing_right, int num, int f_img) {
    number = num;
    for (int i = 0; i < totalRobots; i++) {
      epucks[i]->pos = Point(UniformRand(0, world_width)(), UniformRand(0, world_height)());
      epucks[i]->v_robot_left = v_robot_left;
      epucks[i]->v_robot_right = v_robot_right;
      epucks[i]->v_nothing_left = v_nothing_left;
      epucks[i]->v_nothing_right = v_nothing_right;
    }

		if (f_img) {
			clean_img();
			add_frame_to_img();
		}
  }

	void initialize_img() {
		img_height = world_height * img_v_frames + border * (img_v_frames + 1);
		img_width = world_width * img_h_frames + border * (img_h_frames + 1);

		matrix_img = (int **) malloc (img_height * sizeof(int *));
		for (int i = 0; i < img_height; i++) {
			matrix_img[i] = (int *) malloc (img_width * sizeof(int));
		}
	}

	void clean_img() {
		for (int i = 0; i < img_height; i++) {
			for (int j = 0; j < img_width; j++) {
				matrix_img[i][j] = -1;
			}
		}
	}

	void add_frame_to_img() {
		int off_set_x = current_frame_x * world_width + border * (current_frame_x+1);
		int off_set_y = current_frame_y * world_height + border * (current_frame_y+1);

		for (int i = off_set_y; i < off_set_y + world_height; i++) {
			for (int j = off_set_x; j < off_set_x + world_width; j++) {
				matrix_img[i][j] = 0;
			}
		}

		for (int i = 0; i < totalRobots; i++) {
			int x = off_set_x + int(epucks[i]->pos.x);
			int y = off_set_y + int(epucks[i]->pos.y);

			// for (int i = -2; i< 3; i++) {
			// 	for (int j = -2; j< 3; j++) {
			// 		matrix_img[y+i][x+j] = 1;
			// 	}
			// }

			matrix_img[y][x] = 2;

			matrix_img[y-2][x-1] = 1;
			matrix_img[y-2][x] = 1;
			matrix_img[y-2][x+1] = 1;

			matrix_img[y+2][x-1] = 1;
			matrix_img[y+2][x] = 1;
			matrix_img[y+2][x+1] = 1;

			matrix_img[y-1][x-2] = 1;
			matrix_img[y][x-2] = 1;
			matrix_img[y+1][x-2] = 1;

			matrix_img[y-1][x+2] = 1;
			matrix_img[y][x+2] = 1;
			matrix_img[y+1][x+2] = 1;
		}

		current_frame_x++;

		if (current_frame_x == img_h_frames) {
			current_frame_x = 0;
			current_frame_y ++;
		}

		if (current_frame_y == img_h_frames) {
			current_frame_y = 0;
		}
	}

	void create_img() {
		string name = "results/enki_grouping_ga/best" + std::to_string(number) + ".ppm";
		int n = name.length();
		char *fileName;
		fileName = (char *) malloc(sizeof(char) * (n + 2));
		strcpy(fileName, name.c_str());

		FILE *imageFile;

		imageFile=fopen(fileName,"wb");
		if(imageFile==NULL) {
				perror("ERROR: Cannot open output file");
				exit(EXIT_FAILURE);
		}

		fprintf(imageFile,"P6\n");
		fprintf(imageFile,"%d %d\n", img_width, img_height);
		fprintf(imageFile,"255\n");

		unsigned char pix[img_width * img_height * 3];

		int index = 0;
		for (int i = 0; i < img_height; i++) {
			for (int j = 0; j < img_width; j++) {
				if (matrix_img[i][j] == 1) {
					pix[index++] = 255;
					pix[index++] = 0;
					pix[index++] = 0;
				} else if (matrix_img[i][j] == 2) {
					pix[index++] = 0;
					pix[index++] = 0;
					pix[index++] = 255;
				} else if (matrix_img[i][j] == -1) {
					pix[index++] = 255;
					pix[index++] = 255;
					pix[index++] = 255;
				} else {
					pix[index++] = 0;
					pix[index++] = 0;
					pix[index++] = 0;
				}
			}
		}

		fwrite(pix, 1, img_width * img_height * 3, imageFile);
		fclose(imageFile);
	}
};
