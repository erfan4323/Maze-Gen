//std includes
#include <iostream>
#include <stack>

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
	int _mazeWidth;
	int _mazeHeight;
	int* _maze;

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

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		//Maze Parameters
		_mazeHeight = 25;
		_mazeWidth = 40;

		_maze = new int[_mazeHeight * _mazeWidth];
		memset(_maze, 0x00, _mazeWidth * _mazeHeight * sizeof(int));

		_stack.push(make_pair(0, 0));
		_maze[0] = CELL_VISITED;
		int y = rand() % _mazeHeight;
		_stack.push(make_pair(x, y));
		_maze[y * _mazeWidth + x] = CELL_VISITED;
		_visitedCells = 1;

		_pathWidth = 3;

		return true;
	}

	// called once per frame
	bool OnUserUpdate(float fElapsedTime) override
	{
		this_thread::sleep_for(20ms);

		auto offset = [&](int x, int y)
		{
			return (_stack.top().second + y) * _mazeWidth + (_stack.top().first + x);
		};

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
				int nextCellDir = neighbours[rand() % neighbours.size()];

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

		// Rendering Maze =>

		//Clear screen
		Clear(olc::BLACK);

		//Draw Maze
		for (int x = 0; x < _mazeWidth; x++)
		{
			for (int y = 0; y < _mazeHeight; y++)
			{
				// Each cell is inflated by _pathWidth, so fill it in
				for (int px = 0; px < _pathWidth; px++)
				{
					for (int py = 0; py < _pathWidth; py++)
					{
						if (_maze[y * _mazeWidth + x] & CELL_VISITED)
							Draw(x * (_pathWidth + 1) + px, y * (_pathWidth + 1) + py, olc::Pixel(olc::WHITE)); // Draw Cell
						else
							Draw(x * (_pathWidth + 1) + px, y * (_pathWidth + 1) + py, olc::Pixel(olc::DARK_YELLOW)); // Draw Cell
					}
				}

				// Draw passageways between cells
				for (int p = 0; p < _pathWidth; p++)
				{
					if (_maze[y * _mazeWidth + x] & CELL_S)
						Draw(x * (_pathWidth + 1) + p, y * (_pathWidth + 1) + _pathWidth);

					if (_maze[y * _mazeWidth + x] & CELL_E)
						Draw(x * (_pathWidth + 1) + _pathWidth, y * (_pathWidth + 1) + p);
				}
			}
		}

		for (int py = 0; py < _pathWidth; py++)
			for (int px = 0; px < _pathWidth; px++)
				Draw(_stack.top().first * (_pathWidth + 1) + px, _stack.top().second * (_pathWidth + 1) + py, olc::Pixel(olc::GREEN)); // Draw Cell

		return true;
	}
};

int main()
{
	srand(clock());

	Maze maze;

	if (maze.Construct(160, 100, 8, 8))
		maze.Start();

	return 0;
}