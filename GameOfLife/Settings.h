#pragma once

static const bool VERBOSE = false;
static const bool SAVE_PNG = true;

static const bool SAVE_ALL_PNG_STEPS = true;

static const int DEFAULT_NUMBER_OF_THREADS = 4;

static const int UNIVERSE_X_COUNT = 10;
static const int UNIVERSE_Y_COUNT = 10;
static const int DEFAULT_TOTAL_TIME_STEPS = 10;

static const int DEFAULT_INIT_LIVE_CELLS = 10;
static const double DEFAULT_INIT_LIVE_CELLS_PROPORTION = 0.8;
static const int DEFAULT_NEIGHBORHOOD_SIZE = 9;

//Any live cell with fewer than two live neighbours dies, as if caused by under - population.
//Any live cell with two or three live neighbours lives on to the next generation.
//Any live cell with more than three live neighbours dies, as if by over - population.
//Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
static const int REPRODUCTION_COUNT = 3;
static const int UNDERPOPULATION_COUNT = 2;
static const int OVERPOPULATION_COUNT = 3;

static const bool DEAD = false;
static const bool ALIVE = true;