// GameOfLife.cpp : Defines the entry point for the console application.
//

#include "Settings.h"
#include <vector>
#include <iostream>

#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/cache_aligned_allocator.h"
#include "tbb/concurrent_vector.h"
#include "UniverseModifier.h"
#include <cassert>

using namespace tbb;
using namespace std;

void simulate_serial(vector<bool>& universe, size_t universe_size_x, size_t universe_size_y, size_t total_time_steps) {

	UniverseModifier universe_modifier;

	for (size_t i = 0; i < total_time_steps; ++i) {
		universe_modifier.advance_universe(universe, universe_size_x, universe_size_y);
	}
}

void simulate_tbb(concurrent_vector<bool>& universe, size_t universe_size_x, size_t universe_size_y, size_t total_time_steps) {

	UniverseModifier universe_modifier;

	for (size_t i = 0; i < total_time_steps; ++i) {
		universe_modifier.advance_universe_tbb(universe, universe_size_x, universe_size_y);
	}
}

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
	universe_size_x = 7;
	universe_size_y = 3;
	thread_count = 4;
	total_time_steps = 2;
	default_live_cell_count = 13;

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
		vector<bool> init_universe_grid;
		init_universe_grid.resize(universe_size_x * universe_size_y);
		
		// Put random live cells
		UniverseModifier grid_modifier;
		grid_modifier.allocate_random_live_cells(default_live_cell_count, init_universe_grid, universe_size_x, universe_size_y);

		// Show Init universe
		if (VERBOSE) {
			cout << "Init Universe" << endl;
			grid_modifier.debug_show_universe(init_universe_grid, universe_size_x, universe_size_y);
		}

		// Copy the grids
		vector<bool> universe_serial(init_universe_grid);
		concurrent_vector<bool, cache_aligned_allocator<bool>> universe_tbb(UniverseModifier::to_concurrent_vector(init_universe_grid));

		// Serial execution
		before = tick_count::now();
		simulate_serial(universe_serial, universe_size_x, universe_size_y, total_time_steps); // Advance Game Of Life serially
		after = tick_count::now();
		cout << endl << "Serial execution: " << 1000 * (after - before).seconds() << " ms" << endl;

		// Thread Building Blocks		
		before = tick_count::now();
		simulate_tbb(universe_tbb, universe_size_x, universe_size_y, total_time_steps); // Advance Game Of Life with TBB
		after = tick_count::now();
		cout << endl << "Thread Building Blocks execution: " << 1000 * (after - before).seconds() << " ms" << endl;

		// Assert
		assert(UniverseModifier::are_equal(universe_serial, UniverseModifier::to_vector(universe_tbb)));

		// Show universe
		if (VERBOSE) {

			cout << "Final Universe Serial" << endl;
			grid_modifier.debug_show_universe(universe_serial, universe_size_x, universe_size_y);

			cout << "Final Universe TBB" << endl;
			grid_modifier.debug_show_universe(UniverseModifier::to_vector(universe_tbb), universe_size_x, universe_size_y);
		}

		system("pause");
	}

    return 0;
}

