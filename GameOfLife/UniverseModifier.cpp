#include "UniverseModifier.h"
#include <iostream>
#include "Settings.h"
#include <tbb/cache_aligned_allocator.h>
#include <tbb/concurrent_vector.h>
#include <tbb/tbb.h>
#include "lodepng.h"

using namespace tbb;

void UniverseModifier::allocate_random_live_cells(size_t live_cells_count, std::vector<bool>& grid, size_t size_x, size_t size_y) {
	if (live_cells_count > 0) {
		std::mt19937 mersenne_twister_engine(random_device_());
		std::uniform_int_distribution<> cell_distribution(0, static_cast<int>((size_x * size_y) - 1));

		for (size_t i = 0; i < live_cells_count; ++i)
			grid[cell_distribution(mersenne_twister_engine)] = true;
	}
}

void UniverseModifier::universe_to_png(const std::vector<bool>& universe, size_t universe_size_x, size_t universe_size_y, const char* filename) const {
	// Generate the image
	uint32_t width = static_cast<uint32_t>(universe_size_y);
	uint32_t height = static_cast<uint32_t>(universe_size_x);
	std::vector<uint8_t> image;
	image.resize(width * height * 4);

	for (uint32_t x = 0; x < height; x++) {
		for (uint32_t y = 0; y < width; y++) {
			uint32_t red = 0;
			if (universe[UniverseModifier::get_vector_index(static_cast<size_t>(x), static_cast<size_t>(y), universe_size_y)] == ALIVE)
				red = 255;
			image[4 * width * x + 4 * y + 0] = red;
			image[4 * width * x + 4 * y + 1] = 0;
			image[4 * width * x + 4 * y + 2] = 0;
			image[4 * width * x + 4 * y + 3] = 255;
		}
	}

	lodepng::encode(filename, image, width, height);
}

void UniverseModifier::debug_show_universe(const std::vector<bool>& universe, size_t size_x, size_t size_y) {

	std::cout << "Universe:" << std::endl;

	for (size_t x = 0; x < size_x; ++x) {
		for (size_t y = 0; y < size_y; ++y) {
			std::cout << static_cast<int>(universe[get_vector_index(x, y, size_y)]) << "\t";
		}
		std::cout << std::endl;
	}
}

size_t UniverseModifier::get_vector_index(size_t x, size_t y, size_t cell_count) {
	return (x * cell_count) + y;
}

size_t UniverseModifier::get_count(const std::vector<bool>& input_vector, bool state) {
	size_t returning_count = 0;
	for (const bool &arrayElement : input_vector) {
		if (arrayElement == state)
			returning_count++;
	}
	return returning_count;
}


size_t UniverseModifier::get_count_tbb(const tbb::concurrent_vector<bool>& input_vector, bool state) {
	size_t returning_count = 0;
	for (const bool &arrayElement : input_vector) {
		if (arrayElement == state)
			returning_count++;
	}
	return returning_count;
}

tbb::concurrent_vector<bool> UniverseModifier::get_neighborhood_tbb(size_t input_cell_x, size_t input_cell_y, size_t neighborhood_size, const tbb::concurrent_vector<bool>& universe, size_t size_x, size_t size_y) const {
	tbb::concurrent_vector<bool, tbb::cache_aligned_allocator<bool>> returning_neighbourhood;
	returning_neighbourhood.resize(neighborhood_size);

	size_t neighbourhood_side_size_y = static_cast<size_t>(sqrt(neighborhood_size));

	// CENTER row
	returning_neighbourhood[(neighborhood_size / 2)] = universe[get_vector_index(input_cell_x, input_cell_y, size_y)];
	if (input_cell_y > 0)
		returning_neighbourhood[get_vector_index(1, 0, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x, input_cell_y - 1, size_y)];
	if (input_cell_y < size_y - 1)
		returning_neighbourhood[get_vector_index(1, 2, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x, input_cell_y + 1, size_y)];

	// TOP row
	if (input_cell_x > 0) {
		returning_neighbourhood[get_vector_index(0, 1, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x - 1, input_cell_y, size_y)];
		if (input_cell_y > 0)
			returning_neighbourhood[get_vector_index(0, 0, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x - 1, input_cell_y - 1, size_y)];
		if (input_cell_y < size_y - 1)
			returning_neighbourhood[get_vector_index(0, 2, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x - 1, input_cell_y + 1, size_y)];
	}

	// BOTTOM row
	if (input_cell_x < size_x - 1) {
		returning_neighbourhood[get_vector_index(2, 1, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x + 1, input_cell_y, size_y)];
		if (input_cell_y > 0)
			returning_neighbourhood[get_vector_index(2, 0, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x + 1, input_cell_y - 1, size_y)];
		if (input_cell_y < size_y - 1)
			returning_neighbourhood[get_vector_index(2, 2, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x + 1, input_cell_y + 1, size_y)];
	}


	return returning_neighbourhood;
}

std::vector<bool> UniverseModifier::get_neighborhood(size_t input_cell_x, size_t input_cell_y, size_t neighborhood_size, const std::vector<bool>& universe, size_t size_x, size_t size_y) {
	std::vector<bool> returning_neighbourhood;
	returning_neighbourhood.resize(neighborhood_size);

	size_t neighbourhood_side_size_y = static_cast<size_t>(sqrt(neighborhood_size));

	// CENTER row
	returning_neighbourhood[(neighborhood_size / 2)] = universe[get_vector_index(input_cell_x, input_cell_y, size_y)];
	if (input_cell_y > 0)
		returning_neighbourhood[get_vector_index(1, 0, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x, input_cell_y - 1, size_y)];
	if (input_cell_y < size_y - 1)
		returning_neighbourhood[get_vector_index(1, 2, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x, input_cell_y + 1, size_y)];

	// TOP row
	if (input_cell_x > 0) {
		returning_neighbourhood[get_vector_index(0, 1, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x - 1, input_cell_y, size_y)];
		if (input_cell_y > 0)
			returning_neighbourhood[get_vector_index(0, 0, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x - 1, input_cell_y - 1, size_y)];
		if (input_cell_y < size_y - 1)
			returning_neighbourhood[get_vector_index(0, 2, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x - 1, input_cell_y + 1, size_y)];
	}

	// BOTTOM row
	if (input_cell_x < size_x - 1) {
		returning_neighbourhood[get_vector_index(2, 1, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x + 1, input_cell_y, size_y)];
		if (input_cell_y > 0)
			returning_neighbourhood[get_vector_index(2, 0, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x + 1, input_cell_y - 1, size_y)];
		if (input_cell_y < size_y - 1)
			returning_neighbourhood[get_vector_index(2, 2, neighbourhood_side_size_y)] = universe[get_vector_index(input_cell_x + 1, input_cell_y + 1, size_y)];
	}


	return returning_neighbourhood;
}


bool UniverseModifier::get_new_state_tbb(const tbb::concurrent_vector<bool>& input_current_neighbourhood) {

	bool current_state = input_current_neighbourhood[(input_current_neighbourhood.size() / 2)];
	bool returning_state = DEAD;
	size_t live_neighbours_count = get_count_tbb(input_current_neighbourhood, ALIVE);

	//Any live cell with fewer than two live neighbours dies, as if caused by under - population.
	//Any live cell with two or three live neighbours lives on to the next generation.
	//Any live cell with more than three live neighbours dies, as if by over - population.
	//-Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

	if (current_state == DEAD) {
		if (live_neighbours_count == REPRODUCTION_COUNT)
			returning_state = ALIVE;
	}
	else {
		--live_neighbours_count; // Self is ALIVE, so don't count it, only the neighbours
		if (live_neighbours_count <= OVERPOPULATION_COUNT && live_neighbours_count >= UNDERPOPULATION_COUNT)
			returning_state = ALIVE;
	}

	return returning_state;
}

bool UniverseModifier::get_new_state(const std::vector<bool>& input_current_neighbourhood) {
	
	bool current_state = input_current_neighbourhood[(input_current_neighbourhood.size() / 2)];
	bool returning_state = DEAD;
	size_t live_neighbours_count = get_count(input_current_neighbourhood, ALIVE);

	//Any live cell with fewer than two live neighbours dies, as if caused by under - population.
	//Any live cell with two or three live neighbours lives on to the next generation.
	//Any live cell with more than three live neighbours dies, as if by over - population.
	//-Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

	if (current_state == DEAD) {
		if (live_neighbours_count == REPRODUCTION_COUNT)
			returning_state = ALIVE;
	} else {
		--live_neighbours_count; // Self is ALIVE, so don't count it, only the neighbours
		if (live_neighbours_count <= OVERPOPULATION_COUNT && live_neighbours_count >= UNDERPOPULATION_COUNT)
			returning_state = ALIVE;
	}
	
	return returning_state;
}

bool UniverseModifier::are_equal(const std::vector<bool>& first_universe, const std::vector<bool>& second_universe) {
	bool are_equal = false;

	if (first_universe.size() > 0 && first_universe.size() == second_universe.size()) {
		for (size_t i = 0; i < first_universe.size(); ++i) {
			if (first_universe[i] != second_universe[i])
				break;
		are_equal = true;
	}
}
	return are_equal;
}

void UniverseModifier::advance_universe(std::vector<bool>& grid, size_t size_x, size_t size_y) {
	
	// TODO: Create & return a new universe, NOT edit the old

	for (size_t x = 0; x < size_x; ++x) {
		for (size_t y = 0; y < size_y; ++y) {
			grid[get_vector_index(x, y, size_y)] = get_new_state(get_neighborhood(x, y, DEFAULT_NEIGHBORHOOD_SIZE, grid, size_x, size_y));
		}
	}
}

void UniverseModifier::advance_universe_cell_tbb(size_t x, size_t y, tbb::concurrent_vector<bool>& grid, size_t size_x, size_t size_y) const {
	grid[get_vector_index(x, y, size_y)] = get_new_state_tbb(get_neighborhood_tbb(x, y, DEFAULT_NEIGHBORHOOD_SIZE, grid, size_x, size_y));
}


void UniverseModifier::advance_universe_tbb(tbb::concurrent_vector<bool>& grid, size_t size_x, size_t size_y) const {
	
	parallel_for(blocked_range2d<size_t, size_t>(0, size_x, 0, size_y),
		[&](const blocked_range2d<size_t, size_t>& r) {

			size_t StartX = r.rows().begin();
			size_t StopX = r.rows().end();
			size_t StartY = r.cols().begin();
			size_t StopY = r.cols().end();

			for (size_t x = StartX; x < StopX; ++x)
				for (size_t y = StartY; y < StopY; ++y)
					advance_universe_cell_tbb(x, y, grid, size_x, size_y);
		}
	);
}

tbb::concurrent_vector<bool> UniverseModifier::to_concurrent_vector(const std::vector<bool>& input_grid) {
	tbb::concurrent_vector<bool, tbb::cache_aligned_allocator<bool>> returning_concurrent_vector(input_grid.size());

	for (size_t i = 0; i < input_grid.size(); ++i)
		returning_concurrent_vector[i] = input_grid[i];

	return returning_concurrent_vector;
}


std::vector<bool> UniverseModifier::to_vector(const tbb::concurrent_vector<bool>& input_grid) {
	std::vector<bool> returning_vector(input_grid.size());

	for (size_t i = 0; i < input_grid.size(); ++i)
		returning_vector[i] = input_grid[i];

	return returning_vector;
}