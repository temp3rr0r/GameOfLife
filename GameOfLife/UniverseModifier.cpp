#include "UniverseModifier.h"
#include <iostream>
#include "Settings.h"

void UniverseModifier::allocate_random_live_cells(size_t live_cells_count, std::vector<bool>& grid, size_t size_x, size_t size_y) {
	if (live_cells_count > 0) {
		std::mt19937 mersenne_twister_engine(random_device_());
		std::uniform_int_distribution<> cell_distribution(0, size_x * size_y);

		for (size_t i = 0; i < live_cells_count; ++i)
			grid[cell_distribution(mersenne_twister_engine)] = true;
	}
}

void UniverseModifier::debug_show_universe(const std::vector<bool>& grid, size_t size_x, size_t size_y) {

	std::cout << "Universe:" << std::endl;

	for (size_t x = 0; x < size_x; ++x) {
		for (size_t y = 0; y < size_y; ++y) {
			std::cout << static_cast<int>(grid[x * y + y]) << "\t";
		}
		std::cout << std::endl;
	}
}

size_t UniverseModifier::get_vector_index(size_t x, size_t y, size_t cell_count) {
	return (x * cell_count) + y;
}

size_t UniverseModifier::get_count(const std::vector<bool>& inputArray, bool element) {
	size_t returning_count = 0;
	for (const bool &arrayElement : inputArray) {
		if (arrayElement == element)
			returning_count++;
	}
	return returning_count;
}

std::vector<bool> UniverseModifier::get_neighborhood(size_t input_cell_x, size_t input_cell_y, size_t neighborhood_size, const std::vector<bool>& universe_, size_t size_x, size_t size_y) const {
	std::vector<bool> returning_neighbourhood;
	returning_neighbourhood.resize(neighborhood_size);

	size_t neighborhood_side_size = sqrt(neighborhood_size);
	size_t universe_size = universe_.size();

	// CENTER row
	returning_neighbourhood[static_cast<int32_t>(neighborhood_size / 2)] = universe_[get_vector_index(input_cell_x, input_cell_y, universe_size)];
	if (input_cell_y > 0)
		returning_neighbourhood[get_vector_index(1, 0, neighborhood_side_size)] = universe_[get_vector_index(input_cell_x, input_cell_y - 1, universe_size)];
	if (input_cell_y < size_y - 1)
		returning_neighbourhood[get_vector_index(1, 2, neighborhood_side_size)] = universe_[get_vector_index(input_cell_x, input_cell_y + 1, universe_size)];

	// TOP row
	if (input_cell_x > 0) {
		returning_neighbourhood[get_vector_index(0, 1, neighborhood_side_size)] = universe_[get_vector_index(input_cell_x - 1, input_cell_y, universe_size)];
		if (input_cell_y > 0)
			returning_neighbourhood[get_vector_index(0, 0, neighborhood_side_size)] = universe_[get_vector_index(input_cell_x - 1, input_cell_y - 1, universe_size)];
		if (input_cell_y < size_y - 1)
			returning_neighbourhood[get_vector_index(0, 2, neighborhood_side_size)] = universe_[get_vector_index(input_cell_x - 1, input_cell_y + 1, universe_size)];
	}

	// BOTTOM row
	if (input_cell_x < size_x - 1) {
		returning_neighbourhood[get_vector_index(2, 1, neighborhood_side_size)] = universe_[get_vector_index(input_cell_x + 1, input_cell_y, universe_size)];
		if (input_cell_y > 0)
			returning_neighbourhood[get_vector_index(2, 0, neighborhood_side_size)] = universe_[get_vector_index(input_cell_x + 1, input_cell_y - 1, universe_size)];
		if (input_cell_y < size_y - 1)
			returning_neighbourhood[get_vector_index(2, 2, neighborhood_side_size)] = universe_[get_vector_index(input_cell_x + 1, input_cell_y + 1, universe_size)];
	}


	return returning_neighbourhood;
}

bool UniverseModifier::get_new_state(const std::vector<bool>& input_current_neighbourhood) {
	
	bool current_state = input_current_neighbourhood[static_cast<int32_t>(input_current_neighbourhood.size() / 2)];
	bool returning_state = DEAD;
	size_t live_neighbours_count = get_count(input_current_neighbourhood, true);

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

void UniverseModifier::advance_universe(std::vector<bool>& grid, size_t size_x, size_t size_y) const {
	size_t universe_size = grid.size();

	for (size_t x = 0; x < size_x; ++x) {
		for (size_t y = 0; y < size_y; ++y) {
			grid[get_vector_index(x, y, universe_size)] = get_new_state(get_neighborhood(x, y, DEFAULT_NEIGHBORHOOD_SIZE, grid, size_x, size_y));
		}
		std::cout << std::endl;
	}
}
