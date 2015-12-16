// GameOfLife.cpp : Defines the entry point for the console application.
//

#include "Settings.h"
#include <vector>
#include <iostream>

#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include "UniverseModifier.h"

using namespace tbb;
using namespace std;

int main() {
	int thread_count = DEFAULT_NUMBER_OF_THREADS;
	size_t universe_size_x = UNIVERSE_X_COUNT;
	size_t universe_size_y = UNIVERSE_Y_COUNT;
	size_t total_time_steps = DEFAULT_TOTAL_TIME_STEPS;
	size_t default_live_cell_count = DEFAULT_INIT_LIVE_CELLS;
	size_t default_neighborhood_size = DEFAULT_NEIGHBORHOOD_SIZE;

	// Set TBB thread count system automatic (usually as many as the system's cores)
	//task_scheduler_init init(task_scheduler_init::automatic);

	// User input data
	universe_size_x = 10;
	universe_size_y = 10;
	thread_count = 4;
	total_time_steps = 10;
	default_live_cell_count = 10;

	task_scheduler_init init(thread_count); // Set the number of threads

	if (universe_size_x > 0 && universe_size_y > 0 && total_time_steps > 0) {

		// Print calculation info
		cout << "= Parallel Game Of Life serially and with Thread Building Blocks =" << endl;
		cout << "Number of threads: " << thread_count << endl;
		cout << "Total time steps: " << total_time_steps << endl;
		cout << "Grid Size: " << universe_size_x << " x " << universe_size_y << endl << endl;

		// Advance Game Of Life
		tick_count before, after; // Execution timers

		// Initialized grid
		vector<bool> universe_grid(universe_size_x * universe_size_y);

		// Put random live cells
		UniverseModifier grid_modifier;
		grid_modifier.allocate_random_live_cells(default_live_cell_count, universe_grid, universe_size_x, universe_size_y);

		// Serial execution
		before = tick_count::now();
		// Advance Game Of Life serially
		after = tick_count::now();
		cout << endl << "Serial execution: " << 1000 * (after - before).seconds() << " ms" << endl;

		// Thread Building Blocks		
		before = tick_count::now();
		// Advance Game Of Life serially with TBB
		after = tick_count::now();
		cout << endl << "Thread Building Blocks execution: " << 1000 * (after - before).seconds() << " ms" << endl;

		// Assert

		// Show universe
		grid_modifier.debug_show_universe(universe_grid, universe_size_x, universe_size_y);

		system("pause");
	}

    return 0;
}

