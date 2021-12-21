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
#include <vector>
#include<thread>

using namespace Enki;
using namespace std;

#define WORLD_WIDTH 600
#define WORLD_HEIGHT 400
#define VARIABLE 6
#define ROBOT_N 15
#define INDIVIDUAL_N 10
#define MAX_VALUE 12.8
#define TIMES_TEST 3
#define BATCHS 6

int ind_id = 0;
int number = 0;
FILE *fp_evaluate;
FILE *fp_best;
static std::mutex m;
float value[TIMES_TEST];
std::thread thread_objs[TIMES_TEST];
vector <NoViewerMode*> views;

float rand_values() {
	float normalized = ((float) rand() / (RAND_MAX / 2)) - 1;
	return normalized * MAX_VALUE;
}

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

const MySolution *p_to_eval;

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

void init_words() {
	for(int i=0; i < TIMES_TEST; i++) {
		NoViewerMode *view = new NoViewerMode(ROBOT_N, BATCHS, 0, 0, 0, 0, 0, 0, WORLD_WIDTH, WORLD_HEIGHT);

		views.push_back(view);
	}
}

void eval_t(int t_id) {
	EA::Chronometer timer_aux;
	timer_aux.tic();

	views[t_id]->reset(p_to_eval->x[0], p_to_eval->x[1], p_to_eval->x[2], p_to_eval->x[3], p_to_eval->x[4], p_to_eval->x[5], 0, 0);
	views[t_id]->run(0);
	value[t_id] = views[t_id]->fitness();

	m.lock();
	fp_evaluate = fopen("results/enki_grouping_mult_color_ga/evaluate.txt", "a+");
	fprintf(fp_evaluate, "%i, %i, %i, %f, %f, %f, %f, %f, %f, %f, %f\n", number, ind_id, t_id, p_to_eval->x[0], p_to_eval->x[1], p_to_eval->x[2], p_to_eval->x[3], p_to_eval->x[4], p_to_eval->x[5], value[t_id], timer_aux.toc());
	fclose(fp_evaluate);
	m.unlock();
}

bool eval_solution(const MySolution& p, MyMiddleCost &c) {
	p_to_eval = &p;

	float value_media = 0;
	ind_id++;

	for (int i = 0; i < TIMES_TEST; i++) {
    value[i] = 0;
    thread_objs[i] = std::thread(eval_t, i);
  }

  for (int i = 0; i < TIMES_TEST; i++) {
    thread_objs[i].join();
    thread_objs[i].thread::~thread();
  }

	for (int i = 0; i < TIMES_TEST; i++) {
		value_media += value[i] / TIMES_TEST;
	}

	c.cost = value_media;
	return true;
}

MySolution mutate(const MySolution& X_base, const std::function<double(void)> &rnd01, double shrink_scale) {
	MySolution X_new;

  X_new=X_base;

	int genes_matate = rand() % VARIABLE + 1;
	for (int i = 0; i < genes_matate; i++) {
		X_new.x[rand() % VARIABLE] = rand_values();
	}

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

	views[0]->reset(best_genes.x[0], best_genes.x[1], best_genes.x[2], best_genes.x[3], best_genes.x[4], best_genes.x[5], ++number, 1);
	views[0]->run(1);

	for (int i = 0; i < TIMES_TEST; i++) {
		views[0]->reset_points(TIMES_TEST);
	}

	cout
		<< "Generation [" << generation_number << "], "
		<< "Best=" << last_generation.best_total_cost << ", "
		<< "Average=" << last_generation.average_cost << ", "
		<< "Best genes=(" << best_genes.to_string() << ")" << ", "
		<< "Exe_time=" << last_generation.exe_time
		<< std::endl;

	fp_best = fopen("results/enki_grouping_mult_color_ga/best.txt", "a+");
	fprintf(fp_best, "%i, %f, %f, (%s), %f\n", generation_number, last_generation.best_total_cost, last_generation.average_cost, best_genes.to_string().c_str(), last_generation.exe_time);
	fclose(fp_best);
}

int main(int argc, char *argv[]) {
	srand (time(NULL));

	bool trainig = argv[1][0] == '1';

	if (trainig) {
		init_words();

		fp_evaluate = fopen ("results/enki_grouping_mult_color_ga/evaluate.txt", "w+");
		fprintf(fp_evaluate, "gen_id, ind_id, test_id, v_similar_robot_left, v_similar_robot_right, v_diferent_robot_left, v_diferent_robot_right, v_nothing_left, v_nothing_right, value\n");
		fp_best = fopen ("results/enki_grouping_mult_color_ga/best.txt", "w+");
		fprintf(fp_best, "Generation, Best Cost, Average Cost, Best Genes, Exec Time\n");
		fclose(fp_evaluate);
		fclose(fp_best);

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
		ga_obj.population=INDIVIDUAL_N;
		ga_obj.user_initial_solutions={
			// MySolution({4.993422031,2.112761021,0.5469299555,-5.600748539,-5.945645332,9.778637886}),
			// MySolution({1.810392737,3.364384413,0.5469299555,2.190844774,-5.732907295,-1.758403778}),
			// MySolution({-1.156605482,3.364384413,10.49389076,2.190844774,-5.732907295,-1.758403778}),
			// MySolution({9.221704483,10.00989342,-7.000594139,2.190844774,-5.732907295,6.723426819}),
			// MySolution({9.221704483,10.00989342,-7.000594139,2.190844774,-7.077551365,-1.758403778}),
			// MySolution({5.154663086,4.03364563,4.678225517,9.345822334,-6.364855766,8.264378548}),
			// MySolution({5.154663086,4.03364563,-5.498818874,7.333633423,-9.482215881,2.197157383}),
			// MySolution({9.221704483,4.03364563,0.5469299555,9.345822334,-5.732907295,8.264378548}),
			// MySolution({8.444081306,7.131147861,-9.399335861,3.244485378,4.76662302,5.808334351}),
		};
		ga_obj.generation_max=500; // geracoes
		ga_obj.calculate_SO_total_fitness=calculate_SO_total_fitness;
		ga_obj.init_genes=init_genes;
		ga_obj.eval_solution=eval_solution;
		ga_obj.mutate=mutate;
		ga_obj.crossover=crossover;
		ga_obj.SO_report_generation=SO_report_generation;
		ga_obj.best_stall_max=100; //repeticoes do melhor
		ga_obj.average_stall_max=100; //repeticoes da media
		ga_obj.tol_stall_best=1e-6;
		ga_obj.tol_stall_average=1e-6;
		ga_obj.elite_count=10;
		ga_obj.crossover_fraction=0.7;
		ga_obj.mutation_rate=0.3;
		ga_obj.solve();

		std::cout<<"The problem is optimized in "<<timer.toc()<<" seconds."<<std::endl;

	} else {
		QApplication app(argc, argv);

		World world(WORLD_WIDTH, WORLD_HEIGHT, Color::gray,  World::GroundTexture());

		EpuckGroupingGA *viewer;

		if (argv[2][0] == '0') {
			printf("exp: 0 <%f, %f, %f, %f, %f, %f>\n", atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]), atof(argv[8]));
			viewer = new EpuckGroupingGA(&world, ROBOT_N, BATCHS, atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]), atof(argv[8]), WORLD_WIDTH, WORLD_HEIGHT);
		}

		viewer->show();
		app.exec();
	}

	return 0;
}
