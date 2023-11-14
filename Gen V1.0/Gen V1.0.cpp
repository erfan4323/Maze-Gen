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
		_visitedCells = 1;

		return true;
	}

	// called once per frame
	bool OnUserUpdate(float fElapsedTime) override
	{

		// Rendering ->

		//Clear screen
		Clear(olc::VERY_DARK_RED);

		//Draw Maze
		for (int x = 0; x < _mazeWidth; x++)
		{
			for (int y = 0; y < _mazeHeight; y++) 
			{
				if (_maze[y * _mazeWidth + x] && CELL_VISITED)
					Draw(x, y, olc::Pixel(olc::WHITE));
				else
					Draw(x, y, olc::Pixel(olc::BLUE));
			}
		}

		return true;
	}
};

int main()
{
	Maze maze;

	if (maze.Construct(160, 100, 8, 8))
		maze.Start();

	return 0;
}
