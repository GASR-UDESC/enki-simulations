#include "EPuckController.hpp"

using namespace Enki;
using namespace std;

class NoViewerMode {
	private:
		int batchRobotsSize;
		int batchs;
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
    QVector<QVector<Point>> point_epucks;
    QVector<Color> colors = {Color(0,1,0), Color(0,0,1), Color(1,0,0), Color(1,1,0), Color(1, 0.5, 0), Color(1,1,1)};

	public:
	NoViewerMode(
		int batchRobotsSize,
		int batchs,
		float v_similar_robot_left,
		float v_similar_robot_right,
		float v_diferent_robot_left,
		float v_diferent_robot_right,
		float v_nothing_left,
		float v_nothing_right,
		int world_width,
		int world_height,
		int times_test
	):
		totalRobots(0),
		time_solution(0),
		max_i(15000),
		border(10),
		img_v_frames(4),
		img_h_frames(4),
		current_frame_y(0),
		current_frame_x(0),
		world_width(world_width),
		world_height(world_height),
		batchRobotsSize(batchRobotsSize),
		batchs(batchs),
		world(world_width, world_height)
	{
		initialize_batchs(batchs, v_similar_robot_left, v_similar_robot_right, v_diferent_robot_left, v_diferent_robot_right, v_nothing_left, v_nothing_right);
		initialize_img();
		initialize_points(times_test);
	}

	void initialize_batchs(int batchs, float v_similar_robot_left, float v_similar_robot_right, float v_diferent_robot_left, float v_diferent_robot_right, float v_nothing_left, float v_nothing_right) {
		for(int i = 0; i < batchs; i++) {
			initiazeEpucks(colors[i], v_similar_robot_left, v_similar_robot_right, v_diferent_robot_left, v_diferent_robot_right, v_nothing_left, v_nothing_right);
		}
	}

	void initiazeEpucks(Color color, float v_similar_robot_left, float v_similar_robot_right, float v_diferent_robot_left, float v_diferent_robot_right, float v_nothing_left, float v_nothing_right) {
    for (int i = 0; i < batchRobotsSize; ++i) {
      EPuckController *epuck = new EPuckController(v_similar_robot_left, v_similar_robot_right, v_diferent_robot_left, v_diferent_robot_right, v_nothing_left, v_nothing_right, EPuckController::CAPABILITY_CAMERA);
      epuck->pos = Point(UniformRand(0, world_width)(), UniformRand(0, world_height)());
      epuck->setColor(color);

      epucks.push_back(epuck);
      world.addObject(epuck);
    }

    totalRobots += batchRobotsSize;
	}

	void initialize_points(int times_test) {
		for (int i = 0; i < times_test; ++i) {
			QVector<Point> aux;
			for (int j = 0; j < totalRobots; ++j) {
				aux.push_back(Point(UniformRand(0, world_width)(), UniformRand(0, world_height)()));
			}
			point_epucks.push_back(aux);
		}
	}

	void run(int f_img) {
		for (time_solution = 0; time_solution<max_i; time_solution++) {
			for (int j = 0; j < totalRobots; j++) {
        epucks[j]->move();
      }

			// values from viewer/Viewer.cpp
			world.step(0.3, 3);

			if (f_img) {
				if (time_solution % (max_i / (img_v_frames * img_h_frames - 1)) == 0 && ((current_frame_y+1) != img_v_frames || (current_frame_x+1) != img_h_frames) && time_solution != 0) {
					add_frame_to_img();
				}
			}
		}

		if (f_img) {
			add_frame_to_img();
			create_img();
		}
	}

  float diameter(int batch_number) {
		float d_max = 0;
		float value = 0;

		int start = batch_number * batchRobotsSize;
		int end = (batch_number + 1) * batchRobotsSize;

    for (int i = start; i < end; i++) {
      for (int j = start; j < end; j++) {
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

	float small_distance_between_diferent_colors(int batch_number) {
		float d_min = 600;
		float value = 0;

		int start = batch_number * batchRobotsSize;
		int end = (batch_number + 1) * batchRobotsSize;

    for (int i = start; i < end; i++) {
      for (int j = 0; j < totalRobots; j++) {
				if (j >= start && j < end) continue;

				value = sqrt(
					(epucks[i]->pos.x - epucks[j]->pos.x) * (epucks[i]->pos.x - epucks[j]->pos.x) +
					(epucks[i]->pos.y - epucks[j]->pos.y) * (epucks[i]->pos.y - epucks[j]->pos.y)
				);

				if (value < d_min) {
					d_min = value;
				}
      }
    }

    return d_min;
  }

	int fitness() {
		// return diameter(0) + diameter(1);
		int value = 0;

		for(int i = 0; i < batchs; i++) {
			int a3 = 0;
			int a = diameter(0);
			int a2 = small_distance_between_diferent_colors(0);
			if (a > a2) {
				a3 = a + ( a - a2);
			} else {
				a3 = a;
			}

			value += a3;
		}

		return value;
	}

  void reset(float v_similar_robot_left, float v_similar_robot_right, float v_diferent_robot_left, float v_diferent_robot_right, float v_nothing_left, float v_nothing_right, int num, int f_img, int test_number) {
    number = num;
    for (int i = 0; i < totalRobots; i++) {
      epucks[i]->pos = point_epucks[test_number][i];
      epucks[i]->v_similar_robot_left = v_similar_robot_left;
      epucks[i]->v_similar_robot_right = v_similar_robot_right;
      epucks[i]->v_diferent_robot_left = v_diferent_robot_left;
      epucks[i]->v_diferent_robot_right = v_diferent_robot_right;
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

		for (int i = 0; i < batchs; i++) {
			int color = i+1;

			for (int j = 0; j < batchRobotsSize; j++) {
				int x = off_set_x + int(epucks[j+(i*batchRobotsSize)]->pos.x);
				int y = off_set_y + int(epucks[j+(i*batchRobotsSize)]->pos.y);

				matrix_img[y][x] = color;

				matrix_img[y-2][x-1] = color;
				matrix_img[y-2][x] = color;
				matrix_img[y-2][x+1] = color;

				matrix_img[y+2][x-1] = color;
				matrix_img[y+2][x] = color;
				matrix_img[y+2][x+1] = color;

				matrix_img[y-1][x-2] = color;
				matrix_img[y][x-2] = color;
				matrix_img[y+1][x-2] = color;

				matrix_img[y-1][x+2] = color;
				matrix_img[y][x+2] = color;
				matrix_img[y+1][x+2] = color;
			}
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
		string name = "results/enki_grouping_mult_color_ga/best" + std::to_string(number) + ".ppm";
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

    // QVector<Color> colors = {Color(0,1,0), Color(0,0,1), Color(1,0,0), Color(1,1,0), Color(0.6, 0.2, 1), Color(1,1,1)};
		int index = 0;
		for (int i = 0; i < img_height; i++) {
			for (int j = 0; j < img_width; j++) {
				if (matrix_img[i][j] == 1) {
					pix[index++] = 0;
					pix[index++] = 255;
					pix[index++] = 0;
				} else if (matrix_img[i][j] == 2) {
					pix[index++] = 0;
					pix[index++] = 0;
					pix[index++] = 255;
				} else if (matrix_img[i][j] == 3) {
					pix[index++] = 255;
					pix[index++] = 0;
					pix[index++] = 0;
				} else if (matrix_img[i][j] == 4) {
					pix[index++] = 255;
					pix[index++] = 255;
					pix[index++] = 0;
				} else if (matrix_img[i][j] == 5) {
					pix[index++] = 255;
					pix[index++] = 127;
					pix[index++] = 0;
				} else if (matrix_img[i][j] == 6) {
					pix[index++] = 255;
					pix[index++] = 255;
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
