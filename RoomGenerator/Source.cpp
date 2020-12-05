#include <SDL.h>
#include <SDL_image.h>

#define ERROR_LOGGING
#include "SDLG.h"

using namespace SDLG;

int main(int argc, char* argv[]) { return StartSDL(); }

enum class Wall {
	empty,
	wall,
	door
};

Wall* horizontalWalls;
Wall* verticalWalls;
int* cells;
struct room {
	int x;
	int y;
	int w;
	int h;
	//int network;
	int growths = 0;
	union {
		bool locks[4];
		struct {
			bool lLock;
			bool rLock;
			bool uLock;
			bool dLock;
		};
	};
	int numLocks = 0;
};
std::vector<room> rooms;

int roomCount = 4000;

int maxGrowths = 20;

int minSize = 15;

int w = 1000;
int h = 1000;

int cellSize = 1;
int cellmargin = 0;
int wallSize = 5;

int connected = 1;

void SDLG::OnStart() {
	srand(time(0));
	
	horizontalWalls = new Wall[w * (h + 1)];
	verticalWalls = new Wall[(w + 1) * h];
	cells = new int[w * h];
	for (int i = 0; i < w * h; i++) {
		cells[i] = -1;
	}
	int tmpMin = minSize;
	for (int i = 0; i < roomCount; i++) {
		int seedx, seedy;
		if (i == 0) {
			//seedx = rand() % w;
			//seedy = rand() % h;
			seedx = w / 2;
			seedy = h / 2;
		}
		else {
			do {
				seedx = rand() % w;
				seedy = rand() % h;
			} while (
				!(
					cells[seedy * h + seedx] == -1 && (
						(seedx + 1  < w && cells[seedy * h + (seedx + 1)] != -1) ||
						(seedy + 1  < h && cells[(seedy + 1) * h + seedx] != -1) ||
						(seedx - 1 >= 0 && cells[seedy * h + (seedx - 1)] != -1) ||
						(seedy - 1 >= 0 && cells[(seedy - 1) * h + seedx] != -1)
					)
				)
			);
		}
		room r = { seedx,seedy,1,1 };

		while (r.growths < maxGrowths && r.numLocks != 4) {
			room bachelor = r;
			int direction = rand() % 4;
			bool valid = true;
			int x0, y0, x1, y1;
			switch (direction) {
			case 0: // Left
				if (bachelor.lLock) {
					valid = false;
					continue;
				}
				bachelor.x -= 1;
				bachelor.w += 1;
				x0 = bachelor.x;
				y0 = bachelor.y;
				x1 = bachelor.x + 1;
				y1 = bachelor.y + bachelor.h;
				break;
			case 1: // Right
				if (bachelor.rLock) {
					valid = false;
					continue;
				}
				bachelor.w += 1;
				x0 = bachelor.x + bachelor.w - 1;
				y0 = bachelor.y;
				x1 = bachelor.x + bachelor.w;
				y1 = bachelor.y + bachelor.h;
				break;
			case 2: // Up
				if (bachelor.uLock) {
					valid = false;
					continue;
				}
				bachelor.y -= 1;
				bachelor.h += 1;
				x0 = bachelor.x;
				y0 = bachelor.y;
				x1 = bachelor.x + bachelor.w;
				y1 = bachelor.y + 1;
				break;
			case 3: // Down
				if (bachelor.dLock) {
					valid = false;
					continue;
				}
				bachelor.h += 1;
				x0 = bachelor.x;
				y0 = bachelor.y + bachelor.h - 1;
				x1 = bachelor.x + bachelor.w;
				y1 = bachelor.y + bachelor.h;
				break;
			}


			if ( bachelor.x < 0 || bachelor.y < 0 || bachelor.x + bachelor.w > w || bachelor.y + bachelor.h > h) {
				r.numLocks++;
				r.locks[direction] = true;
				continue;
			}

			for (int y = y0; y < y1; y++)
				for (int x = x0; x < x1; x++)
					if (cells[y * w + x] != -1) {
						valid = false;
						goto escape;
					}

			escape:

			if (!valid) {
				r.numLocks++;
				r.locks[direction] = true;
				continue;
			}

			bachelor.growths++;
			r = bachelor;
		}

		if (r.w >= tmpMin && r.h >= tmpMin) {

			if (seedx + 1 < w && cells[seedy * h + (seedx + 1)] != -1) {

			}
			else if (seedy + 1 < h && cells[(seedy + 1) * h + seedx] != -1) {

			}
			else if (seedx - 1 >= 0 && cells[seedy * h + (seedx - 1)] != -1) {

			}
			else if (seedy - 1 >= 0 && cells[(seedy - 1) * h + seedx] != -1) {

			}

			for (int y = r.y; y < r.y + r.h; y++)
				for (int x = r.x; x < r.x + r.w; x++)
					cells[y * w + x] = i;
			rooms.push_back(r);
			tmpMin = minSize;
		}
		else {
			tmpMin--;
			i--;
		}
	}

	/*for (int i = 0; i < roomCount; i++) {
		int x, y;
		
		do {
			x = rand() % w;
			y = rand() % h;
		} while (cells[y * w + x] != -1);
		cells[y * w + x] = i;
		rooms.push_back({x,y,1,1,i});
	}
	while (connected < roomCount) {
		for (int i = 0; i < roomCount; i++) {
			bool valid = false;
			while (!valid && (!rooms[i].lLock || !rooms[i].rLock || !rooms[i].uLock || !rooms[i].dLock)) {
				room bachelor = rooms[i];
				int direction = rand() % 4;
				bool valid = true;
				int x0, y0, x1, y1;
				switch (direction) {
				case 0: // Left
					if (bachelor.lLock) {
						valid = false;
						break;
					}
					bachelor.x -= 1;
					bachelor.w += 1;
					x0 = bachelor.x;
					y0 = bachelor.y;
					x1 = bachelor.x + 1;
					y1 = bachelor.y + bachelor.h;
					break;
				case 1: // Right
					if (bachelor.rLock) {
						valid = false;
						break;
					}
					bachelor.w += 1;
					x0 = bachelor.x + bachelor.w - 1;
					y0 = bachelor.y;
					x1 = bachelor.x + bachelor.w;
					y1 = bachelor.y + bachelor.h;
					break;
				case 2: // Up
					if (bachelor.uLock) {
						valid = false;
						break;
					}
					bachelor.y -= 1;
					bachelor.h += 1;
					x0 = bachelor.x;
					y0 = bachelor.y;
					x1 = bachelor.x + bachelor.w;
					y1 = bachelor.y + 1;
					break;
				case 3: // Down
					if (bachelor.dLock) {
						valid = false;
						break;
					}
					bachelor.h += 1;
					x0 = bachelor.x;
					y0 = bachelor.y + bachelor.h - 1;
					x1 = bachelor.x + bachelor.w;
					y1 = bachelor.y + bachelor.h;
					break;
				}

				if (!valid) continue;

				if ((bachelor.network == 0 && bachelor.growths > maxGrowths) || bachelor.x < 0 || bachelor.y < 0 || bachelor.x + bachelor.w > w || bachelor.y + bachelor.h > h) {
					valid = false;
					rooms[i].locks[direction] = true;
					continue;
				}

				for (int y = y0; y < y1; y++) {
					for (int x = x0; x < x1; x++) {
						if (cells[y * w + x] != -1 && cells[y * w + x] != i) {
							valid = false;
							int net1 = rooms[i].network;
							int net2 = rooms[cells[y * w + x]].network;
							if (net1 == net2)
								continue;

							int min = net1 < net2 ? net1 : net2;
							int max = net1 > net2 ? net1 : net2;

							int replacements = 0;
							for (room& r : rooms) {
								if (r.network == max) {
									replacements++;
									r.network = min;
								}
							}
							if (min == 0) connected += replacements;
							if (bachelor.network == max) bachelor.network = min;
						}
					}
				}

				if (!valid) {
					rooms[i].locks[direction] = true;
					continue;
				}

				for (int y = y0; y < y1; y++)
					for (int x = x0; x < x1; x++)
						cells[y * w + x] = i;
				bachelor.growths++;
				rooms[i] = bachelor;
				break;
			}
		}
	}*/

	minFrameDelta = 1000 / 60;
}

void SDLG::OnFrame() {
	SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gameRenderer);

	int* cell = cells;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			if (*cell > -1) {
				int cx = cellmargin * (x + 1) + cellSize * x;
				int cy = cellmargin * (y + 1) + cellSize * y;
				SDL_Rect cellRect = {
					cx,
					cy,
					cellSize,
					cellSize
				};
				if (roomCount == 1)
					SDL_SetRenderDrawColor(gameRenderer,
						63,
						63,
						63,
						255
					);
				else
					SDL_SetRenderDrawColor(gameRenderer,
						*cell * 255 / (roomCount - 1),
						(1.0 - *cell / (double)(roomCount - 1)) * 255.0,
						63,
						255
					);
				SDL_RenderFillRect(gameRenderer, &cellRect);
			}
			cell++;
		}
	}

	SDL_RenderPresent(gameRenderer);
}

void SDLG::OnQuit() {

}