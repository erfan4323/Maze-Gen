//std includes
#include <stack>
#include <random>
#include <vector>

//renderer
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;

class Maze : public olc::PixelGameEngine
{
public:
	Maze()
	{
		sAppName = "Maze V1.0";
	}

private:
	static const int _mazeWidth = 40;
	static const int _mazeHeight = 25;

	vector<int> _maze;

	enum
	{
		CELL_N = 0x01,
		CELL_E = 0x02,
		CELL_S = 0x04,
		CELL_W = 0x08,
		CELL_VISITED = 0x10,
	};

	int _visitedCells;

	stack<pair<int, int>> _stack; // it will stores the path we went in a (x, y) pair

	int _pathWidth;

	//random set
	std::random_device rd;
	std::mt19937 rng;

public:
	vector<int> GetMaze() 
	{
		return _maze;
	}

	// Called once at the start, so create we things here
	bool OnUserCreate() override
	{
		_maze = std::vector<int>(_mazeWidth * _mazeHeight, 0);

		rng = std::mt19937(rd()); // Seed the random number generator

		int x = rng() % _mazeWidth;
		int y = rng() % _mazeHeight;

		_stack.push(make_pair(x, y));

		_maze[y * _mazeWidth + x] = CELL_VISITED;

		_visitedCells = 1;

		_pathWidth = 3;

		return true;
	}

	// called once per frame
	bool OnUserUpdate(float fElapsedTime) override
	{
		this_thread::sleep_for(10ms);

		auto offset = [&](int x, int y)
		{
			return (_stack.top().second + y) * _mazeWidth + (_stack.top().first + x);
		};

		//--------------------------------|Updating Maze|--------------------------------//

		Generate(offset);

		//--------------------------------|Rendering Maze|--------------------------------//

		//Clear screen
		Clear(olc::VERY_DARK_GREY);

		Render();

		return true;
	}

private:
	void Generate(std::function<int(int, int)> offset)
	{
		//Maze Algorithm
		if (_visitedCells < _mazeWidth * _mazeHeight)
		{
			//create a set of unvisited neighbours

			vector<int> neighbours;

			//north neighbour
			if (_stack.top().second > 0 && (_maze[offset(0, -1)] & CELL_VISITED) == 0)
				neighbours.push_back(0);

			//east neighbour
			if (_stack.top().first < _mazeWidth - 1 && (_maze[offset(1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(1);

			//south neighbour
			if (_stack.top().second < _mazeHeight - 1 && (_maze[offset(0, 1)] & CELL_VISITED) == 0)
				neighbours.push_back(2);

			//west neighbour
			if (_stack.top().first > 0 && (_maze[offset(-1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(3);

			//we check if there are any neighbours available
			if (!neighbours.empty())
			{
				//choose a random neighbour
				int nextCellDir = neighbours[rng() % neighbours.size()];

				//create path from current cell to the neighbour cell
				switch (nextCellDir)
				{
				case 0: //north
					_maze[offset(0, -1)] |= CELL_VISITED | CELL_S;
					_maze[offset(0, 0)] |= CELL_N;
					_stack.push(make_pair((_stack.top().first + 0), (_stack.top().second - 1)));
					break;
				case 1: //east
					_maze[offset(+1, 0)] |= CELL_VISITED | CELL_W;
					_maze[offset(0, 0)] |= CELL_E;
					_stack.push(make_pair((_stack.top().first + 1), (_stack.top().second + 0)));
					break;
				case 2: //south
					_maze[offset(0, +1)] |= CELL_VISITED | CELL_N;
					_maze[offset(0, 0)] |= CELL_S;
					_stack.push(make_pair((_stack.top().first + 0), (_stack.top().second + 1)));
					break;
				case 3: // west
					_maze[offset(-1, 0)] |= CELL_VISITED | CELL_E;
					_maze[offset(0, 0)] |= CELL_W;
					_stack.push(make_pair((_stack.top().first - 1), (_stack.top().second + 0)));
					break;
				default:
					break;
				}

				//new cell
				_maze[offset(0, 0)] |= CELL_VISITED;
				_visitedCells++;
			}
			else
			{
				_stack.pop(); //Backtrack
			}
		}
	}
	
	void Render()
	{
		//Draw Maze
		for (int x = 0; x < _mazeWidth; x++)
		{
			for (int y = 0; y < _mazeHeight; y++)
			{
				olc::Pixel cellColor = (_maze[y * _mazeWidth + x] & CELL_VISITED) ? olc::DARK_CYAN : olc::DARK_GREEN;

				// Each cell is inflated by _pathWidth, so fill it in
				for (int px = 0; px < _pathWidth; px++)
					for (int py = 0; py < _pathWidth; py++)
						Draw(x * (_pathWidth + 1) + px, y * (_pathWidth + 1) + py, cellColor); // Draw Cell

				// Draw passageways between cells
				for (int p = 0; p < _pathWidth; p++)
				{
					if (_maze[y * _mazeWidth + x] & CELL_S)
						Draw(x * (_pathWidth + 1) + p, y * (_pathWidth + 1) + _pathWidth, olc::Pixel(olc::DARK_CYAN));

					if (_maze[y * _mazeWidth + x] & CELL_E)
						Draw(x * (_pathWidth + 1) + _pathWidth, y * (_pathWidth + 1) + p, olc::Pixel(olc::DARK_CYAN));
				}
			}
		}

		for (int py = 0; py < _pathWidth; py++)
			for (int px = 0; px < _pathWidth; px++)
				Draw(_stack.top().first * (_pathWidth + 1) + px, _stack.top().second * (_pathWidth + 1) + py, olc::Pixel(olc::GREEN)); // Draw Cell
	}
};

int main()
{

	Maze maze;

	if (maze.Construct(160, 100, 6, 6))
		maze.Start();

	return 0;
}