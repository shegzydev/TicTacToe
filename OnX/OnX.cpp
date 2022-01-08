#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define W 512
#define H 512

SDL_Point *O(int x, int y);

int main(int args, char** argv) {
	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("OnX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Event event; //used to capture events like input

	//used for text rendering
	SDL_Surface *O_win = NULL;
	SDL_Surface *X_win = NULL;
	SDL_Surface *Tie = NULL;
	SDL_Texture *_Owin = NULL;
	SDL_Texture *_Xwin = NULL;
	SDL_Texture *_Tie = NULL;
	TTF_Font *font = TTF_OpenFont("consola.ttf", 64);
	if (!font) {
		std::cout << "Could not load font";
	}
	else {
		O_win = TTF_RenderText_Blended_Wrapped(font, "O wins", { 255,255,255 }, W);
		_Owin = SDL_CreateTextureFromSurface(renderer, O_win);
		
		Tie = TTF_RenderText_Blended_Wrapped(font, "Tie", { 255,255,255 }, W);
		_Tie = SDL_CreateTextureFromSurface(renderer, Tie);

		X_win = TTF_RenderText_Blended_Wrapped(font, "X wins", { 255,255,255 }, W);
		_Xwin = SDL_CreateTextureFromSurface(renderer, X_win);
	}

	//The game board
	int grid[3][3] = {
	{0,0,0},
	{0,0,0},
	{0,0,0}
	};
	//mouse position
	int mouseX = 0;
	int mouseY = 0;
	//position of the highlighting box
	int Xhighlight = 0;
	int Yhighlight = 0;
	//current grid position the mouse is over
	int Xcurrentindex = 0;
	int Ycurrentindex = 0;
	bool me = true; //Who is playing?
	int gameover = 0;//1: O wins 2: X wins 3: Tie
	int cnt = 0; //Number of times the players have played
	float timer = 0; //post gameover timer

	//Used for time measurement
	Uint32 start = 0;
	Uint32 end = 0;
	float deltatime = 0;

	auto reset = [&] {
		mouseX = 0;
		mouseY = 0;
		Xhighlight = 0;
		Yhighlight = 0;
		Xcurrentindex = 0;
		Ycurrentindex = 0;
		me = true;
		gameover = 0;
		timer = 0;
		cnt = 0;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				grid[i][j] = 0;
			}
		}
	};

	bool running = true;
	while (running) {
		start = SDL_GetTicks();

		SDL_SetRenderDrawColor(renderer, 12, 72, 255, 255);
		SDL_RenderClear(renderer);

		SDL_GetMouseState(&mouseX, &mouseY);

		Xhighlight = (mouseX / (W / 3)) * (W / 3);
		Yhighlight = (mouseY / (H / 3)) * (H / 3);

		Xcurrentindex = Xhighlight / (W / 3);
		Ycurrentindex = Yhighlight / (H / 3);

		if (gameover != 0) {
			timer += deltatime;
			if (timer >= 2) {
				timer = 0;
				gameover = 0;
				reset();
			}
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				{
					running = false;
					break;
				}
				case SDLK_r:
				{
					reset();
					break;
				}
				default:
				{
					break;
				}
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && gameover == 0) {
				if (grid[Xcurrentindex][Ycurrentindex] == 0) {
					if (me) {
						grid[Xcurrentindex][Ycurrentindex] = 1;
						me = false;
					}else{
						grid[Xcurrentindex][Ycurrentindex] = 2;
						me = true;
					}
					cnt++;
				}
			}
		}

		//Checking for gameover
		if (gameover == 0) {
			for (int i = 0; i < 3; i++) {
				int r_indx = 3;
				int c_indx = 3;
				int r_indo = 3;
				int c_indo = 3;
				for (int j = 0; j < 3; j++) {
					if (grid[i][j] == 2) {
						r_indx--;
					}
					if (grid[j][i] == 2) {
						c_indx--;
					}
					if (grid[i][j] == 1) {
						r_indo--;
					}
					if (grid[j][i] == 1) {
						c_indo--;
					}
				}
				if (r_indx == 0 || c_indx == 0) {
					gameover = 2;
					break;
				}
				else if (r_indo == 0 || c_indo == 0) {
					gameover = 1;
					break;
				}
			}

			if (gameover > 0) goto label;

			if ((grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2] && grid[2][2] == 1) || (grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0] && grid[2][0] == 1)) {
				gameover = 1;
			}
			else if ((grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2] && grid[2][2] == 2) || (grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0] && grid[2][0] == 2)) {
				gameover = 2;
			}
			else if (cnt >= 9) {
				gameover = 3;
			}
		}
		label:

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		for (int i = 1; i < 3; i++) {
			int x = i * W / 3;
			SDL_RenderDrawLine(renderer, x, 0, x, H);
		}
		for (int i = 1; i < 3; i++) {
			int y = i * H / 3;
			SDL_RenderDrawLine(renderer, 0, y, W, y);
		}
		if (grid[Xhighlight / (W / 3)][Yhighlight / (H / 3)] != 0) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		}
		else {
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		}
		SDL_Rect rect = { Xhighlight, Yhighlight, W / 3, H / 3 };
		SDL_RenderDrawRect(renderer, &rect);

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (grid[i][j] == 1) {
					SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
					int x = i * (W / 3) + (W / 6);
					int y = j * (H / 3) + (H / 6);
					SDL_RenderDrawLines(renderer, O(x, y), 33);
				}
				else if (grid[i][j] == 2) {
					SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
					int x = i * (W / 3) + (W / 6);
					int y = j * (H / 3) + (H / 6);
					SDL_RenderDrawLine(renderer, x - 64, y + 64, x + 64, y - 64);
					SDL_RenderDrawLine(renderer, x - 64, y - 64, x + 64, y + 64);
				}
			}
		}

		if (gameover == 1) {
			int w = 0;
			int h = 0;
			SDL_QueryTexture(_Owin, NULL, NULL, &w, &h);
			SDL_Rect go = { (W / 2) - (w / 2),(H / 2) - (h / 2), w, h };
			SDL_RenderCopy(renderer, _Owin, NULL, &go);
		}
		else if (gameover == 2) {
			int w = 0;
			int h = 0;
			SDL_QueryTexture(_Xwin, NULL, NULL, &w, &h);
			SDL_Rect go = { (W / 2) - (w / 2),(H / 2) - (h / 2), w, h };
			SDL_RenderCopy(renderer, _Xwin, NULL, &go);
		}
		else if (gameover == 3) {
			int w = 0;
			int h = 0;
			SDL_QueryTexture(_Tie, NULL, NULL, &w, &h);
			SDL_Rect go = { (W / 2) - (w / 2),(H / 2) - (h / 2), w, h };
			SDL_RenderCopy(renderer, _Tie, NULL, &go);
		}
		SDL_RenderPresent(renderer);

		//Calculating frame time
		end = SDL_GetTicks() - start;
		deltatime = end / 1000.0;
	}

	SDL_DestroyTexture(_Owin);
	SDL_DestroyTexture(_Xwin);
	SDL_FreeSurface(O_win);
	SDL_FreeSurface(X_win);
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

SDL_Point *O(int x, int y) {
	double interval = (2.0 * M_PI) / 32;
	SDL_Point points[33];
	for (int i = 0; i < 33; i++) {
		points[i] = { 0,0 };
	}
	int ind = 0;
	for (double i = 0; i <= (2.0 * M_PI); i += interval) {
		points[ind] = { int(x + 64 * cos(i)),int(y + 64 * sin(i)) };
		ind++;
	}
	points[32] = points[0];
	return points;
}