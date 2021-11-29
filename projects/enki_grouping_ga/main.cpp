#include <QApplication>
#include <QtGui>
#include <iostream>
#include "../utils/openGA/src/openGA.hpp"
#include "viewScene.hpp"
#include "worldTrain.hpp"
#include <string>
#include <vector>
#include "../utils/openGA/src/openGA.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace Enki;
using namespace std;

#define WORLD_WIDTH 600
#define WORLD_HEIGHT 400
#define VARIABLE 4
#define MAX_VALUE 12.8

FILE *fp_evaluate;
FILE *fp_best;

float rand_values() {
	float normalized = ((float) rand() / (RAND_MAX / 2)) - 1;
	return normalized * MAX_VALUE;
}

NoViewerMode view(60, 0, 0, 0, 0, WORLD_WIDTH, WORLD_HEIGHT);
int number = 0;

/// Modelagem do problema
struct MySolution {
	std::vector<double> x;

	MySolution() { }

	MySolution(std::vector<double> x): x(x) { }

	std::string to_string() const
	{
		std::ostringstream out;
		out << "{";
		for(unsigned long i=0;i<x.size();i++)
			out << (i?",":"") << std::setprecision(10) << x[i];
		out << "}";
		return out.str();
	}
};

// resultado da função de fitness
struct MyMiddleCost {
	double cost;
};

typedef EA::Genetic<MySolution,MyMiddleCost> GA_Type;
typedef EA::GenerationType<MySolution,MyMiddleCost> Generation_Type;


void init_genes(MySolution& p,const std::function<double(void)> &rnd01) {
	for(int i=0;i<VARIABLE;i++) {
    p.x.push_back(rand_values());
  }
}

bool eval_solution(const MySolution& p, MyMiddleCost &c) {
	float value = 0;
	int times_test = 5;

	for (int i = 0; i < times_test; i++) {
		view.reset(p.x[0], p.x[1], p.x[2], p.x[3], 0, 0);
		view.run(0);
		value += view.fitness();
	}

	fprintf(fp_evaluate, "%f, %f, %f, %f, %f\n", p.x[0], p.x[1], p.x[2], p.x[3], value);

	c.cost = value / times_test;
	return true;
}

MySolution mutate(const MySolution& X_base, const std::function<double(void)> &rnd01, double shrink_scale) {
	MySolution X_new;

  X_new=X_base;
	X_new.x[rand() % VARIABLE] = rand_values();

	return X_new;
}

MySolution crossover(const MySolution& X1, const MySolution& X2, const std::function<double(void)> &rnd01) {
	MySolution X_new;

	for (unsigned long i=0; i < X1.x.size(); i++) {
		double r = rand() % 2;

    if (r) {
      X_new.x.push_back(X1.x[i]);
    } else {
      X_new.x.push_back(X2.x[i]);
    }
	}
	return X_new;
}

double calculate_SO_total_fitness(const GA_Type::thisChromosomeType &X) {
	return X.middle_costs.cost;
}

void SO_report_generation(int generation_number, const EA::GenerationType<MySolution,MyMiddleCost> &last_generation, const MySolution& best_genes){

	view.reset(best_genes.x[0], best_genes.x[1], best_genes.x[2], best_genes.x[3], ++number, 1);
	view.run(1);

	cout
		<< "Generation [" << generation_number << "], "
		<< "Best=" << last_generation.best_total_cost << ", "
		<< "Average=" << last_generation.average_cost << ", "
		<< "Best genes=(" << best_genes.to_string() << ")" << ", "
		<< "Exe_time=" << last_generation.exe_time
		<< std::endl;

	fprintf(fp_best, "%i, %f, %f, (%s), %f\n", generation_number, last_generation.best_total_cost, last_generation.average_cost, best_genes.to_string().c_str(), last_generation.exe_time);

	// cout
	// 	<< generation_number << "\t"
	// 	<< last_generation.average_cost << "\t"
	// 	<< last_generation.best_total_cost << "\t"
	// 	<< best_genes.x[0] << "\t"
	// 	<< best_genes.x[1]
	// 	<< endl;
}

int main(int argc, char *argv[]) {
	fp_evaluate = fopen ("results/enki_grouping_ga/evaluate.txt", "w+");
	fprintf(fp_evaluate, "v_robot_left, v_robot_right, v_nothing_left, v_nothing_right, value\n");
	fp_best = fopen ("results/enki_grouping_ga/best.txt", "w+");
	fprintf(fp_best, "Generation, Best Cost, Average Cost, Best Genes, Exec Time\n");

	srand (time(NULL));

	bool trainig = argv[1][0] == '1';

	if (trainig) {
		cout
			<< "step" << "\t"
			<< "cost_best" << "\t"
			<< "cost_avg" << "\t"
			<< "genes" << "\t"
			<< "time"
			<< endl;
		EA::Chronometer timer;
		timer.tic();

		GA_Type ga_obj;
		ga_obj.problem_mode=EA::GA_MODE::SOGA;
		ga_obj.multi_threading=false;
		ga_obj.dynamic_threading=false;
		ga_obj.idle_delay_us=0;
		ga_obj.verbose=false;
		ga_obj.population=30;
		ga_obj.user_initial_solutions={ };
		ga_obj.generation_max=200; // geracoes
		ga_obj.calculate_SO_total_fitness=calculate_SO_total_fitness;
		ga_obj.init_genes=init_genes;
		ga_obj.eval_solution=eval_solution;
		ga_obj.mutate=mutate;
		ga_obj.crossover=crossover;
		ga_obj.SO_report_generation=SO_report_generation;
		ga_obj.best_stall_max=20; //repeticoes do melhor
		ga_obj.average_stall_max=5; //repeticoes da media
		ga_obj.tol_stall_best=1e-6;
		ga_obj.tol_stall_average=1e-6;
		ga_obj.elite_count=10;
		ga_obj.crossover_fraction=0.7;
		ga_obj.mutation_rate=0.1;
		ga_obj.solve();

		std::cout<<"The problem is optimized in "<<timer.toc()<<" seconds."<<std::endl;

	} else {
		QApplication app(argc, argv);

		World world(WORLD_WIDTH, WORLD_HEIGHT, Color::gray,  World::GroundTexture());

		EpuckGroupingGA *viewer;

		// v_robot_left, v_robot_right, v_nothing_left, v_nothing_right
		if (argv[2][0] == '0') {
			printf("exp: 0\n");
			viewer = new EpuckGroupingGA(&world, 60, atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), WORLD_WIDTH, WORLD_HEIGHT);
		} else if (argv[2][0] == '1') {
			printf("exp: 1\n");
			// viewer = new EpuckGroupingGA(&world, 60, 9, -9, -9, -12.8, WORLD_WIDTH, WORLD_HEIGHT);
			viewer = new EpuckGroupingGA(&world, 60, -9, 9, -12.8, -9, WORLD_WIDTH, WORLD_HEIGHT);
		} else if (argv[2][0] == '2') {
			printf("exp: 2\n");
			viewer = new EpuckGroupingGA(&world, 60, 3, 12.8, -12.8, -10.5, WORLD_WIDTH, WORLD_HEIGHT);
		} else if (argv[2][0] == '3') {
			printf("exp: 3\n");
			viewer = new EpuckGroupingGA(&world, 60, 8, 12.8, -12.8, -9, WORLD_WIDTH, WORLD_HEIGHT);
		} else {
			printf("exp: 4\n");
			viewer = new EpuckGroupingGA(&world, 60, -10, 6, 10.5, 3.5, WORLD_WIDTH, WORLD_HEIGHT);
		}

		viewer->show();
		app.exec();
	}

	fclose(fp_evaluate);
	fclose(fp_best);

	return 0;
}
