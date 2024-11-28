#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

//Some variables
const int screenWidth = 800;
const int screenHeight = 800;

const int cellCount = 40;
const int cellSize = 20;


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
	int size;
	int direction;
	int score;
}Snake;

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
	return;
}



int main(){	
	Snake snake = {NULL, cellSize, RIGHT, 0};
	for(int i = 0; i < 5; i++){
	       	addPart(20 * cellSize, (20 - i) *cellSize, &snake);	
	
	}

	int direction = RIGHT;

	InitWindow(screenWidth, screenHeight, "Raylib Snake");		
	ClearBackground(WHITE);
	SetTargetFPS(10);
	
	while(!WindowShouldClose()){
		if(IsKeyPressed(KEY_S))direction = DOWN;
		else if(IsKeyPressed(KEY_W))direction = UP;
		else if(IsKeyPressed(KEY_A))direction = LEFT;
		else if(IsKeyPressed(KEY_D))direction = RIGHT;

		moveSnake(&snake, direction);

		BeginDrawing();	
		drawSnake(&snake);
		for(int i = 0; i < 41;i++){

			DrawLineEx((Vector2){cellSize * i, 0},(Vector2){cellSize * i, screenHeight}, 2, BLACK);
			DrawLineEx((Vector2){0, i*cellSize}, (Vector2){screenWidth, i*cellSize}, 2, BLACK);	
		}

		ClearBackground(WHITE);	

		EndDrawing();	
	}
	freeSnake(&snake);	
	return 0;
}
