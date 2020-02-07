#include "Maze.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

Maze::Maze(int row, int col)
{
	this->row = row;
	this->col = col;
	Init();
}

Maze::~Maze()
{
}

int Maze::Init() {
	srand(time(NULL));
	border.h = 600;
	border.w = 600;
	border.x = 100;
	border.y = 100;
	if (col >= row) {
		gridSize = border.w / col;
	} else {
		gridSize = border.w / row;
	}
	unwalked.reserve(row*col);
	for (int i = 0; i < row*col; ++i) {
		unwalked.push_back(i);
	}
	horWalls.reserve(row*col);
	horWalls.assign(row * col, true);
	verWalls.reserve(row*col);
	verWalls.assign(row * col, true);
	deadEnd = false;
	passenger = new SDL_Rect;
	s = SDL_CreateRGBSurface(0, gridSize, gridSize, 4, 255, 255, 0, 255);
	passenger->h = gridSize;
	passenger->w = gridSize;
	fmt = s->format;
	return 0;
}

void Maze::GetStartPos() {
	dir = none;
	if (Hunting()) {
		RemoveWall(currentPos.x, currentPos.y, dir);
	}
	UpdateGridInfo();
}

void Maze::Update() {

		HuntAndKill();
		std::cout << currentPos.x << "; " << currentPos.y << "; " << dir << "; " << deadEnd << std::endl;

}

void Maze::Render(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &border);

	int indexVer = 0;
	for (auto ite = verWalls.begin(); ite != verWalls.end();ite++) {
		auto wall = *ite;
		if (!wall) {
			indexVer++;
			continue;
		}
		int x = indexVer % col;
		int y = indexVer / col;
		int topX = 100 + (x+1) * gridSize;
		int topY = 100 + y * gridSize;
		SDL_RenderDrawLine(renderer,topX,topY,topX,topY + gridSize);
		indexVer++;
	}
	int indexHor = 0;
	for (auto ite = horWalls.begin(); ite != horWalls.end(); ite++) {
		auto wall = *ite;
		if (!wall) {
			indexHor++;
			continue;
		}
		int x = indexHor % col;
		int y = indexHor / col;
		int topX = 100 + x * gridSize;
		int topY = 100 + (y+1) * gridSize;
		SDL_RenderDrawLine(renderer, topX, topY, topX + gridSize, topY);
		indexHor++;
	}
	passenger->x = currentPos.x * gridSize + 100;
	passenger->y = currentPos.y * gridSize + 100;
	SDL_FillRect(s, passenger, SDL_MapRGB(fmt, 255, 0, 0));
	SDL_RenderFillRect(renderer, passenger);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (auto ite = walked.begin(); ite != walked.end(); ++ite) {
		auto info = *ite;
		DrawArrow(renderer, info.x, info.y, info.dir);
	}
}

void Maze::DrawArrow(SDL_Renderer *renderer,int x, int y, directions dir) {
	int arrowSize = gridSize/8;
	float centerX = x * gridSize + 100 + gridSize/2;
	float centerY = y * gridSize + 100 + gridSize/2;
	if (dir == right) {
		centerX += arrowSize * 2;
		SDL_RenderDrawLine(renderer,centerX+ arrowSize,centerY,centerX- arrowSize,centerY+ arrowSize);
		SDL_RenderDrawLine(renderer, centerX+ arrowSize, centerY, centerX - arrowSize, centerY - arrowSize);
	}
	else if (dir == left) {
		centerX -= arrowSize * 2;
		SDL_RenderDrawLine(renderer, centerX - arrowSize, centerY, centerX + arrowSize, centerY + arrowSize);
		SDL_RenderDrawLine(renderer, centerX - arrowSize, centerY, centerX + arrowSize, centerY - arrowSize);
	}
	else if (dir == up) {
		centerY -= arrowSize * 2;
		SDL_RenderDrawLine(renderer, centerX, centerY- arrowSize, centerX + arrowSize, centerY + arrowSize);
		SDL_RenderDrawLine(renderer, centerX, centerY- arrowSize, centerX - arrowSize, centerY + arrowSize);
	}
	else if (dir == down) {
		centerY += arrowSize * 2;
		SDL_RenderDrawLine(renderer, centerX, centerY + arrowSize, centerX + arrowSize, centerY - arrowSize);
		SDL_RenderDrawLine(renderer, centerX, centerY + arrowSize, centerX - arrowSize, centerY - arrowSize);
	}
}

void Maze::HuntAndKill() {
	if (unwalked.size() > 0) {
		ScanGrid();
		UpdateGridInfo();
	}
}

void Maze::ScanGrid() {
	if (unwalked.size() == row * col && deadEnd == false) {
		currentPos.x = rand() % col;
		currentPos.y = rand() % row;
		FindDirection();
		UpdateGridInfo();
		RemoveWall(currentPos.x, currentPos.y, dir);
		return;
	}
	if (deadEnd) {
		//hunting
		deadEnd = false;
		GetStartPos();
	}
	currentPos = NewPos(currentPos, dir);
	if (FindDirection()) {
		RemoveWall(currentPos.x, currentPos.y, dir);
	}
}

std::vector<directions> Maze::PossibleDirections() {
	std::vector<directions> dirs;
	dirs.reserve(4);
	if (currentPos.x > 0 && dir != right) {
		dirs.push_back(left);
	}
	if (currentPos.x < col - 1 && dir != left) {
		dirs.push_back(right);
	}
	if (currentPos.y > 0 && dir != down) {
		dirs.push_back(up);
	}
	if (currentPos.y < row - 1 && dir != 3) {
		dirs.push_back(down);
	}
	return dirs;
}

directions Maze::ProbeDirection(SDL_Point point) {
	auto dirs = PossibleDirections();
	if (walked.size() > 0) {
		for (auto ite = dirs.begin(); ite != dirs.end();) {
			auto possibleDir = *ite;
			bool deleted = false;
			SDL_Point possiblePos = NewPos(point, possibleDir);

			for (auto walkedIte = walked.begin(); walkedIte != walked.end(); ++walkedIte) {
				auto walkedPos = *walkedIte;
				if (possiblePos.x == walkedPos.x && possiblePos.y == walkedPos.y) {
					deleted = true;
					ite = dirs.erase(ite);
					break;
				}
			}
			if (!deleted) {
				++ite;
			}
		}
	}
	if (dirs.size() > 0) {
		dir = dirs[rand() % dirs.size()];
		return dir;
	}
	return none;
}

bool Maze::FindDirection() {
	auto dirs = PossibleDirections();
	if (walked.size() > 0) {
		for (auto ite = dirs.begin(); ite != dirs.end();) {
			auto possibleDir = *ite;
			bool deleted = false;
			SDL_Point possiblePos = NewPos(currentPos, possibleDir);

			for (auto walkedIte = walked.begin(); walkedIte != walked.end(); ++walkedIte) {
				auto walkedPos = *walkedIte;
				if (possiblePos.x == walkedPos.x && possiblePos.y == walkedPos.y) {
					deleted = true;
					ite = dirs.erase(ite);
					break;
				}
			}
			if (!deleted) {
				++ite;
			}
		}
	}
	if (dirs.size() > 0) {
		dir = dirs[rand() % dirs.size()];
		deadEnd = false;
		return true;
	}
	else {
		deadEnd = true;
		return false;
	}
}

void Maze::RemoveWall(int x, int y, directions dir) {
	if (dir == left) {
		if (x > 0) {
			int index = x + y * col - 1;
			verWalls[index] = false;
			return;
		}
	}
	if (dir == right) {
		if ((x - 1) < (int)col) {
			int index = x + y * col;
			verWalls[index] = false;
			return;
		}
	}
	if (dir == up) {
		if (y > 0) {
			int index = x + (y-1) * col;
			horWalls[index] = false;
			return;
		}
	}
	if (dir == down) {
		if ((y - 1) < (int)row) {
			int index = x + y * col;
			horWalls[index] = false;
			return;
		}
	}
}

bool Maze::Hunting() {
	auto startPoint = walked.begin() + rand() % walked.size();
	{
		auto forwardPoint = startPoint;
		for (; forwardPoint != walked.end(); forwardPoint++) {
			auto entry = *forwardPoint;
			currentPos = { entry.x,entry.y };
			if (FindDirection()) {
				return true;
			}
		}
	}
	{
		auto backwardPoint = startPoint;
		for (; backwardPoint != walked.begin(); backwardPoint--) {
			auto entry = *backwardPoint;
			currentPos = { entry.x,entry.y };
			if (FindDirection()) {
				return true;
			}
		}
	}
	return false;
}

bool Maze::IsEmpty(int index) {
	
	for (auto ite = unwalked.begin(); ite != unwalked.end();++ite) {
		auto entry = *ite;
		if (entry == index) {
			return true;
		}
	}
	return false;
}

SDL_Point Maze::NewPos(SDL_Point currentGrid, int direction) {
	if (direction == left) {
		currentGrid.x -= 1;
	}
	else if (direction == right) {
		currentGrid.x += 1;
	}
	else if (direction == up) {
		currentGrid.y -= 1;
	}
	else if (direction == down) {
		currentGrid.y += 1;
	}
	return currentGrid;
}

bool Maze::ValidityCheck() {
//	for (auto ite = walked.begin(); ite != walked.end(); ++ite) {
//		auto walkedGrid = *ite;
//		bool repeated = false;
//		if (walkedGrid.x == currentPos.x && walkedGrid.y == currentPos.y) {
//			if (joints.size() > 0) {
//				for (auto jointIte = joints.begin(); jointIte != joints.end(); ++jointIte) {
//					auto jointGrid = *jointIte;
//					if (walkedGrid.x == jointGrid.x && walkedGrid.y == jointGrid.y && walkedGrid.dir == jointGrid.dir) {
//						repeated = true;
//						break;
//					}
//				}
//			}
//			if (repeated == false && dir != 0) {
//				joints.push_back({ currentPos.x, currentPos.y, dir });
//			}
//			return false;
//		}
//	}
//	if (currentPos.x >= 0 && currentPos.y >= 0 && currentPos.x <= col - 1 && currentPos.y <= row - 1) {
//		return true;
//	}
//	else 
	return false;
}

void Maze::UpdateGridInfo() {
	//if (ValidityCheck()) {
		walked.push_back({ currentPos.x, currentPos.y, dir });
		ind = col * currentPos.y + currentPos.x;
		auto ite = std::find(unwalked.begin(), unwalked.end(), ind);
		if (ite != unwalked.end()) {
			unwalked.erase(ite);
		}
	//}
	//else {
	//	FindDirection();
	//}
}

void Maze::DrawWalls() {
	int indexHor = 0;
	int indexVer = 0;
	for (auto horIte = horWalls.begin(); horIte != horWalls.end(); ++horIte) {
		auto horWall = *horIte;
		if (horWall) {
			int x = indexHor % col;
			int y = indexHor / col;
			walls.push_back({ 100 + x * gridSize, 100 + (y + 1)* gridSize, 100 + (x + 1) * gridSize, 100 + (y + 1)* gridSize});
		}
		++indexHor;
	}
	for (auto verIte = verWalls.begin(); verIte != verWalls.end(); ++verIte) {
		auto verWall = *verIte;
		if (verWall) {
			int x = indexVer % col;
			int y = indexVer / col;
			walls.push_back({100 + (x+1) * gridSize, 100 + y * gridSize, 100 + (x + 1) * gridSize, 100 + (y +1) * gridSize, });
		}
		++indexVer;
	}
}

void Maze::UpdateHorWalls(int i, int j) {
	for (auto walkedIte = walked.begin(); walkedIte != walked.end(); ++walkedIte)
	{
		auto firstGrid = *walkedIte;
		if (firstGrid.x == i && firstGrid.y == j) {
			wallDirs[0] = firstGrid.dir;
			for (auto secondIte = walked.begin(); walkedIte != walked.end(); ++secondIte)
			{
				auto secondGrid = *secondIte;
				if (secondGrid.x == i && secondGrid.y == j + 1) {
					wallDirs[1] = secondGrid.dir;
					break;
				}
			}
			break;
		}
	}
	if ((wallDirs[0] == 4 && (wallDirs[1] == 1 || wallDirs[1] == 2 || wallDirs[1] == 4)) ||
		(wallDirs[1] == 3 && (wallDirs[0] == 1 || wallDirs[0] == 2 || wallDirs[0] == 3))) {
		horWalls[col*j + i] = false;
	}
}

void Maze::UpdateVerWalls(int i, int j) {
	for (auto walkedIte = walked.begin(); walkedIte != walked.end(); ++walkedIte)
	{
		auto firstGrid = *walkedIte;
		if (firstGrid.x == i && firstGrid.y == j) {
			wallDirs[0] = firstGrid.dir;
			for (auto secondIte = walked.begin(); walkedIte != walked.end(); ++secondIte)
			{
				auto secondGrid = *secondIte;
				if (secondGrid.x == i+1 && secondGrid.y == j) {
					wallDirs[1] = secondGrid.dir;
					break;
				}
			}
			break;
		}
	}
	if ((wallDirs[0] == 2 && (wallDirs[1] == 2 || wallDirs[1] == 3 || wallDirs[1] == 4)) ||
		(wallDirs[1] == 1 && (wallDirs[0] == 1 || wallDirs[0] == 3 || wallDirs[0] == 4))) {
		verWalls[col*j + i] = false;
	}
}

SDL_Point Maze::IndToPos(unsigned int index) {
	int x = index % col;
	int y = index / col;
	return { x, y };
}