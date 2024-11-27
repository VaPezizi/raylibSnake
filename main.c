#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

const int screenWidth = 800;
const int screenHeight = 800;

const int cellCount = 40;
const int cellSize = 20;

//Structs

typedef struct SnakePart{
	int posX;
	int posY;
	struct SnakePart * next;	
}SnakePart;

typedef struct{
	SnakePart * HEAD;
	int size;
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

void addPart(const int posY, const int posX, Snake * snake){
	if(snake->HEAD == NULL){
		snake->HEAD = malloc(sizeof(SnakePart));
		snake->HEAD->next = NULL;
		snake->HEAD->posX = posX;
		snake->HEAD->posY = posY;
	}else{
		SnakePart * temp = snake->HEAD;
		while(temp->next != NULL){
			temp = temp->next;
		}
		snake->score++;
		temp->next = safeMalloc(sizeof(SnakePart));
		temp->next->next = NULL;
		temp->next->posX = posX;
		temp->next->posY = posY;
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
	Snake snake = {NULL, cellSize, 0};
	for(int i = 0; i < 10; i++){
	       	addPart(20 * cellSize, (20 - i) *cellSize, &snake);	
	
	}
	InitWindow(screenWidth, screenHeight, "Raylib Snake");		
	ClearBackground(WHITE);
	SetTargetFPS(60);
	
	while(!WindowShouldClose()){
		BeginDrawing();
		
		for(int i = 0; i < 41;i++){
			drawSnake(&snake);	
			DrawLineEx((Vector2){cellSize * i, 0},(Vector2){cellSize * i, screenHeight}, 2, BLACK);
			DrawLineEx((Vector2){0, i*cellSize}, (Vector2){screenWidth, i*cellSize}, 2, BLACK);	
		}

		ClearBackground(WHITE);	

		EndDrawing();	
	}
	freeSnake(&snake);	
	return 0;
}
