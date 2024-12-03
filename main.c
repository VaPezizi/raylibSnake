#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//Some variables
const int screenWidth = 800;
const int screenHeight = 800;

const int cellCount = 20;
const int cellSize = screenWidth / cellCount; //800 / 40,
const int STARTLENGHT = 5;

int colormode = 0;

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

//Safe wrapper for malloc, maybe useless but idc
void * safeMalloc(size_t n){
	void *p = malloc(n);
	if(p==NULL){
		fprintf(stderr, "Failed to allocate %zu bytes!\n", n);
		abort();
	}
	return p;
}

//Doesn't move the snake itself, but changes the position of nodes in the "Snake"	(The linked list)
void moveNode(Snake * snake){
	if(snake->HEAD == NULL){
		fprintf(stderr, "Error in snake moving, snake head NULL!");
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
//Checks if given coordinates are inside the snake
int newAppleInsideSnake(Snake * snake, int * rx, int * ry){
	
	SnakePart * part = snake->HEAD;	
	
	while(part != NULL){
		//puts("ALOO");
		printf("PosX: %d\nPosY: %d\nrx*cellSize: %d\nry*cellSize:%d\n", part->posX, part->posY, *rx * cellSize, *ry * cellSize);
		if(part->posX == *rx * cellSize && part->posY == *ry * cellSize)return 1; 
		part = part->next;	
	}
	puts("----------------------");
	return 0;
}

//Moves the apple to a new random location
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

//TODO: Optimize this, i'm no expert, but i'd guess checking the collision this way is slower than it has to
int checkCollision(Snake * snake, Apple * apple){
	return CheckCollisionPointRec((Vector2){snake->HEAD->posX, snake->HEAD->posY}, (Rectangle){apple->posX * cellSize, apple->posY * cellSize, cellSize, cellSize});
}

//This actually moves the snake itself, calls moveNode first, and then just updates the position of the snakes HEAD node
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
//Pretty simple, adds a part to the snake, with given coordinates
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
//Also pretty self explanitory, draws the snake. BeginDrawing(), or something similar has to be called before this
void drawSnake(Snake * snake){
	if(snake->HEAD == NULL){
		
		fprintf(stderr, "Snake not initialized!!");
		abort();
	}
	else{
		int n = 0;
		SnakePart * temp = snake->HEAD;
		if(colormode){
			while(temp->next != NULL){
				if(n >= 340) n = 0;
				else n = n + 10;
				DrawRectangle(temp->posX, temp->posY, cellSize, cellSize, ColorFromHSV(n, 1, 1));	//Draws the snake parts with a color based of the index (-rainbow)
				temp = temp->next;
			}
			DrawRectangle(temp->posX, temp->posY, cellSize, cellSize, ColorFromHSV(n, 1, 1));
		}else{

			while(temp->next != NULL){
				DrawRectangle(temp->posX, temp->posY, cellSize, cellSize, GREEN);
				temp = temp->next;
			}
			DrawRectangle(temp->posX, temp->posY, cellSize, cellSize, GREEN);
		}

	}
}

//Frees the memory occupied by the snake
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

//Checks if the snake is colloding with itself	//TODO: I go loop trough the snake so much, check for optimisations
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

//Checks that the snake is inbounds, and collision with itself
int checkFail(Snake * snake){

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
	if(checkSnakeCollide(snake))return 1;
	return 0;
}

//Makes the grid for the game
void makeTheGrid(RenderTexture2D * target){

	BeginTextureMode(*target);
	ClearBackground(BLANK);
	for(int i = 0; i < cellCount + 1; i++){
		DrawLineEx((Vector2){cellSize * i, 0},(Vector2){cellSize * i, screenHeight}, 2, BLACK);
		DrawLineEx((Vector2){0, i*cellSize}, (Vector2){screenWidth, i*cellSize}, 2, BLACK);	
	}
	EndTextureMode();

}
// Inits the game (Makes the snake, apple (only kind of) and some small stuff)
void initGame(Snake * snake, Apple * apple){

	for(int i = 0; i < STARTLENGHT; i++){
	       	addPart(STARTLENGHT * cellSize, (STARTLENGHT + i) *cellSize, snake);	
	
	}
	moveApple(snake, apple);	

	InitWindow(screenWidth, screenHeight, "Raylib Snake");		
	ClearBackground(WHITE);
	SetTargetFPS(10);

}

//Resets the arguments to their default states
void resetGameVars(Apple * apple, Snake * snake, char * score, int * direction){
	freeSnake(snake);
	*snake = (Snake){NULL, *direction, 0};	
	for(int i = 0; i < STARTLENGHT; i++){
		addPart(STARTLENGHT * cellSize, (STARTLENGHT + i) * cellSize, snake);
	}
	*direction = RIGHT;
	moveApple(snake, apple);
	
}



int main(int argc, char * argv[]){	

	if(argc > 1){
		if(strcmp(argv[1], "-h") == 0){
			printf("Usage:\n-h\tPrints this menu\n-rainbow\tStarts the game in rainbow snake mode\n");	
			return 0;
		}
		else if(strcmp(argv[1], "-rainbow") == 0){
			colormode = 1;		
		}
	}

	Apple apple = {};
	Snake snake = {NULL, RIGHT, 0};


	char score[20] = "";
	int direction = RIGHT;


	initGame(&snake, &apple);	

	RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);	//I made the grid in to a RenderTexture2D here, to reduce the amount of draw's per frame, drastically
	makeTheGrid(&target);

//	char fps[10] = "";	

	while(!WindowShouldClose()){
		int key = GetKeyPressed();
		sprintf(score, "%d", (snake.score - STARTLENGHT));
//		sprintf(fps, "%d", GetFPS());

		while(key > 0){		//This should make the inputs more reliable
			//fprintf(stdout, "" + key);	
			if((key == KEY_S || key == KEY_J) && direction != UP) direction = DOWN;
			if((key == KEY_W || key == KEY_K) && direction != DOWN)direction = UP;
			if((key == KEY_A || key == KEY_H) && direction != RIGHT)direction = LEFT;
			if((key == KEY_D || key == KEY_L)  && direction != LEFT) direction = RIGHT;

			key = GetCharPressed();
		}

		moveSnake(&snake, direction);
		if(checkFail(&snake)) resetGameVars(&apple, &snake, score, &direction);	
			

		//Collisions with apple
		if(checkCollision(&snake, &apple)){
			addPart(snake.HEAD->posY, snake.HEAD->posX, &snake);
			moveApple(&snake, &apple);
		}	


	//======== ( DRAWING ) ========


		BeginDrawing();	
		

		DrawRectangle(cellSize * apple.posX, cellSize * apple.posY, cellSize, cellSize, RED);
		drawSnake(&snake);

		DrawTextureRec(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float) -target.texture.height}, (Vector2){0,0}, WHITE);
		

		DrawText(score, (screenWidth * .8), 50, 100, BLACK);
		
//		DrawText(fps, 700, 700, 40, GREEN);

		ClearBackground(WHITE);	

		EndDrawing();	
	}
	UnloadRenderTexture(target);
	freeSnake(&snake);
	CloseWindow();	
	return 0;
}
