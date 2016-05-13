#include <stdio.h>
#include <time.h>
#include <windows.h>

#define LINE 2
#define BLOCK 1
#define EMPTY 0

#define X_LEFT_BOARDER 5 //게임 보드 왼쪽 경계
#define X_RIGHT_BOARDER 17 //게임 보드 오른쪽 경계
#define Y_TOP_BOARDER 3 //게임 보드 위쪽 경계
#define Y_BOT_BOARDER 28 //게임 보드 아래쪽 경계
#define X_LEN 12 //게임보드 가로 길이
#define Y_LEN 25 //게임보드 세로 길이
#define SCORE_HEIGHT 3 //점수 보드 높이
#define SCORE_WIDTH  8 //점수 보드 넓이
#define SCORE_STARTX 33 //점수 그리기 시작 x좌표
#define SCORE_STARTY 25 //점수 그리기 시작 y좌표

const int AISPEED = 7000;//AI게임속도 조절_숫자가 낮을수록 빠름
int SCORE;
int BLOCK_ARR[4][4] = { 0 };
int BIT_X = 4, BIT_Y = 1, BIT_Z = 0;
int randomNum;//똑같은 블록이 못 나오게 점검하는 변수
int BOARD_ARR[Y_LEN][X_LEN] = { EMPTY, };
int scoreBoard[SCORE_HEIGHT][SCORE_WIDTH];// 점수 출력 보드를 위한 2차원 배열 
int rank1 = 0, rank2 = 0, rank3 = 0, rank4 = 0, rank5 = 0;

char COLOUR[7][4] = {
	"□","♡","ペ","■","★","の","☆"
};

const int _BLOCK_ARR[7][4][4] = {
	{
		{ 0,1,0,0 },
		{ 0,1,0,0 },
		{ 0,1,0,0 },
		{ 0,1,0,0 }
	},
	{
		{ 0,0,0,0 },
		{ 0,0,1,0 },
		{ 0,1,1,0 },
		{ 0,0,1,0 }
	},
	{
		{ 0,0,0,0 },
		{ 0,1,0,0 },
		{ 0,1,1,1 },
		{ 0,0,0,0 }
	},
	{
		{ 0,0,0,0 },
		{ 0,0,1,0 },
		{ 1,1,1,0 },
		{ 0,0,0,0 }
	},
	{
		{ 0,0,0,0 },
		{ 0,1,1,0 },
		{ 1,1,0,0 },
		{ 0,0,0,0 }
	},
	{
		{ 0,0,0,0 },
		{ 0,1,1,0 },
		{ 0,0,1,1 },
		{ 0,0,0,0 }
	},
	{
		{ 0,0,0,0 },
		{ 0,1,1,0 },
		{ 0,1,1,0 },
		{ 0,0,0,0 }
	}
};

typedef struct position {
	int x, y;
	int rotation;
	int score;
}Pos;

enum { LEFT_KEY = 1, DOWN_KEY, RIGHT_KEY, UP_KEY, QUIT, SPACE_KEY };

int writeBlockOnBoard(void);
void eraseBlockOffBoard(void);
void initGame(void);
void drawBoarder(void);
void drawInsideGame(void);
void rotateBlock(void);
int collision(void);
void stack(void);
void moveBit(char ch);
void gravity(int y);
int clearLine(void);
void randomNextBlock(void);
int getIdealPoint(void);
Pos findIdealPosition(void);
int getClearLinePoint(void);
void removeCursor(void);
int getAdjacentPoint(void);
int getMinusPoint(void);
void animationeffect(double par);
void moveBestPos(Pos bestPosition, char *nxt);
void gotoXY(int x, int y);
void rank();

int main(void)
{
	int x = 0, y = 0, i = 0;
	char NXT = '\0';
	Pos bestPosition;

	START:
	removeCursor();
	initGame();
	drawBoarder();
	randomNextBlock();

	while (NXT != QUIT) {
		bestPosition = findIdealPosition();
		moveBestPos(bestPosition, &NXT);
		writeBlockOnBoard();
		drawInsideGame();
		eraseBlockOffBoard();
	}

	rank();

	goto START;

	return 0;
}

void initGame(void) {
	system("cls");
	int SCORE = 0;
	srand((unsigned int)time(NULL));
	int x = 0, y = 0;
	randomNum = 9;
	for (x = 0; x<X_RIGHT_BOARDER - X_LEFT_BOARDER; x++) {
		BOARD_ARR[0][x] = LINE;
		BOARD_ARR[Y_LEN - 1][x] = LINE;
	}
	for (y = 0; y<Y_BOT_BOARDER - Y_TOP_BOARDER; y++) {
		BOARD_ARR[y][0] = LINE;
		BOARD_ARR[y][X_LEN - 1] = LINE;
	}

	//점수 보드 초기화
	for (y = 0; y< SCORE_HEIGHT; y++)
	{
		for (x = 0; x<SCORE_WIDTH; x++)
		{
			if ((x == 0 || x == SCORE_WIDTH - 1) ||
				(y == 0 || y == SCORE_HEIGHT - 1))
			{
				scoreBoard[y][x] = LINE;
			}
			else
			{
				scoreBoard[y][x] = EMPTY;
			}
		}
	}
}

void drawBoarder(void) {
	int x = 0, y = 0, k = 0;
	for (x = 0, k = 0; x<X_LEN; x++, k++) {
		for (y = 0; y<Y_LEN; y++) {
			if (BOARD_ARR[y][x] == LINE) {
				gotoXY(k + x + X_LEFT_BOARDER, y + Y_TOP_BOARDER);
				printf("▣");
			}
		}
	}
	gotoXY(7, 2); printf("HCI's TETRIS GAME");

	//5. 게임 화면에 점수 보드 창을 그리기
	for (y = 0; y<SCORE_HEIGHT; y++)
	{
		gotoXY(SCORE_STARTX, SCORE_STARTY + y);
		for (x = 0; x < SCORE_WIDTH; x++)
			if (scoreBoard[y][x] == LINE) printf("♥");
			else
				printf("  ");
	}

	gotoXY (35, 3);
	printf ("1등 : %d점", rank1);
	gotoXY(35, 4);
	printf ("2등 : %d점", rank2);
	gotoXY(35, 5);
	printf ("3등 : %d점", rank3);
	gotoXY(35, 6);
	printf ("4등 : %d점", rank4);
	gotoXY(35, 7);
	printf ("5등 : %d점", rank5);
}

void randomNextBlock(void) {
	int x, y;

	BIT_Z = rand() % 7;

	if (BIT_Z == randomNum)
		BIT_Z = rand() % 7;
	else
		randomNum = BIT_Z;

	for (y = 0; y<4; y++) {
		for (x = 0; x<4; x++) {
			BLOCK_ARR[y][x] = _BLOCK_ARR[BIT_Z][y][x];
		}
	}
}

int writeBlockOnBoard(void) {
	int x = 0, y = 0;
	for (x = 0; x<4; x++) {
		for (y = 0; y<4; y++) {
			if (BLOCK_ARR[y][x] != EMPTY) {
				BOARD_ARR[BIT_Y + y][BIT_X + x] = BIT_Z + 1;
			}
		}
	}
	return 0;
}

Pos findIdealPosition(void) {
	int firstX = BIT_X, firstY = BIT_Y;
	Pos position[4] = { 0, };
	int highestScore = 0, plusScore = 0, minusScore = 0, totalScore = 0;
	int rotation = 0, bestRotation = 0;
	int i = 0;

	for (rotation = 0; rotation<4; rotation++) {
		for (BIT_X = 0, i = 0; 1; BIT_X++) {
			BIT_Y = 1;

			if (i == 0 && collision()) {
				BIT_X = 1; i++;
			}
			else if (collision()) {
				break;
			}
			while (!collision()) {
				BIT_Y++;
			}
			BIT_Y--;
			writeBlockOnBoard();
			plusScore = 4 * getIdealPoint() + 4 * getClearLinePoint() + 4 * getAdjacentPoint();
			minusScore = 14 * getMinusPoint();
			totalScore = plusScore - minusScore;
			eraseBlockOffBoard();

			if (highestScore <= totalScore) {
				highestScore = totalScore;
				position[rotation].x = BIT_X;
				position[rotation].y = BIT_Y;
				position[rotation].rotation = rotation;
				position[rotation].score = totalScore;
			}
		}

		rotateBlock();
	}

	for (rotation = 0; rotation<4; rotation++) {
		if (position[rotation].score >= highestScore) {
			highestScore = position[rotation].score;
			bestRotation = rotation;
		}
	}

	BIT_X = firstX; BIT_Y = firstY;
	return position[bestRotation];
}//최적의 회전 형태 찾기

void moveBestPos(Pos bestPosition, char *nxt) {
	int i;
	BIT_Y = 1; BIT_X = 5;
	for (i = 0; i<bestPosition.rotation; i++) {
		rotateBlock();
		if (collision())
			*nxt = QUIT;
		animationeffect(2);
	}
	while (BIT_X != bestPosition.x) {
		if (BIT_X>bestPosition.x) {
			BIT_X--; animationeffect(2);
			if (collision())
				*nxt = QUIT;
		}
		else if (BIT_X<bestPosition.x) {
			BIT_X++; animationeffect(2);
			if (collision())
				*nxt = QUIT;

		}
	}
	animationeffect(4);
	while (!collision()) {
		animationeffect(0.5);
		BIT_Y++;
	}
	stack();
	return;
}

void drawInsideGame(void) {
	int x = 0, y = 0, k = 0;
	for (y = 1; y<Y_LEN - 1; y++) {
		for (x = 1, k = 1; x<X_LEN - 1; x++, k++) {
			if (BOARD_ARR[y][x] == EMPTY) {
				gotoXY(k + x + X_LEFT_BOARDER, y + Y_TOP_BOARDER);
				printf(" ");
			}
			else if (BOARD_ARR[y][x] >= 1 && BOARD_ARR[y][x]<8) {
				gotoXY(k + x + X_LEFT_BOARDER, y + Y_TOP_BOARDER);
				printf("%s", COLOUR[BOARD_ARR[y][x] - 1]);
			}
		}
	}

	//점수 출력
	gotoXY(SCORE_STARTX + 3, SCORE_STARTY - 1);
	printf("S C O R E");
	gotoXY(SCORE_STARTX + 5, SCORE_STARTY + 1);
	printf("%d", SCORE);
}

void eraseBlockOffBoard(void) {
	int x, y;
	for (y = 0; y<4; y++) {
		for (x = 0; x<4; x++) {
			if (BLOCK_ARR[y][x] != EMPTY) {
				BOARD_ARR[y + BIT_Y][x + BIT_X] = EMPTY;
			}
		}
	}
}

//////////////블록이 꽉찬 줄 제거 함수////////////
int clearLine(void) {
	int x = 0, y = 0, cnt;
	for (y = Y_LEN - 2; y>1; y--) {
		cnt = 0;
		for (x = 1; x<X_LEN - 1; x++) {
			if (BOARD_ARR[y][x] != EMPTY)
				cnt += 1;
		}
		if (cnt == 1 * (X_LEN - 2)) {
			SCORE += 10;
			gravity(y);
			y = Y_LEN - 1;
		}
	}
	return 0;
}

void gravity(int y) {
	int x = 1;
	for (; y >1; y--) {
		for (x = 1; x < X_LEN - 1; x++) {
			BOARD_ARR[y][x] = BOARD_ARR[y - 1][x];
		}
	}
}//한줄에 블록이 꽉차면 해당 블록의 윗 줄 블록을 한 줄씩 내려서 덮어버림  

 ////////////점수 연산 함수들////////////
int getIdealPoint(void) {
	int boardIdealPoint = 0;
	int x = 0, y = 0;
	for (y = 2; y<Y_LEN - 1; y++) {
		for (x = 1; x<X_LEN - 1; x++) {
			if (BOARD_ARR[y][x] != EMPTY) {
				boardIdealPoint += y;
			}
		}
	}
	return boardIdealPoint;
}//블록이 아래로 갈수록 점수가 높다

int getClearLinePoint(void) {
	int clearLinePoint = 0;
	int x = 0, y = 0;
	int count = 0;
	for (y = BIT_Y; y<BIT_Y + 4; y++) {
		for (x = 1, count = 0; x<X_LEN - 1; x++) {
			if (BOARD_ARR[y][x] != EMPTY) {
				count++;
			}
		}
		if (count == X_LEN - 2) {
			clearLinePoint += y*(X_LEN - 2);
		}
	}
	return clearLinePoint;
}
//한줄을 다 채워서 블록을 깰 수 있으면 점수를 준다 & 여러 줄 깰수록 높은 점수 

int getAdjacentPoint(void) {
	int adjacentPoint = 0;
	int x = 0, y = 0;
	for (y = 0; y<4; y++) {
		for (x = 0; x<4; x++) {
			if (BLOCK_ARR[y][x] != EMPTY) {
				if (BOARD_ARR[BIT_Y + y][BIT_X + x - 1] != EMPTY) {
					adjacentPoint += BIT_Y + y;
				}
				if (BOARD_ARR[BIT_Y + y][BIT_X + x + 1] != EMPTY)
					adjacentPoint += BIT_Y + y;
			}
		}
	}
	return adjacentPoint;
}//블록의 왼쪽 또는 오른쪽에 인접한 블록이 있을수록 점수가 높다 & 인접한 수준이 같으면 밑에 있을 수록 점수가 높다

int getMinusPoint(void) {
	int boardMinusPoint = 0;
	int x = 0, y = 0;
	int height = 0;

	for (x = 1; x<X_LEN - 1; x++) {
		for (y = 2; BOARD_ARR[y][x] == EMPTY; y++) {
			height = y;
		}
		for (y = Y_LEN - 1; y>height; y--) {
			if (BOARD_ARR[y][x] == EMPTY) {
				boardMinusPoint += y;
			}
		}
	}
	return boardMinusPoint;
}//각 줄을 세로로 검사하여 블록조각 사이에 빈 공간이 있으면 마이너스 포인트가 높다

 ////////////////////////////////////////////////////////

void gotoXY(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void removeCursor(void) // 커서를 안보이게한다
{
	CONSOLE_CURSOR_INFO curInfo;

	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

	curInfo.bVisible = 0;

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

}

void rotateBlock(void) {
	char tmpArr[4][4];
	int x = 0, y = 0;
	for (y = 0; y<4; y++) {
		for (x = 0; x<4; x++) {
			tmpArr[y][x] = BLOCK_ARR[y][x];
		}
	}
	for (y = 0; y<4; y++) {
		for (x = 0; x<4; x++) {
			BLOCK_ARR[x][3 - y] = tmpArr[y][x];
		}
	}
}//블록 90도 회전

void stack(void) {
	BIT_Y--;
	writeBlockOnBoard();
	randomNextBlock();
	BIT_Y = 1; BIT_X = 4;
	clearLine();
}

void animationeffect(double par) {
	writeBlockOnBoard();
	drawInsideGame();
	eraseBlockOffBoard();
	Sleep((unsigned long)(AISPEED*par / 1000.0));
}

int collision(void) {
	int x = 0, y = 0;
	int collision = 0;
	for (x = 0; x<4; x++) {
		for (y = 0; y<4; y++) {
			if (BLOCK_ARR[y][x] != EMPTY) {
				if (BOARD_ARR[BIT_Y + y][BIT_X + x] != EMPTY) {
					return 1;
				}
			}
		}
	}
	return 0;
}

void moveBit(char ch) {
	switch (ch) {
	case DOWN_KEY:
		BIT_Y++;
		if (collision()) {
			stack();
		}
		break;
	case UP_KEY:
		rotateBlock();
		if (collision()) rotateBlock();
		break;
	case LEFT_KEY:
		BIT_X--;
		if (collision()) BIT_X++;
		break;
	case RIGHT_KEY:
		BIT_X++;
		if (collision()) BIT_X--;
		break;
	case SPACE_KEY:
		while (!collision()) {
			BIT_Y++;
		}
		BIT_Y--;
		break;
	default:
		break;
	}
}

void rank(void)
{
	if (rank4 > rank5)
		rank5 = SCORE;
	else if (rank3 > rank4 && rank4 >= rank5)
		rank4 = SCORE;
	else if (rank2 > rank3 && rank3 >= rank4)
		rank3 = SCORE;
	else if (rank1 > rank2 && rank2 >= rank3)
		rank2 = SCORE;
	else
		rank1 = SCORE;
}