#pragma once
#include <vector>
#include <random>

class UniverseModifier
{
public:
	UniverseModifier() {};
	void allocate_random_live_cells(size_t live_cells_count, std::vector<bool>& input_universe, size_t size_x, size_t size_y);
	static void advance_universe(std::vector<bool>& universe, size_t size_x, size_t size_y);
	static void debug_show_universe(const std::vector<bool>& universe, size_t size_x, size_t size_y);
	static size_t get_vector_index(size_t x, size_t y, size_t cell_count);
	static size_t get_count(const std::vector<bool>& inputArray, bool element);
	static std::vector<bool> get_neighborhood(size_t input_cell_x, size_t input_cell_y, size_t neighborhood_size, const std::vector<bool>& universe_, size_t size_x, size_t size_y);
	static bool get_new_state(const std::vector<bool>& input_current_neighbourhood);
	static bool are_equal(const std::vector<bool>& first_universe, const std::vector<bool>& second_universe);
private:
	std::random_device random_device_;
};

