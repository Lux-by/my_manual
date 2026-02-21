#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// ===================
// Stars
// ==================

typedef struct {
	int x;
	int y;
	int value;
	int lifetime;
	int active;
} Star;

#define STAR_COUNT 5
Star stars[STAR_COUNT];

// =====================
// Global variables
// ====================

#define FPS 60
#define DELAY (1000000 / FPS)

#define H 20
#define W 30

struct termios oldt;

enum GameState {
  STATE_MENU,
  STATE_PLAY,
  STATE_PAUSE,
  STATE_GAMEOVER,
  STATE_EXIT
};

int state;

int x = W / 2; // Player starting position
int y = H / 2;

int score = 0; // scoring
int best_score = 0;
int time = 0;
int tick = 0;
int sx, sy;

// ====================
// Terminal control
// ===================

void init_terminal() {
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void restore_terminal() { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); }

// =======================
// Suporting functions
// =======================

void display_clean() { printf("\033[2J\033[H"); }

void draw(char field[H][W]) {
  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++)
      putchar(field[i][j]);
    putchar('\n');
  }
}

void build_world(char field[H][W]) {
  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++) {
      if (i == 0 || i == H - 1 || j == 0 || j == W - 1)
        field[i][j] = '#';
      else
        field[i][j] = '.';
    }
  }
}

void spawn_all_stars(Star *stars, int px, int py) {
  for (int i = 0; i < STAR_COUNT; i++) {
		do {
			stars[i].x = rand() % (W - 2) + 1;
			stars[i].y = rand() % (H - 2) + 1;
			stars[i].value = rand() % (30 + 1 - 10) + 10;
			stars[i].active = 1;
			stars[i].lifetime = 0;
		} while (stars[i].x == px && stars[i].y == py);
	}
}

void respawn_stars(Star *stars, int i, int px, int py) {
	if (stars[i].active == 0) {
		do {
			stars[i].lifetime = 0;
			stars[i].x = rand() % (W - 2) + 1;
			stars[i].y = rand() % (H - 2) + 1;
			stars[i].value = rand() % (30 + 1 - 10) + 10;
			stars[i].active = 1;
		} while (stars[i].x == px && stars[i].y == py);
	}
}

void reset_play_mode() {
  x = W / 2;
  y = H / 2;
  score = 0;
  time = 0;
  tick = 0;
  spawn_all_stars(stars, x, y);
}

//=====================
// LOOPS
// ====================

int menu_loop() {
  char c;
  reset_play_mode();
  while (1) {
    display_clean();
    printf("=====MENU====\n");
    printf("Select option: \n");
    printf("1 - Play\n");
    printf("q - Quit\n");
    fflush(stdout);

    if (read(STDIN_FILENO, &c, 1) > 0) {
      if (c == '1')
        return STATE_PLAY;
      if (c == 'q')
        return STATE_EXIT;
    }

    usleep(DELAY);
  }
}

int play_loop() {
  char c;
  char field[H][W];

  while (1) {
    display_clean();
    printf("=====PLAY=====\n");
    printf("Time: 00:%02d | Score: %d | Best Score: %d\n", time, score,
           best_score);
    build_world(field);
    field[y][x] = '@';
		for (int i = 0; i < STAR_COUNT; i++) {
			if (stars[i].active == 1)
				field[stars[i].y][stars[i].x] = '*';
		}
    draw(field);
    printf("wasd - move\n");
    printf("p - Pause\n");
    printf("q - Menu\n");
    fflush(stdout);

    int nx = x;
    int ny = y;

    if (read(STDIN_FILENO, &c, 1) > 0) {
      if (c == 'a')
        nx--;
      else if (c == 'd')
        nx++;
      else if (c == 'w')
        ny--;
      else if (c == 's')
        ny++;
      if (c == 'p')
        return STATE_PAUSE;
      if (c == 'q')
        return STATE_MENU;
    }

    if (x == 0 || x == W - 1 || y == 0 || y == H - 1 || time == 30) {
      if (score > best_score)
        best_score = score;
      return STATE_GAMEOVER;
    }

    x = nx;
    y = ny;

		for (int i = 0; i < STAR_COUNT; i++) {
    	if (x == stars[i].x && y == stars[i].y) {
      	score += stars[i].value;
      	stars[i].active = 0;
				respawn_stars(stars, i, x, y);
    	}
		}

    // time
    tick++;
    if (tick == 60) {
      time++;
      score++;
			for (int i = 0; i < STAR_COUNT; i++) {
				stars[i].lifetime += 1;
				if (stars[i].lifetime == 5) {
					stars[i].active = 0;
					respawn_stars(stars, i, x, y);
				}
			}
      tick = 0;
    }

    usleep(DELAY);
  }
}

int pause_loop() {
  char c;
  while (1) {
    display_clean();
    printf("=====PAUSE=====\n");
    printf("Time: 00:%02d | Score: %d | Best Score: %d\n", time, score,
           best_score);
    printf("Select option: \n");
    printf("r - Resume\n");
    printf("q - Menu\n");

    if (read(STDIN_FILENO, &c, 1) > 0) {
      if (c == 'r')
        return STATE_PLAY;
      if (c == 'q')
        return STATE_MENU;
    }

    usleep(DELAY);
  }
}

int gameover_loop() {
  char c;
  while (1) {
    display_clean();
    printf("=====GAMEOVER=====\n");
    printf("Score: %d | Best score: %d\n", score, best_score);
    printf("Select option: \n");
    printf("m - Menu\n");

    if (read(STDIN_FILENO, &c, 1) > 0)
      if (c == 'm')
        return STATE_MENU;

    usleep(DELAY);
  }
}

int main() {
  init_terminal();
  state = STATE_MENU;
  srand(getpid());

  while (state != STATE_EXIT) {

    if (state == STATE_MENU)
      state = menu_loop();

    else if (state == STATE_PLAY)
      state = play_loop();

    else if (state == STATE_PAUSE)
      state = pause_loop();

    else if (state == STATE_GAMEOVER)
      state = gameover_loop();
  }

  restore_terminal();
  return 0;
}
