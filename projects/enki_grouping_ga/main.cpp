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

#define WORLD_WIDTH 500
#define WORLD_HEIGHT 500
#define VARIABLE 4

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
    p.x.push_back(rand() % 19 - 9);
  }
}

bool eval_solution(const MySolution& p, MyMiddleCost &c) {
	int value = 0;

	for (int i = 0; i < 3; i++) {
		view.reset(p.x[0], p.x[1], p.x[2], p.x[3], 0, 0);
		view.run(0);
		value += view.fitness();
	}


	c.cost = value;
	return true;
}

MySolution mutate(const MySolution& X_base, const std::function<double(void)> &rnd01, double shrink_scale) {
	MySolution X_new;

  X_new=X_base;
	X_new.x[rand() % VARIABLE] = rand() % 19 - 9;

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

	// cout
	// 	<< generation_number << "\t"
	// 	<< last_generation.average_cost << "\t"
	// 	<< last_generation.best_total_cost << "\t"
	// 	<< best_genes.x[0] << "\t"
	// 	<< best_genes.x[1]
	// 	<< endl;
}

int main(int argc, char *argv[]) {
	// srand (time(NULL));
	bool trainig = true;

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
		EpuckGroupingGA viewer(&world, 50, -1,-3,0,-7);

		viewer.show();
		app.exec();
	}

	return 0;
}
