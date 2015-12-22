#pragma once
#include <vector>
#include <random>
#include <tbb/concurrent_vector.h>

using namespace std;
using namespace tbb;

class UniverseModifier
{
public:
	UniverseModifier() {};
	void allocate_random_live_cells(size_t live_cells_count, vector<bool>& input_universe, size_t size_x, size_t size_y);
	void universe_to_png(const vector<bool>& universe, size_t universe_size_x, size_t universe_size_y, const char* filename) const;	
	static concurrent_vector<bool> to_concurrent_vector(const vector<bool>& input_grid);
	static vector<bool> to_vector(const concurrent_vector<bool>& input_grid);
	static void debug_show_universe(const vector<bool>& universe, size_t size_x, size_t size_y);
	static size_t get_vector_index(size_t x, size_t y, size_t cell_count);
	static size_t get_count(const vector<bool>& inputArray, bool element);
	static size_t get_count_tbb(const concurrent_vector<bool>& input_vector, bool state);
	concurrent_vector<bool> get_neighborhood_tbb(size_t input_cell_x, size_t input_cell_y, size_t neighborhood_size, const concurrent_vector<bool>& universe, size_t size_x, size_t size_y) const;
	static vector<bool> get_neighborhood(size_t input_cell_x, size_t input_cell_y, size_t neighborhood_size, const vector<bool>& universe_, size_t size_x, size_t size_y);
	static bool get_new_state_tbb(const concurrent_vector<bool>& input_current_neighbourhood);
	static bool get_new_state(const vector<bool>& input_current_neighbourhood);
	static bool are_equal(const vector<bool>& first_universe, const vector<bool>& second_universe);
private:
	random_device random_device_;
};

