#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<SDL.h>
#include<SDL_mixer.h>

#define SCREEN_WIDTH 640	//window width
#define SCREEN_HEIGHT 480	//window height
#define FPS 60 //Frames Per Second

typedef struct{

	int x, y; //position of the ball on the screen
	int w, h; //ball width and height
	int dx, dy; // movement vector (velocity)(movement in pixels per frame)
}ball_t; 

typedef struct{

	int x, y; //position of paddle on the screen
	int w, h; //paddle width and height
}paddle_t;

// Program globals
ball_t ball;
paddle_t paddle[2]; //paddle[0] is left paddle (AI) and paddle[1] is right paddle (Player)

int score[] = { 0,0 };


//define window and renderer
SDL_Window* window;
SDL_Renderer* renderer;

//define surfaces
SDL_Surface* screen;
SDL_Surface* title;
SDL_Surface* choose_color;
SDL_Surface* choose_mode;
SDL_Surface* numbermap;
SDL_Surface* end;

//define texture
SDL_Texture* screen_texture;

//define sound effects and music
Mix_Chunk* selectsound;
Mix_Chunk* hitsound;
Mix_Chunk* cpuwin;
Mix_Chunk* playerwin;
Mix_Chunk* scorepoint;
Mix_Music* backgroundmusic;



//Setup window,renderer,main surface,texture,images,audio (Aham mn el 4o8l tazbeet el 4o8l)
int init() {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0 || SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 10096) == -1)
	{
		printf("Audio device could not be opened!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}
	
	//Create window and renderer 
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);
	if (window == NULL) {
		printf("Window could not be created!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	//Create the main screen sruface where all the elements will be drawn onto (ball, paddles, net etc)
	screen = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA32);
	if (screen == NULL) {
		printf("Could not create the screen surface!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}
	
	//Create the screen texture to render the screen surface to the actual display
	screen_texture = SDL_CreateTextureFromSurface(renderer, screen);
	
	if (screen_texture == NULL) {
		printf("Could not create screen_texture!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}


	//Load all audio
	selectsound = Mix_LoadWAV("Audio/select_sound.wav");
	if (selectsound == NULL) {
		printf("Could not load selectsound audio!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	hitsound = Mix_LoadWAV("Audio/hitsound.wav");
	if (hitsound == NULL) {
		printf("Could not load hitsound audio!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	cpuwin = Mix_LoadWAV("Audio/lose.wav");
	if (cpuwin == NULL) {
		printf("Could not load lose.wav!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	playerwin = Mix_LoadWAV("Audio/win.wav");
	if (playerwin == NULL) {
		printf("Could not load win.wav!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	scorepoint = Mix_LoadWAV("Audio/scorepoint.wav");
	if (scorepoint == NULL) {
		printf("Could not load scorepoint audio!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	backgroundmusic = Mix_LoadMUS("Audio/Arcade_Game_BGM_3.mp3");
	if (backgroundmusic == NULL) {
		printf("Could not load background music: Arcade_Game_BGM_3.mp3!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}
	
	//adjust volumes of audio
	Mix_VolumeChunk(selectsound, 40);
	Mix_VolumeChunk(hitsound, 50);
	Mix_VolumeChunk(cpuwin, 60);
	Mix_VolumeChunk(playerwin, 60);
	Mix_VolumeChunk(scorepoint, 50);
	Mix_VolumeMusic(5);

	//load all images into program 
	title = SDL_LoadBMP("Images/title.bmp");
	if (title == NULL) {
		printf("Could not load title image!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}
	
	choose_color = SDL_LoadBMP("Images/paddle_color.bmp");
	if (choose_color == NULL) {
		printf("Could not load paddle_color.bmp image!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	choose_mode = SDL_LoadBMP("Images/mode.bmp");
	if (choose_mode == NULL) {
		printf("Could not load mode.bmp image!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	numbermap = SDL_LoadBMP("Images/numbermap.bmp");
	if (numbermap == NULL) {
		printf("Could not load numbermap image!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}

	end = SDL_LoadBMP("Images/gameover.bmp");
	if (end == NULL) {
		printf("Could not load end image!!!! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}


	//Remove background of images
	Uint32 colorkey = SDL_MapRGB(title->format, 255, 0, 255); //color to be removed from images (pink)
	SDL_SetColorKey(title, SDL_TRUE, colorkey);
	SDL_SetColorKey(numbermap, SDL_TRUE, colorkey);
	return 1;
} 

void init_pos() {

	ball.x = screen->w / 2-5;
	ball.y = screen->h / 2-5; 
	ball.w = 10;
	ball.h = 10;
	ball.dy = rand() % 9 - 4;   //initial dy random from -4 to 4 
	ball.dx = 1 * pow(-1, rand() % 2); //initial dx random -1 or 1 

	paddle[0].w = 10;
	paddle[0].h = 50;
	paddle[0].x = 20;
	paddle[0].y = screen->h / 2 - paddle[0].h / 2;
	

	paddle[1].w = 10;
	paddle[1].h = 50;
	paddle[1].x = screen->w - 20 - paddle[1].w;
	paddle[1].y = screen->h / 2 - paddle[1].h / 2;
	
} 

int check_score(int mode) {

	int i;

	//loop through player scores
	for (i = 0; i < 2; i++) {

		//check if score is at the score win limit
		if (score[i] == 10) {

			//reset scores 
			score[0] = 0; //reset AI score
			score[1] = 0; //reset player score

			//if AI wins 
			if (i == 0 && mode == 1) {
				Mix_PlayChannel(-1, cpuwin, 0);  //play gameover sound 
				return 0; 
			}
			else if (i == 0 && mode == 2) {
				Mix_PlayChannel(-1, playerwin, 0);  //play gameover sound 
				return 2;
			}
			//if player wins 
			else {
				Mix_PlayChannel(-1, playerwin, 0); //play gameover sound 
				return 1;
			}
		}
	}

	//return -1 if no one has reached a score of 10 yet
	return -1;
} 

//if return value is 1 collision occured. if return is 0, no collision.
int check_collision(paddle_t pad) {

	int left_ball, left_paddle;
	int right_ball, right_paddle;
	int top_ball, top_paddle;
	int bottom_ball, bottom_paddle;

	left_ball = ball.x;
	right_ball = ball.x + ball.w;
	top_ball = ball.y;
	bottom_ball = ball.y + ball.h;

	left_paddle = pad.x;
	right_paddle = pad.x + pad.w;
	top_paddle = pad.y;
	bottom_paddle = pad.y + pad.h;


	if (left_ball > right_paddle) 
		return 0;
	

	if (right_ball < left_paddle) 
		return 0;
	

	if (top_ball > bottom_paddle) 
		return 0;
	

	if (bottom_ball < top_paddle) 
		return 0;

	return 1;
} 

void move_ball()
{
	//Move the ball by its motion vector. 
	ball.x += ball.dx;
	ball.y += ball.dy;

	//score goal if ball hits the edge of the screen. 
	if (ball.x < 0) {  //if ball reaches left side of screen

		score[1] += 1;
		if (score[1] < 10) Mix_PlayChannel(-1, scorepoint, 0); 
		init_pos();
	}

	if (ball.x > screen->w - ball.w) {   //if ball reaches right side of screen

		score[0] += 1;
		if (score[0] < 10) Mix_PlayChannel(-1, scorepoint, 0);
		init_pos();
	}


	//if ball reaches upper and lower sides of screen change direction
	if (ball.y <= 3) { 
		ball.dy = -ball.dy;
		ball.y = 4;   //teleport ball to avoid multi collision with border
	}

	if (ball.y >= screen->h - ball.h - 3) {
		ball.dy = -ball.dy;
		ball.y = screen->h - ball.h - 4; //teleport ball to avoid multi collision with border
	}

	//check for collision with the paddle
	int i;

	for (i = 0; i < 2; i++) {

		int c = check_collision(paddle[i]);

		//collision detected	
		if (c == 1) {
			
			Mix_PlayChannel(-1, hitsound, 0);

			if (abs(ball.dx) < 25) { //speed limit
				//ball moving left
				if (ball.dx < 0) 
					ball.dx -= 1;  //to increase speed of the ball in left (negative) direction
				
				//ball moving right
				else 
					ball.dx += 1;  //to increase speed of the ball in right (positive) direction
				
			}
			//change ball direction
			ball.dx = -ball.dx;

			//change ball angle based on where on the paddle it hit
			int hit_pos = (paddle[i].y + paddle[i].h) - ball.y; //position of ball top from bottom of paddle

			if (hit_pos >= 0 && hit_pos < 7) 
				ball.dy = 4;
		
			else if (hit_pos >= 7 && hit_pos < 14) 
				ball.dy = 3;
			
			else if (hit_pos >= 14 && hit_pos < 21) 
				ball.dy = 2;
			
			else if (hit_pos >= 21 && hit_pos < 28) 
				ball.dy = 1;
			
			else if (hit_pos >= 28 && hit_pos < 32) 
				ball.dy = 0;
			
			else if (hit_pos >= 32 && hit_pos < 39) 
				ball.dy = -1;

			else if (hit_pos >= 39 && hit_pos < 46) 
				ball.dy = -2;

			else if (hit_pos >= 46 && hit_pos < 53) 
				ball.dy = -3;
			
			else if (hit_pos >= 53 && hit_pos <= 60) 
				ball.dy = -4;
			

			//teleport ball to avoid mutli collision when paddle hits ball from the bottom (el darba el mo4t3la)
			//ball moving right and inside AI paddle
			if (ball.dx > 0 && ball.x < 30)
				ball.x = 30;  //teleport ball in front of AI paddle


			//ball moving left and inside player paddle
			else if (ball.dx < 0 && ball.x > 610 - ball.w)
				ball.x = 610 - ball.w; //teleport ball in front of player paddle
			
		}
	}
} 

void move_paddle_ai() {

	int paddle_center = paddle[0].y + paddle[0].h / 2;
	int screen_center = screen->h / 2;
	int ball_speed = abs(ball.dy);
	
	//ball moving right
	if (ball.dx > 0) {

		//return to center position
		if (paddle_center < screen_center - 5)  //if paddle above center of screen, move down
			paddle[0].y += ball_speed;

		else if (paddle_center > screen_center + 5)  //if paddle below center of screen, move up
			paddle[0].y -= ball_speed;
	}
	
	//ball moving left
	else {

		//ball moving up or down
		if (ball.dy != 0) {

			if (ball.y > paddle_center)     //if ball below paddle center, move down
				paddle[0].y += ball_speed;

			else  //if ball above paddle center, move up
				paddle[0].y -= ball_speed;
		}

		
		int reference_pos = (paddle[0].y + paddle[0].h) - ball.y; //distance bewteen paddle bottom and ball top
		//ball moving straight across
		if (ball.dy == 0) {

			if (reference_pos >= 21 && reference_pos < 39) {}  //if ball at same level of paddle center region (21 to 39), don't move
			
			else if (ball.y < paddle_center)   //if ball above paddle center, move up
				paddle[0].y -= 5;

			else  //if ball below paddle center, move down
				paddle[0].y += 5;
			
		}
	}

	//limit ai movement bewteen the two borders
	if (paddle[0].y >= screen->h - 3 - paddle[0].h )
		paddle[0].y = screen->h - 3 - paddle[0].h ;

	else if (paddle[0].y <= 3)
		paddle[0].y = 3;
} 

void move_paddle(int i,char direction)  {

	// if the down arrow is pressed move paddle down
	if (direction == 'd') {

		if (paddle[i].y >= screen->h - 3 - paddle[i].h) 
			paddle[i].y = screen->h - 3 - paddle[i].h;
		else 
			paddle[i].y += 5;
	}

	// if the up arrow is pressed move paddle up
	if (direction == 'u') {

		if (paddle[i].y <= 3) 
			paddle[i].y = 3;
		else
			paddle[i].y -= 5;
	}
} 

void draw_arrows(SDL_Rect rect) {

	SDL_Rect arrow_src, arrow_dest;

	arrow_src.x = 89;
	arrow_src.y = 56;
	arrow_src.w = 12;
	arrow_src.h = 15;

	arrow_dest.x = rect.x - 12;
	arrow_dest.y = rect.y + 1;
	arrow_dest.w = 12;
	arrow_dest.h = 15;

	SDL_BlitSurface(choose_color, &arrow_src, screen, &arrow_dest);

	arrow_src.x += arrow_src.w;
	arrow_dest.x = rect.x + rect.w + 2;

	SDL_BlitSurface(choose_color, &arrow_src, screen, &arrow_dest);

} 

void draw_menu(char color1[], char color2[], int mode) {

	SDL_Rect dest1; //"PONG press space to start"
	SDL_Rect src2,dest2; //"choose paddle color"
	SDL_Rect src3, dest3; //selected mode
	SDL_Rect rect; //square of selected color 
	SDL_Rect arrow_src, arrow_dest; //selection arrow

	dest1.x = (screen->w / 2) - (title->w / 2);
	dest1.y = (screen->h / 2) - (title->h / 2);
	dest1.w = title->w;
	dest1.h = title->h;

	rect.w = 15;
	rect.h = 15;
	rect.y = (screen->h / 2) + (dest1.h / 2) + 10;
	//if one player draw one rectangle
	if (mode == 1)
		rect.x = (screen->w / 2) - (rect.w / 2);
	//if two players draw two rectangles
	else {

		rect.x = (screen->w / 2) - (rect.w / 2) - 30;
		draw_arrows(rect);

		if (strcmp(color2, "White") == 0) SDL_FillRect(screen, &rect, 0xffffffff);
		else if (strcmp(color2, "Green") == 0) SDL_FillRect(screen, &rect, 0xff32c31a);
		else if (strcmp(color2, "Sky") == 0) SDL_FillRect(screen, &rect, 0xff2bf2fa);
		else if (strcmp(color2, "Blue") == 0) SDL_FillRect(screen, &rect, 0xff0026f0);
		else if (strcmp(color2, "Red") == 0) SDL_FillRect(screen, &rect, 0xffdf0000);

		rect.x = (screen->w / 2) - (rect.w / 2) + 30;
	}

	draw_arrows(rect);
	if (strcmp(color1, "White") == 0) SDL_FillRect(screen, &rect, 0xffffffff);
	else if (strcmp(color1, "Green") == 0) SDL_FillRect(screen, &rect, 0xff32c31a);
	else if (strcmp(color1, "Sky") == 0) SDL_FillRect(screen, &rect, 0xff2bf2fa);
	else if (strcmp(color1, "Blue") == 0) SDL_FillRect(screen, &rect, 0xff0026f0);
	else if (strcmp(color1, "Red") == 0) SDL_FillRect(screen, &rect, 0xffdf0000);

	src2.x = 0;
	src2.y = 32;
	src2.w = choose_color->w;
	src2.h = 19;

	dest2.x = (screen->w / 2) - (choose_color->w / 2);
	dest2.y = rect.y + rect.h + 14;
	dest2.w = src2.w;
	dest2.h = src2.h;

	src3.x = 13;
	src3.y = 16;
	src3.h = 45;
	src3.w = 176;

	dest3.x = (screen->w / 2) - (choose_mode->w / 2) + 17;
	dest3.y = rect.y + rect.h + 14 + 40;
	dest3.w = src3.w;
	dest3.h = src3.h;
	
	arrow_src.x = 101;
	arrow_src.y = 56;
	arrow_src.w = 12;
	arrow_src.h = 15;

	arrow_dest.w = 12;
	arrow_dest.h = 15;
	arrow_dest.x = dest3.x - 15;
	if(mode==1)
		arrow_dest.y = dest3.y + 2;
	else
		arrow_dest.y = dest3.y +27;
	

	SDL_BlitSurface(title, NULL, screen, &dest1);
	SDL_BlitSurface(choose_color, &src2, screen, &dest2);
	SDL_BlitSurface(choose_mode, &src3, screen, &dest3);
	SDL_BlitSurface(choose_color, &arrow_src, screen, &arrow_dest);

} 

void draw_game_over(int r) {

	SDL_Rect p1,p2;
	SDL_Rect cpu;
	SDL_Rect dest;

	p1.x = 0;
	p1.y = 0;
	p1.w = end->w;
	p1.h = 75;

	p2.x = 0;
	p2.y = 75;
	p2.w = end->w;
	p2.h = 75;

	cpu.x = 0;
	cpu.y = 150;
	cpu.w = end->w;
	cpu.h = 75;

	dest.x = (screen->w / 2) - (end->w / 2);
	dest.y = (screen->h / 2) - (75 / 2);
	dest.w = end->w;
	dest.h = 75;


	switch (r) {

	case 0:
		SDL_BlitSurface(end, &cpu, screen, &dest);
		break;
	case 1:
		SDL_BlitSurface(end, &p1, screen, &dest);
		break;
	case 2:
		SDL_BlitSurface(end, &p2, screen, &dest);
		break;

	}

} 

void draw_net_R(SDL_Rect net,int i) {

	if (i == 15) return;

	//alternating net colors
	if (i % 2 == 0) SDL_FillRect(screen, &net, 0xff666666);
	else SDL_FillRect(screen, &net, 0xffebebeb);

	net.y = net.y + 30;

	draw_net_R(net, i + 1);
} 

void draw_net() {

	SDL_Rect net;

	net.w = 4;
	net.h = 15;
	net.x = (screen->w / 2) - (net.w / 2);
	net.y = 20;
	
	//draw the net using recursion
	draw_net_R(net, 0);
	
} 

void draw_border() {

	SDL_Rect border1;
	SDL_Rect border2;

	border1.x = 0;
	border1.y = 0;
	border1.w = screen->w;
	border1.h = 3;

	border2.x = 0;
	border2.y = screen->h - 3;
	border2.w = screen->w;
	border2.h = 3;

	SDL_FillRect(screen, &border1, 0xff945bfa);
	SDL_FillRect(screen, &border2, 0xff945bfa);
	
} 

void draw_paddle(char color1[],char color2[],int mode) {

	SDL_Rect pad;
	int i;

	for (i = 0; i < 2; i++) {

		pad.x = paddle[i].x;
		pad.y = paddle[i].y;
		pad.w = paddle[i].w;
		pad.h = paddle[i].h;

		if (i == 0 && mode==1) 
			SDL_FillRect(screen, &pad, 0xffdf0000);
		else if (i == 0 && mode==2) {
			if (strcmp(color2, "White") == 0) SDL_FillRect(screen, &pad, 0xffffffff);
			else if (strcmp(color2, "Green") == 0) SDL_FillRect(screen, &pad, 0xff32c31a);
			else if (strcmp(color2, "Sky") == 0) SDL_FillRect(screen, &pad, 0xff2bf2fa);
			else if (strcmp(color2, "Blue") == 0) SDL_FillRect(screen, &pad, 0xff0026f0);
			else if (strcmp(color2, "Red") == 0) SDL_FillRect(screen, &pad, 0xffdf0000);
		}
		if (i == 1) {
			if (strcmp(color1, "White") == 0) SDL_FillRect(screen, &pad, 0xffffffff);
			else if (strcmp(color1, "Green") == 0) SDL_FillRect(screen, &pad, 0xff32c31a);
			else if (strcmp(color1, "Sky") == 0) SDL_FillRect(screen, &pad, 0xff2bf2fa);
			else if (strcmp(color1, "Blue") == 0) SDL_FillRect(screen, &pad, 0xff0026f0);
			else if (strcmp(color1, "Red") == 0) SDL_FillRect(screen, &pad, 0xffdf0000);
		}
	}
} 

void draw_ball() {

	SDL_Rect b;
	b.x = ball.x;
	b.y = ball.y;
	b.w = ball.w;
	b.h = ball.h;
	int ball_speed = abs(ball.dx);

	//color of ball changing with speed
	if (ball_speed>=1 && ball_speed<=5)  SDL_FillRect(screen, &b, 0xffbdffe4);
	else if (ball_speed >= 6 && ball_speed <= 10)  SDL_FillRect(screen, &b, 0xff90ffd2);
	else if (ball_speed >= 11 && ball_speed <= 15)  SDL_FillRect(screen, &b, 0xff6bfdcd);
	else if (ball_speed >= 16 && ball_speed <= 20)  SDL_FillRect(screen, &b, 0xff3ffcb6);
	else if (ball_speed >= 21 && ball_speed <= 25)  SDL_FillRect(screen, &b, 0xff00ffa3);
	else SDL_FillRect(screen, &b, 0xffffffff);
	
} 

void draw_player_0_score() {

	SDL_Rect src;
	SDL_Rect dest;

	src.x = 0;
	src.y = 0;
	src.w = 64;
	src.h = 64;

	dest.x = (screen->w / 2) - src.w - 12; //12 is just padding spacing
	dest.y = 0;
	dest.w = 64;
	dest.h = 64;

	if (score[0] > 0 && score[0] < 10) 
		src.x += src.w * score[0];
	
	SDL_BlitSurface(numbermap, &src, screen, &dest);
} 

void draw_player_1_score() {

	SDL_Rect src;
	SDL_Rect dest;

	src.x = 0;
	src.y = 0;
	src.w = 64;
	src.h = 64;

	dest.x = (screen->w / 2) + 12;
	dest.y = 0;
	dest.w = 64;
	dest.h = 64;

	if (score[1] > 0 && score[1] < 10) 
		src.x += src.w * score[1];
	

	SDL_BlitSurface(numbermap, &src, screen, &dest);
} 

int main()
{
	//to hide console window
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	//if initilization failed show console and end program
	if (init() == 0)
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		printf("\nPress any key to exit.");
		getch();
		return 0;
	}
	
	Mix_PlayMusic(backgroundmusic, -1);

	const Uint8* keystate;

	char colors[5][10]={"Blue","Red","White","Green","Sky"};
	int index1 = 0, index2 = 1;

	int running = 1;
	int mode = 1;
	int state = 0;
	int winner;

	srand(SDL_GetTicks());  //seed for rand()

	// Initialize the ball and paddle position data. 
	init_pos();

	int crnt;
	int prev;
	while (running)
	{
		prev = SDL_GetTicks();
		//check for new events every frame
		SDL_Event event;
		while (SDL_PollEvent(&event)) {   
			if (event.type == SDL_QUIT)
				running = 0;
		}

		keystate = SDL_GetKeyboardState(NULL);

		if (keystate[SDL_SCANCODE_ESCAPE]) 
			running = 0;
		
		//draw background
		SDL_RenderClear(renderer);
		SDL_FillRect(screen, NULL, 0x00000000);

		//display main menu
		if (state == 0) {

			if (keystate[SDL_SCANCODE_SPACE])
				state = 1;
			
			//cycle through paddle colors
			if (keystate[SDL_SCANCODE_UP]) {
				if (mode != 1) {
					Mix_PlayChannel(-1, selectsound, 0);
					SDL_Delay(300);
				}
				mode = 1;
			}

			if (keystate[SDL_SCANCODE_DOWN]) {
				if (mode != 2) {
					Mix_PlayChannel(-1, selectsound, 0);
					SDL_Delay(300);
				}
				mode = 2;
			}

			//player 1 colors
			if (keystate[SDL_SCANCODE_RIGHT]) {
				if (index1 == 4) index1 = 0;
				else index1++;
				Mix_PlayChannel(-1, selectsound, 0);
				SDL_Delay(300);
			}

			else if (keystate[SDL_SCANCODE_LEFT]) {
				if (index1 == 0) index1 = 4;
				else index1--;
				Mix_PlayChannel(-1, selectsound, 0);
				SDL_Delay(300);
			}

			//player 2 colors
			if (keystate[SDL_SCANCODE_D] && mode==2) {
				if (index2 == 4) index2 = 0;
				else index2++;
				Mix_PlayChannel(-1, selectsound, 0);
				SDL_Delay(300);
			}

			else if (keystate[SDL_SCANCODE_A] && mode==2) {
				if (index2 == 0) index2 = 4;
				else index2--;
				Mix_PlayChannel(-1, selectsound, 0);
				SDL_Delay(300);
			}

			//draw menu 
			draw_menu(colors[index1],colors[index2],mode);
			
		}

		else if (state == 1)  
		{
			//check score
			winner = check_score(mode);

			//if either player wins, change to game over state
			if (winner == 0 || winner == 1 || winner == 2)
				state = 2;

			//move player paddle
			if (keystate[SDL_SCANCODE_DOWN]) 
				move_paddle(1,'d');
			
			if (keystate[SDL_SCANCODE_UP]) 
				move_paddle(1,'u');
			
			if (mode == 2) {

				if (keystate[SDL_SCANCODE_W])
					move_paddle(0, 'u');

				if (keystate[SDL_SCANCODE_S])
					move_paddle(0, 'd');
			}
			//paddle ai movement
			else 
				move_paddle_ai();

			//Move the balls for the next frame. 
			move_ball();

			//draw borders
			draw_border();

			//draw net
			draw_net();

			//draw paddles
			draw_paddle(colors[index1],colors[index2],mode);

			//draw ball
			draw_ball();

			//draw the score
			draw_player_0_score();

			//draw the score
			draw_player_1_score();
		}

		//display gameover
		else if (state == 2) {

			draw_game_over(winner);
			Mix_PauseMusic();
			if (keystate[SDL_SCANCODE_SPACE]) {
				state = 0;
				//delay for a little bit so the space bar press dosnt get triggered twice
				//while the main menu is showing
				SDL_Delay(500);
				Mix_ResumeMusic();
			}

		}

		//update texture with every pixel on screen
		SDL_UpdateTexture(screen_texture, NULL, screen->pixels, screen->w * sizeof(Uint32)); 

		//links texture with renderer
		SDL_RenderCopy(renderer, screen_texture, NULL, NULL);

		//draw to the display
		SDL_RenderPresent(renderer);

		//time it takes to render  frame in milliseconds 
		crnt = SDL_GetTicks() - prev;  
		if (crnt < 1000 / FPS) 
			SDL_Delay(1000 / FPS - crnt);
	}

	//free audio files
	Mix_FreeChunk(selectsound);
	Mix_FreeChunk(hitsound);
	Mix_FreeChunk(cpuwin);
	Mix_FreeChunk(playerwin);
	Mix_FreeChunk(scorepoint);
	Mix_FreeMusic(backgroundmusic);
	
	//Close audio 
	Mix_CloseAudio();

	//free loaded images
	SDL_FreeSurface(screen);
	SDL_FreeSurface(title);
	SDL_FreeSurface(choose_color);
	SDL_FreeSurface(choose_mode);
	SDL_FreeSurface(numbermap);
	SDL_FreeSurface(end);

	//free renderer and all textures used with it
	SDL_DestroyRenderer(renderer);

	//Destroy window 
	SDL_DestroyWindow(window);
	 
	//Quit SDL subsystems 
	SDL_Quit();
	
	return 0;
}  

