#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

// |----------------------------------------------------------------------------|
// | Made This in 17/03/26 and 18/03/26 Without Any Tutorials Or GameDev Theory |
// |----------------------------------------------------------------------------|

// ----------------------------------------------
//    Start Of Field Related Stuffs And Things
// ----------------------------------------------

// Default Size 55x17, Keep Odd Numbers, Best Not To Touch At All
#define FIELDLENGTH 55
#define FIELDWIDTH 17

char field[FIELDWIDTH][FIELDLENGTH+1];
int scoreLeft = 0;
int scoreRight = 0; 

double timeElapsed = 0;

void initField() {
	field[0][0] = field[0][FIELDLENGTH - 1] = field[FIELDWIDTH - 1][0] = field[FIELDWIDTH - 1][FIELDLENGTH - 1] = '+';

	for (int i = 1; i < FIELDWIDTH - 1; i++) {
		for (int j = 1; j < FIELDLENGTH - 1; j++) {
			field[i][j] = ' ';
		}
	}

	for (int i = 1; i < FIELDLENGTH - 1; i++) {
		field[0][i] = '-';
		field[FIELDWIDTH - 1][i] = '-';
	}

	for (int i = 1; i < FIELDWIDTH - 1; i++) {
		field[i][0] = '|';
		field[i][FIELDLENGTH - 1] = '|';
		if (i % 2) field[i][FIELDLENGTH / 2] = '|';
		field[i][FIELDLENGTH] = '\0';
	}
	field[FIELDWIDTH-1][FIELDLENGTH] = '\0';
}

void printScore() {
	for (int i = 0; i < FIELDLENGTH / 4-2; i++) printf(" ");
	printf("W / S");
	for (int i = 0; i < FIELDLENGTH / 4 - 5; i++) printf(" "); //   W / S     UP / DOWN    O / L   
	printf("UP / DOWN");
	for (int i = 0; i < FIELDLENGTH / 4 - 7; i++) printf(" ");
	printf("O / L");

	printf("\n\n");

	for (int i = 0; i < FIELDLENGTH / 4; i++) printf(" ");
	printf("%d", scoreLeft);
	for (int i = 0; i < FIELDLENGTH / 4 - 2; i++) printf(" "); //    0     Score      0
	printf("Score");
	for (int i = 0; i < FIELDLENGTH / 4 - 2; i++) printf(" ");
	printf("%d\n", scoreRight);
}

void printField() {
	printScore();

	for (int i = 0; i < FIELDWIDTH; i++) printf("%s\n", field[i]);
}

void hideCursor() { // AI COPY PASTE
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &info);
}

void resetCursor() { // AI COPY PASTE
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { 0, 0 };
	SetConsoleCursorPosition(hConsole, pos);
}

// --------------------------------------------
//    End Of Field Related Stuffs And Things
// --------------------------------------------


// ---------------------------------------------
//    Start Of Ball Related Stuffs And Things
// ---------------------------------------------

typedef enum {
	OPEN, CEILING, FLOOR, LEFTWALL, RIGHTWALL, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, LEFTPLAYER, RIGHTPLAYER
} BallCollision;

typedef enum {
	LEFT = -1, RIGHT = 1 
} HorizontalDirection;

typedef enum {
	DOWN = -1, UP = 1
} VerticalDirection;

typedef enum {
	LEFTPOINT = -1, CONTINUE, RIGHTPOINT
} PlayerScoreReturnValue;

typedef struct {
	double y;
	double x;
	HorizontalDirection horizontalDirection;
	VerticalDirection verticalDirection;
} Ball;

Ball ball;
double ballLastX;
double ballLastY;
int lastWasMidLine;

HorizontalDirection lastHitPlayer = LEFT;

void initBall() {
	ball.y = FIELDWIDTH / 2;
	ball.x = FIELDLENGTH / 2;
	ball.horizontalDirection = lastHitPlayer;
	ball.verticalDirection = rand() % 2 ? UP : DOWN;
	ballLastX = FIELDLENGTH / 2;
	ballLastY = FIELDWIDTH / 2;
	lastWasMidLine = !(FIELDWIDTH % 2);
}

#define COLLISIONTHRESHOLD 1.5

BallCollision checkWalls() {
	if ((int)ball.x > 2 && (int)ball.x < FIELDLENGTH - 2) { // Prevent Accessing Out Of Bounds
		if (ball.y <= COLLISIONTHRESHOLD && field[(int)ball.y][(int)ball.x - 1] == '#') return TOPLEFT;
		if (ball.y >= FIELDWIDTH - COLLISIONTHRESHOLD && field[(int)ball.y][(int)ball.x - 1] == '#') return BOTTOMLEFT;
		if (ball.y <= COLLISIONTHRESHOLD && field[(int)ball.y][(int)ball.x + 1] == '#') return TOPRIGHT;
		if (ball.y >= FIELDWIDTH - COLLISIONTHRESHOLD && field[(int)ball.y][(int)ball.x + 1] == '#') return BOTTOMRIGHT;
	}
	if (ball.y <= COLLISIONTHRESHOLD) return CEILING;
	if (ball.y >= FIELDWIDTH - COLLISIONTHRESHOLD) return FLOOR;
	if (ball.x <= COLLISIONTHRESHOLD) return LEFTWALL;
	if (ball.x >= FIELDLENGTH - COLLISIONTHRESHOLD) return RIGHTWALL;
	if (field[(int)ball.y][(int)ball.x - 1] == '#') return LEFTPLAYER;
	if (field[(int)ball.y][(int)ball.x + 1] == '#') return RIGHTPLAYER;
	if (ball.verticalDirection == UP && field[(int)ball.y - 1][(int)ball.x - 1] == '#') return TOPLEFT;
	if (ball.verticalDirection == DOWN && field[(int)ball.y + 1][(int)ball.x - 1] == '#') return BOTTOMLEFT;
	if (ball.verticalDirection == UP && field[(int)ball.y - 1][(int)ball.x + 1] == '#') return TOPRIGHT;
	if (ball.verticalDirection == DOWN && field[(int)ball.y + 1][(int)ball.x + 1] == '#') return BOTTOMRIGHT;
	return OPEN;
}

PlayerScoreReturnValue drawBall() {
	field[(int)ballLastY][(int)ballLastX] = lastWasMidLine ? '|' : ' ';

	ballLastX = ball.x;
	ballLastY = ball.y;

	if (field[(int)ball.y][(int)ball.x] == '|') lastWasMidLine = 1;
	else lastWasMidLine = 0;

	field[(int)ball.y][(int)ball.x] = 'O';

	BallCollision ballState = checkWalls();

	switch (ballState) {
	case TOPLEFT:
		ball.verticalDirection = DOWN;
		ball.horizontalDirection = RIGHT;
		break;
	case TOPRIGHT:
		ball.verticalDirection = DOWN;
		ball.horizontalDirection = LEFT;
		break;
	case BOTTOMLEFT:
		ball.verticalDirection = UP;
		ball.horizontalDirection = RIGHT;
		break;
	case BOTTOMRIGHT:
		ball.verticalDirection = UP;
		ball.horizontalDirection = LEFT;
		break;
	case CEILING:
		ball.verticalDirection = DOWN;
		break;
	case FLOOR:
		ball.verticalDirection = UP;
		break;
	case LEFTPLAYER:
		ball.horizontalDirection = RIGHT;
		break;
	case RIGHTPLAYER:
		ball.horizontalDirection = LEFT;
		break;
	case LEFTWALL:
		return RIGHTPOINT;
	case RIGHTWALL:
		return LEFTPOINT;
	} // Else Ball Is In The Open

	ball.x += (0.25 + timeElapsed/5) * ball.horizontalDirection;
	ball.y -= (0.13 + timeElapsed/50) * ball.verticalDirection;

	return CONTINUE;
}


// -------------------------------------------
//    End Of Ball Related Stuffs And Things
// -------------------------------------------


// ------------------------------------------------
//    Start Of Players Related Stuffs And Things
// ------------------------------------------------

#define LEFTPLAYERFIELDPOSITION(a) field[a][3]
#define RIGHTPLAYERFIELDPOSITION(a) field[a][FIELDLENGTH-4]
#define RACKETSIZE 5

typedef enum {
	MIDDLE, BOTTOM, TOP
} PlayerCollision;

typedef enum {
	LEFTPLAYERMOVED, NONEMOVED, RIGHTPLAYERMOVED
} LastPlayerMoved;

typedef struct {
	int wallPositions[RACKETSIZE];
} Player;

Player leftPlayer;
Player rightPlayer;

void initPlayers() {
	for (int i = 0; i < RACKETSIZE; i++) {
		leftPlayer.wallPositions[i] = rightPlayer.wallPositions[i] = FIELDWIDTH / 2 + (i - RACKETSIZE / 2);
	}
}

PlayerCollision checkPlayerCollision(Player* pl) {
	if (pl->wallPositions[0] == 1) return TOP;
	if (pl->wallPositions[RACKETSIZE - 1] == FIELDWIDTH - 2) return BOTTOM;
	return MIDDLE;
}

void drawPlayers() {
	for (int i = 0; i < RACKETSIZE; i++) {
		LEFTPLAYERFIELDPOSITION(leftPlayer.wallPositions[i]) = '#';
		RIGHTPLAYERFIELDPOSITION(rightPlayer.wallPositions[i]) = '#';
	}
}

void movePlayerInDirection(Player* pl, VerticalDirection dir) {
	if (dir == UP) {
		if (checkPlayerCollision(pl) != TOP) {
			for (int i = 0; i < RACKETSIZE; i++)
				pl->wallPositions[i]--;
		}
	}
	else {
		if (checkPlayerCollision(pl) != BOTTOM) {
			for (int i = 0; i < RACKETSIZE; i++)
				pl->wallPositions[i]++;
		}
	}
}

void clearPlayers() {
	for (int i = 1; i < FIELDWIDTH - 1; i++) {
		field[i][3] = ' ';
		field[i][FIELDLENGTH - 4] = ' ';
	}
}

LastPlayerMoved moveAPlayer(LastPlayerMoved alreadyMovedPlayer) {
	int PlayerInputStatus = kbhit();
	if (PlayerInputStatus) {
		char PlayerInputKey = getch();

		if (PlayerInputKey == 'w' && LEFTPLAYERMOVED != alreadyMovedPlayer) {
			movePlayerInDirection(&leftPlayer, UP);
			return LEFTPLAYERMOVED;
		}
		if (PlayerInputKey == 's' && LEFTPLAYERMOVED != alreadyMovedPlayer) {
			movePlayerInDirection(&leftPlayer, DOWN);
			return LEFTPLAYERMOVED;
		}
		if (PlayerInputKey == 'o' && RIGHTPLAYERMOVED != alreadyMovedPlayer) {
			movePlayerInDirection(&rightPlayer, UP);
			return RIGHTPLAYERMOVED;
		}
		if (PlayerInputKey == 'l' && RIGHTPLAYERMOVED != alreadyMovedPlayer) {
			movePlayerInDirection(&rightPlayer, DOWN);
			return RIGHTPLAYERMOVED;
		}
	}

	return NONEMOVED;
}

void movePlayers() {
	clearPlayers();
	drawPlayers();

	LastPlayerMoved playerMoved = moveAPlayer(NONEMOVED);

	moveAPlayer(playerMoved);
}


// ----------------------------------------------
//    End Of Players Related Stuffs And Things
// ----------------------------------------------

void resetField() {
	field[(int)ballLastY][(int)ballLastX] = ' ';
	initBall();
	field[(int)ballLastY][(int)ballLastX] = 'O';

	clearPlayers();
	initPlayers();
	drawPlayers();

	resetCursor();
	printField();	
}

int main() {
	srand(time(NULL));

	hideCursor();
	initField();

	initBall();

	initPlayers();

	drawPlayers();

	field[(int)ballLastY][(int)ballLastX] = 'O';
	resetCursor();
	printField();

	Sleep(2000);

	int gameOn = 1;

	int neededScoreToWin = 5;

	while (gameOn) {
		resetCursor();
		printField();

		movePlayers();

		PlayerScoreReturnValue scoreState = drawBall();

		timeElapsed += timeElapsed < 3.5 ? 0.001 : 0; // 3.5 For Max Speed To Not Let The Ball Jump More Than 1 Index Per Frame

		if (scoreState) {
			if (scoreState == RIGHTPOINT) {
				scoreRight += 1;
				lastHitPlayer = LEFT;
			}
			else if (scoreState == LEFTPOINT) {
				scoreLeft += 1;
				lastHitPlayer = RIGHT;
			}

			resetField();

			if (scoreLeft >= neededScoreToWin) {
				gameOn = 0;
				printf("\n         Left Wins!\n\n\n");
			}
			else if (scoreRight >= neededScoreToWin) {
				gameOn = 0;
				puts("");
				for (int i = 0; i < FIELDLENGTH - 20; i++) printf(" ");
				printf("Right Wins!\n\n\n");
			}

			timeElapsed = 0;

			Sleep(2000);
		}

		Sleep(8); // Best Framerate IMO
	}
	
	return 0;
}