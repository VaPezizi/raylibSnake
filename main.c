#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Some variables
const int screenWidth = 800;
const int screenHeight = 800;

const int cellCount = 20;
const int cellSize = screenWidth / cellCount; //800 / 40,
const int STARTLENGHT = 5;

//Structs and enums

enum DIRECTION{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

typedef struct SnakePart{
	int posX;
	int posY;
	struct SnakePart * next;	
}SnakePart;

typedef struct{
	SnakePart * HEAD;
	int direction;
	int score;
}Snake;

typedef struct{
	int posX;
	int posY;
}Apple;

void * safeMalloc(size_t n){
	void *p = malloc(n);
	if(p==NULL){
		fprintf(stderr, "Failed to allocate %zu bytes!\n", n);
		abort();
	}
	return p;
}

void moveNode(Snake * snake){
	if(snake->HEAD == NULL){
		fprintf(stderr, "Error in snake moving");
		abort();
	}
	if(snake->score < 3) return;
	SnakePart * current = snake->HEAD;

	while(current->next->next != NULL){
		current = current->next;
	}
	current->next->next = snake->HEAD;
	snake->HEAD = current->next;
	current->next = NULL;

		
}

int newAppleInsideSnake(Snake * snake, int * rx, int * ry){
	
	SnakePart * part = snake->HEAD;	
	
	while(part != NULL){
		//puts("ALOO");
		printf("PosX: %d\nPosY: %d\nrx*cellSize: %d\nry*cellSize:%d\n", part->posX, part->posY, *rx * cellSize, *ry * cellSize);
		if(part->posX == *rx * cellSize && part->posY == *ry)return 1; 
		part = part->next;	
	}

	return 0;
}


void moveApple(Snake * snake, Apple * apple){
	srand(time(NULL));
	int ry = rand() % cellCount;
	int rx = rand() % cellCount;

	while(newAppleInsideSnake(snake, &rx, &ry)){
		ry = rand() % cellCount;
		rx = rand() % cellCount;
		puts("HALOOO TAALA");
	}
	

	apple->posX = rx;
	apple->posY = ry;

	
}

int checkCollision(Snake * snake, Apple * apple){
	return CheckCollisionPointRec((Vector2){snake->HEAD->posX, snake->HEAD->posY}, (Rectangle){apple->posX * cellSize, apple->posY * cellSize, cellSize, cellSize});
}

//Returns -1 if the move is "Illegal" (Meaning if you would lose the game)
int moveSnake(Snake * snake, int direction){
	
	if(snake->HEAD == NULL) return -1;	
	
	moveNode(snake);
	switch(direction){
		case RIGHT:
			snake->HEAD->posX = snake->HEAD->next->posX + cellSize;	
			snake->HEAD->posY = snake->HEAD->next->posY;
			break;
		case LEFT:
			snake->HEAD->posX = snake->HEAD->next->posX - cellSize;	
			snake->HEAD->posY = snake->HEAD->next->posY;
			break;
		case UP:
			snake->HEAD->posY = snake->HEAD->next->posY - cellSize;
			snake->HEAD->posX = snake->HEAD->next->posX;
			break;
		case DOWN:
			snake->HEAD->posY = snake->HEAD->next->posY + cellSize;
			snake->HEAD->posX = snake->HEAD->next->posX;
			break;
		default:
			fprintf(stderr, "ERROR IN SNAKE MOVING");
	}	

	return 1;
}

void addPart(const int posY, const int posX, Snake * snake){
	snake->score++;
	if(snake->HEAD == NULL){
		snake->HEAD = safeMalloc(sizeof(SnakePart));
		snake->HEAD->next = NULL;
		snake->HEAD->posX = posX;
		snake->HEAD->posY = posY;
	}
	else{
		/*SnakePart * temp = snake->HEAD;
		while(temp->next != NULL){
			temp = temp->next;
		}
		snake->score++;*/
		SnakePart * temp = safeMalloc(sizeof(SnakePart));
		temp->next = snake->HEAD;
		temp->posX = posX;
		temp->posY = posY;
		snake->HEAD = temp;
		return;
		
	}	
	return;	
}
void drawSnake(Snake * snake){
	if(snake->HEAD == NULL){
		
		fprintf(stderr, "Snake not initialized!!");
		abort();
	}
	else{
		SnakePart * temp = snake->HEAD;
		while(temp->next != NULL){
			DrawRectangle(temp->posX, temp->posY, cellSize, cellSize, GREEN);
			temp = temp->next;
		}
		DrawRectangle(temp->posX, temp->posY, cellSize, cellSize, GREEN);

	}
}
void freeSnake(Snake * snake){
	if(snake == NULL) return;

	SnakePart * iter = snake->HEAD;
	SnakePart * temp = snake->HEAD;
	
	while(temp->next != NULL){
		temp = iter->next;
		free(iter);
		iter = temp;	
	}
	free(iter);
	snake = NULL;
	return;
}

int checkSnakeCollide(const Snake * snake){
	Vector2 pos = (Vector2){snake->HEAD->posX,snake->HEAD->posY};	

	if(snake->HEAD->next == NULL){
		
		fprintf(stderr, "Snake not initialized!!");
		abort();
	}
	else{
		SnakePart * temp = snake->HEAD->next;
		while(temp->next != NULL){
			if(CheckCollisionPointCircle(pos, (Vector2){temp->posX, temp->posY}, 5)) return 1;
			temp = temp->next;
		}

		if(CheckCollisionPointCircle(pos, (Vector2){temp->posX, temp->posY}, 5)) return 1;

	}
	

	return 0;
}

int checkFail(Snake * snake){
	if(checkSnakeCollide(snake))return 1;
	if(snake->HEAD->posX < 0){
		puts("Snake x <= 0");	
		return 1;	
	}
	if(snake->HEAD->posX >= cellCount * cellSize){
		puts("Snake x too big!");
		return 1;
	}
	if(snake->HEAD->posY < 0){
		puts("Snake y <= 0");
		return 1;	
	}
	if(snake->HEAD->posY >= cellCount * cellSize){
		puts("Snake y too big!!");
		return 1;	
	}	

	return 0;
}

void initGame(Snake * snake, Apple * apple){

	for(int i = 0; i < STARTLENGHT; i++){
	       	addPart(STARTLENGHT * cellSize, (STARTLENGHT + i) *cellSize, snake);	
	
	}
	moveApple(snake, apple);	

	InitWindow(screenWidth, screenHeight, "Raylib Snake");		
	ClearBackground(WHITE);
	SetTargetFPS(10);
}

void resetGameVars(Apple * apple, Snake * snake, char * score, int * direction){
	freeSnake(snake);
	*snake = (Snake){NULL, *direction, 0};	
	for(int i = 0; i < STARTLENGHT; i++){
		addPart(STARTLENGHT * cellSize, (STARTLENGHT + i) * cellSize, snake);
	}
	*direction = RIGHT;
	moveApple(snake, apple);
	
}

void makeTheGrid(RenderTexture2D * target){


	BeginTextureMode(*target);
	ClearBackground(BLANK);
	for(int i = 0; i < cellCount + 1; i++){
		DrawLineEx((Vector2){cellSize * i, 0},(Vector2){cellSize * i, screenHeight}, 2, BLACK);
		DrawLineEx((Vector2){0, i*cellSize}, (Vector2){screenWidth, i*cellSize}, 2, BLACK);	
	}
	EndTextureMode();

}


int main(){	
	Apple apple = {};
	Snake snake = {NULL, RIGHT, 0};

	char score[20] = "";
	int direction = RIGHT;


	initGame(&snake, &apple);	

	//Making the grid
	RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);	//I made the grid in to a RenderTexture2D here, to reduce the amount of draw's per frame, drastically
	makeTheGrid(&target);


	while(!WindowShouldClose()){
		int key = GetKeyPressed();
		
		if((key == KEY_S || key == KEY_J) && direction != UP) direction = DOWN;
		else if((key == KEY_W || key == KEY_K) && direction != DOWN)direction = UP;
		else if((key == KEY_A || key == KEY_H) && direction != RIGHT)direction = LEFT;
		else if((key == KEY_D || key == KEY_L)  && direction != LEFT) direction = RIGHT;

		moveSnake(&snake, direction);
		if(checkFail(&snake)) resetGameVars(&apple, &snake, score, &direction);	
			

		//Collisions with apple
		if(checkCollision(&snake, &apple)){
			addPart(snake.HEAD->posY, snake.HEAD->posX, &snake);
			moveApple(&snake, &apple);
		}	

		BeginDrawing();	
		

		DrawRectangle(cellSize * apple.posX, cellSize * apple.posY, cellSize, cellSize, RED);
		drawSnake(&snake);

		DrawTextureRec(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float) -target.texture.height}, (Vector2){0,0}, WHITE);
		
		sprintf(score, "%d", (snake.score - STARTLENGHT));
		DrawText(score, (screenWidth * .8), 50, 100, BLACK);

		ClearBackground(WHITE);	

		EndDrawing();	
	}
	UnloadRenderTexture(target);
	freeSnake(&snake);
	CloseWindow();	
	return 0;
}
