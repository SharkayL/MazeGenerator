#pragma once
#include "Scene.h"
#include <vector>
#include <array>

enum directions {
	none,
	left,
	right,
	up,
	down
};

struct info {
	int x;
	int y;
	directions dir;
};

struct line {
	float x1;
	float y1;
	float x2;
	float y2;
};

class Maze: public Scene
{
public:
	Maze(int row, int col);
	~Maze();
	int Init();
	//void HandleEvents() override;
	void Update() override;
	void Render(SDL_Renderer *renderer) override;
	void HuntAndKill();
	unsigned int row;
	unsigned int col;
	std::vector<int> unwalked;
private:
	SDL_Rect border;
	//SDL_Point startPos;
	SDL_Point currentPos;
	void GetStartPos();
	void ScanGrid();
	std::vector<info> walked;
	int lastWalkedSize = 0;
	bool deadEnd;
	directions dir;
	int ind;
	std::vector<directions> PossibleDirections();
	void RemoveWall(int x,int y,directions dir);
	bool FindDirection();
	directions ProbeDirection(SDL_Point point);
	bool Hunting();
	void DrawWalls();
	void DrawArrow(SDL_Renderer *renderer,int x,int y,directions dir);
	SDL_Point NewPos(SDL_Point currentGrid, int direction);
	void UpdateGridInfo();
	bool ValidityCheck();
	std::vector<SDL_Point> connections;
	std::vector<bool> horWalls;
	std::vector<bool> verWalls;
	bool IsEmpty(int index);
	std::array<int, 2> wallDirs;
	void UpdateHorWalls(int i, int j);
	void UpdateVerWalls(int i, int j);
	std::vector<line> walls;
	float gridSize;
	std::vector<SDL_Point> verConns;
	SDL_Surface *s;
	SDL_Rect *passenger;
	SDL_PixelFormat *fmt;
	SDL_Point IndToPos(unsigned int index);
};

