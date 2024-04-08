#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

#define KEY_SPACE ' ' // not defined in ncurses.h
#define KEY_Enter 10
#define START_ROW 5
#define START_COL 5

int HEIGHT, WIDTH;

void saveGame(int **board, int row, int col, int turn, int players){
	/*
		Save current game with given name 
	*/
	// TODO 
	char file[100];
	
	echo();
	WINDOW * winFin = newwin(1, 117, 7, 7);
	wprintw(winFin, "ENTER FILE NAME : ");
	refresh();
	wrefresh(winFin);
	wscanw(winFin, "%s", file);
	noecho();

	FILE * save = fopen(file, "w");
	fprintf(save, "%d %d %d %d %d %d\n", row, col, turn, players, HEIGHT, WIDTH);
	for (int i=0; i<HEIGHT; i++){
		for (int j=0; j<WIDTH; j++){
			fprintf(save, "%d\n", board[i][j]);
		}
	}
	fclose(save);
}

int** readSavedGame(int **board, int *row, int *col, int *turn, int *players){
	/*
		Read the existing game
	*/
	// TODO 
	WINDOW * winFile = newwin(1, 117, 0, 0);
	char file[100];

	echo();
	wprintw(winFile, "ENTER FILE NAME : ");
	refresh();
	wrefresh(winFile);
	wscanw(winFile, "%s", file);
	noecho();


	FILE * read = fopen(file, "r");
	fscanf(read, "%d %d %d %d %d %d", row, col, turn, players, &HEIGHT, &WIDTH);

	board = (int**)malloc(sizeof(int*)*HEIGHT);
	for(int i=0; i<HEIGHT; i++){
		board[i] = (int*)malloc(sizeof(int)*WIDTH);
	}

	for (int i=0; i<HEIGHT; i++){
		for (int j=0; j<WIDTH; j++){
			fscanf(read, "%d", board[i]+j);
		}
	}
	fclose(read);

	return board;
}

int** initBoard(int **board, int *row, int *col, int *turn, int load){
	board = (int**)malloc(sizeof(int*)*HEIGHT);
	for(int i=0; i<HEIGHT; i++){
		board[i] = (int*)malloc(sizeof(int)*WIDTH);
	}

	board[0][0] = ACS_ULCORNER;//'┌'
	for (int i=1; i < WIDTH-1; i++)
		board[0][i] = ACS_TTEE;//'┬'
	board[0][WIDTH-1] = ACS_URCORNER; //'┐'

	for (int i=1; i<HEIGHT-1; i++){
		board[i][0] = ACS_LTEE; // '├'
		for (int j=1; j < WIDTH-1; j++)
			board[i][j] = ACS_PLUS; //'┼'
		board[i][WIDTH-1] = ACS_RTEE; //'┤'
	}

	board[HEIGHT-1][0] = ACS_LLCORNER; //'└'
	for (int i=1; i < WIDTH-1; i++)
		board[HEIGHT-1][i] = ACS_BTEE; //'┴'
	board[HEIGHT-1][WIDTH-1] = ACS_LRCORNER; // '┘'

	return board;
}

void paintBoard(int **board, WINDOW *win, int row, int col){
	/*
		Print the board to the given WINDOW 
		using functions of the ncurses library.
	*/
	// TODO
	for (int i=0; i<HEIGHT; i++) {
		for (int j=0; j<WIDTH; j++) {
			mvwaddch(win, i, j, board[i][j]);
		}
	}
	wrefresh(win);
}

void paintMenu(WINDOW * winMenu, int turn, int isEnd){
	if (isEnd) {
		wclear(winMenu);
		mvwprintw(winMenu, 0, 0, "PLAYER%d WIN !! ", turn+1);
		wprintw(winMenu, "Press any button to terminate the program");
	}
	if (turn == 0) mvwprintw(winMenu, isEnd, 0, "Current Turn : O");
	else if (turn == 1) mvwprintw(winMenu, isEnd, 0, "Current Turn : X");
	else mvwprintw(winMenu, isEnd, 0, "Current Turn : Y");
	mvwprintw(winMenu, isEnd+1, 0, "1. Press 1 to save");
	mvwprintw(winMenu, isEnd+2, 0, "2. Exit without save");
	wrefresh(winMenu);
	if (isEnd) {
		getch();
	}
}

int checkWin(int **board, int row, int col, int turn, int players){ // parameters and return type can be modified with any form. 
	/*
		Check if the game is over. 
	*/
	// TODO
	int std = 7-players;
	int cnt = 0;
	char turnChar;
	if (turn == 0) turnChar = 'O';
	else if (turn == 1) turnChar = 'X';
	else turnChar = 'Y';

	int rowStart = (row-std-1 > 0) ? row-std-1 : 0;
	int rowEnd = (row+std < HEIGHT) ? row+std : HEIGHT;
	int colStart = (col-std-1 > 0) ? col-std-1 : 0;
	int colEnd = (col+std < WIDTH) ? col+std : WIDTH;

	for (int i=rowStart; i<rowEnd; i++){
		if (board[i][col] == turnChar) {
			cnt++; 
			if (cnt == std) return 1;
		} else cnt = 0;
	}

	cnt = 0;
	for (int j=colStart; j<colEnd; j++){
		if (board[row][j] == turnChar) {
			cnt++; 
			if (cnt == std) return 1;
		} else cnt = 0;
	}
	
	cnt = 0;
	int i = row, j = col;
	while((i>rowStart)&&(j>colStart)){
		i--;
		j--;
	}
	for (; (i<rowEnd)&&(j<colEnd); i++, j++){
		if (board[i][j] == turnChar) {
			cnt++; 
			if (cnt == std) return 1;
		} else cnt = 0;
	}

	cnt = 0;
	i = row; j = col;
	while((i>rowStart)&&(j<colEnd-1)){
		i--;
		j++;
	}
	for (; (i<rowEnd)&&(j>colStart-1); i++, j--){
		if (board[i][j] == turnChar) {
			cnt++; 
			if (cnt == std) return 1;
		} else cnt = 0;
	}

	return 0;
}

int Action(WINDOW *win, int **board, int keyin, int *row, int *col, int *turn, int players, int *end){
	/*
		following right after the keyboard input,
		perform a corresponding action. 
	*/
	// TODO
	switch (keyin) {
		case KEY_SPACE:
		case KEY_Enter:
			if (*turn == 0) board[*row][*col] = 'O';
			else if (*turn == 1) board[*row][*col] = 'X';
			else board[*row][*col] = 'Y';
			if (checkWin(board, *row, *col, *turn, players)) return 1;
			(*turn)++;
			*turn %= players;
			break;
		case KEY_LEFT:
			if (*col>0) (*col)--;
			break;
		case KEY_RIGHT:
			if (*col<WIDTH-1) (*col)++;
			break;
		case KEY_DOWN:
			if (*row<HEIGHT-1) (*row)++;
			break;
		case KEY_UP:
			if (*row>0) (*row)--;
			break;
		case '1':
			saveGame(board, *row, *col, *turn, players);
		case '2':
			*end = 1;
			break;
	}
	return 0;		
}

void gameStart(WINDOW *win, int load, int players){
	int **board;
	int row = 0;
	int col = 0;
	int keyin;
	int turn = 0;
	int isEnd = 0;
	int end = 0;

	wmove(win, row, col); 	

	if (load) board = readSavedGame(board, &row, &col, &turn, &players);
	else board = initBoard(board, &row, &col, &turn, load); // Initiating the board

	WINDOW * winMenu = newwin(4, 56, START_ROW+HEIGHT+1, START_COL+10);
	refresh();

	paintBoard(board, win, row, col);
	paintMenu(winMenu, turn, isEnd);
	move(START_ROW+row, START_COL+col);


	while(1){
		/* 
			This While loop constantly loops in order to 
			draw every frame of the WINDOW.
		*/

		keyin = getch(); // GET KEYBOARD INPUT
		isEnd = Action(win, board, keyin, &row, &col, &turn, players, &end); // DO ACTION ACCORDING TO THAT INPUT

		paintBoard(board, win, row, col); // PAINT THE BOARD & update WINDOW 
		paintMenu(winMenu, turn, isEnd); // PAINT MENU & update WINDOW 
		move(START_ROW+row, START_COL+col); // MOVE CURSOR TO THE LAST POINT

		if (isEnd||end) break;
	}

	return;
}

int main(){
	char load;
	int players = 2;
	
	// TODO 
	/*
		Prompts to ask options of the game
	*/
	printf("Want to load the game?[y/n] : ");
	scanf("%c", &load);
	load = (load == 'y') ? 1 : 0;

	if (!load) {
		printf("Enter the HEIGHT of the board : ");
		scanf("%d", &HEIGHT);
		printf("Enter the WIDTH of the board : ");
		scanf("%d", &WIDTH);
		printf("Enter the number of players[2/3] : ");
		scanf("%d", &players);
	}

	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	WINDOW * win = newwin(HEIGHT, WIDTH, START_ROW, START_COL); // define a window

	gameStart(win, load, players); 

	endwin(); // terminate the window safely so that the terminal settings can be restored safely as well.

	return 0;
}