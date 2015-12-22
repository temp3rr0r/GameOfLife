// GameOfLife.cpp : Defines the entry point for the console application.
//

#include "lodepng.h"
#include "Settings.h"
#include <vector>
#include <iostream>

#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/cache_aligned_allocator.h"
#include "tbb/concurrent_vector.h"
#include "UniverseModifier.h"
#include <cassert>
#include <string>
#include <tbb/parallel_for.h>

using namespace tbb;
using namespace std;

inline uint8_t count_alive_cells(size_t x0, size_t x1, size_t x2, size_t y0, size_t y1, size_t y2, const vector<bool>& universe) {
	return universe[x0 + y0] + universe[x1 + y0] + universe[x2 + y0]
		+ universe[x0 + y1] + universe[x2 + y1]
		+ universe[x0 + y2] + universe[x1 + y2] + universe[x2 + y2];
}

inline uint8_t count_alive_cells_tbb(size_t x0, size_t x1, size_t x2, size_t y0, size_t y1, size_t y2, const concurrent_vector<bool>& universe) {
	return universe[x0 + y0] + universe[x1 + y0] + universe[x2 + y0]
		+ universe[x0 + y1] + universe[x2 + y1]
		+ universe[x0 + y2] + universe[x1 + y2] + universe[x2 + y2];
}

void simulate_tbb(concurrent_vector<bool>& universe, size_t universe_size_x, size_t universe_size_y, size_t total_time_steps) {
	
	const size_t universe_size_local = universe.size();
	const size_t universe_size_y_local = universe_size_y;
	concurrent_vector<bool, cache_aligned_allocator<bool>> new_universe(universe_size_local);

	for (size_t i = 0; i < total_time_steps; ++i) {		
		parallel_for(blocked_range<size_t>(0, universe_size_local),
			[&](const blocked_range<size_t>& r) {

				const size_t begin = r.begin();
				const size_t end = r.end();				

				for (size_t index = begin; index != end; ++index) {
					size_t x1 = index % universe_size_y_local;
					size_t y1 = index - x1;

					uint8_t aliveCells = count_alive_cells_tbb(
						(x1 + universe_size_y_local - 1) % universe_size_y_local, // x0,
						x1,
						(x1 + 1) % universe_size_y_local, //x2
						(y1 + universe_size_local - universe_size_y_local) % universe_size_local, // y0
						y1,
						(y1 + universe_size_y_local) % universe_size_local, // y2
						universe);
					new_universe[y1 + x1] =
						aliveCells == REPRODUCTION_COUNT || (aliveCells == UNDERPOPULATION_COUNT && universe[x1 + y1]);
				}
			}
		);

		std::swap(universe, new_universe);
		if (SAVE_ALL_PNG_STEPS) {

			string file_name = "universe_serial_timestep_" + to_string(i) + ".png";
			UniverseModifier universe_modifier;
			universe_modifier.universe_to_png(universe_modifier.to_vector(universe), universe_size_x, universe_size_y, file_name.c_str());
		}
	}
}

void simulate_serial(vector<bool>& universe, size_t universe_size_x, size_t universe_size_y, size_t total_time_steps) {

	vector<bool> new_universe(universe.size());
	UniverseModifier universe_modifier;

	for (size_t i = 0; i < total_time_steps; ++i) {
		for (size_t y = 0; y < universe_size_x; ++y) {
			size_t y0 = ((y + universe_size_x - 1) % universe_size_x) * universe_size_y;
			size_t y1 = y * universe_size_y;
			size_t y2 = ((y + 1) % universe_size_x) * universe_size_y;

			for (size_t x = 0; x < universe_size_y; ++x) {
				size_t x0 = (x + universe_size_y - 1) % universe_size_y;
				size_t x2 = (x + 1) % universe_size_y;

				uint8_t aliveCells = count_alive_cells(x0, x, x2, y0, y1, y2, universe);
				new_universe[y1 + x] =
					aliveCells == 3 || (aliveCells == 2 && universe[x + y1]) ? 1 : 0;
			}
		}
		std::swap(universe, new_universe);
		if (SAVE_ALL_PNG_STEPS) {

			string file_name = "universe_serial_timestep_" + to_string(i) + ".png";

			universe_modifier.universe_to_png(universe, universe_size_x, universe_size_y, file_name.c_str());
		}
	}
}

int main() {
	int thread_count = DEFAULT_NUMBER_OF_THREADS;
	size_t universe_size_x = UNIVERSE_X_COUNT;
	size_t universe_size_y = UNIVERSE_Y_COUNT;
	size_t total_time_steps = DEFAULT_TOTAL_TIME_STEPS;
	size_t live_cells_count = DEFAULT_INIT_LIVE_CELLS;
	float live_cells_proportion = DEFAULT_INIT_LIVE_CELLS_PROPORTION;
	size_t neighborhood_size = DEFAULT_NEIGHBORHOOD_SIZE;

	// Set TBB thread count system automatic (usually as many as the system's cores)
	//task_scheduler_init init(task_scheduler_init::automatic);

	// User input data
	universe_size_y = 4000;
	universe_size_x = 2000;
	thread_count = 4;
	total_time_steps = 10;
	live_cells_proportion = 0.6;
	live_cells_count = static_cast<size_t>(universe_size_x * universe_size_y * live_cells_proportion);

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
		grid_modifier.allocate_random_live_cells(live_cells_count, init_universe_grid, universe_size_x, universe_size_y);

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

		if (SAVE_PNG) {
			grid_modifier.universe_to_png(universe_serial, universe_size_x, universe_size_y, "final_serial_universe.png");
			grid_modifier.universe_to_png(grid_modifier.to_vector(universe_tbb), universe_size_x, universe_size_y, "final_tbb_universe.png");
		}
		
		system("pause");
	}

    return 0;
}

