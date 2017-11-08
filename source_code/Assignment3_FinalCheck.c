/*
	PROGRAM DESCRIPTION
	Name:
		Assignment 3 (MINESWEEPER)
	Description:
		This program demonstates the minesweeper game from Microsoft
	Note:
		When this program defined BONUS1, it must has at least one argument at the time it is called
		TESTMODE could be activated by using this statement #include TESTMODE
*/

#include <stdio.h>
#include <stdlib.h>

#define MINE 0 //location of MINE bit in status_byte
#define FLAG 1 //location of FLAG bit in status_byte
#define REVEALED 2 //location of REVEALED bit in status_byte

//#define TESTMODE

#define BONUS1 //write move from users to file
#define BONUS3 //colourful
#define BONUS2 //clean the screen for better view

#ifdef BONUS2
	/*FUNCTION FOR CURSOR IN BONUS 2*/
	#define clearScreen() printf("\033[0J") //clear screen from the top to the bottom
	#define moveToTopLeftCorner() printf("\033[H"); //move cursor to the top left corner
#endif

#ifdef BONUS3
	/*COLOUR FOR VIEWING IN TERMINAL*/
	#define GREY_B "\x1b[30;1m"
	#define RED_B "\x1b[31;1m"
	#define BLUE_B "\x1b[34;1m"
	#define GREEN_B "\x1b[32;1m"
	#define MAGENTA_B "\x1b[35;1m"
	#define MAGENTA "\x1b[35m"
	#define YELLOW "\x1b[33m"
	#define YELLOW_B "\x1b[33;1m"
	#define CYAN_B "\x1b[36;1m"
	#define RESET "\x1b[0m"
#endif

/*STRUCT DECLARATION*/
struct Location
{
	int value;
	char status_byte;
};

struct Game
{
	int width;
	int height;
	int mines;
	int mines_flagged;
	int flags;
};

/*FUNCTION DECLARATION*/
int getWidthOfTheBoard();
int getHeightOfTheBoard();
int getNumberofMine(int widthOfTheBoard, int heightOfTheBoard);

#ifdef TESTMODE
void testGameBoard(struct Game game, struct Location** mine_board);
void testPrintPlayingField(struct Game game, struct Location** mine_board);
#endif

void initialize_mine_board(struct Game game, struct Location** mine_board);
void initialize_mine_value(struct Game game, struct Location** mine_board);

void make_move(struct Game game, struct Location** mine_board, int x, int y);
void get_move(int* x_ptr, int* y_ptr, int* flag_ptr, int max_x, int max_y);
void print_mine_board(struct Game game, struct Location** mine_board);
void uncover_zeros(struct Location** mine_board, int x, int y);

#ifdef BONUS3
	char* colour(int input);
#endif

/*GLOBAL VARIABLE*/
int statusOfPlayer = 1; //the player is still alive

#ifdef BONUS1
	FILE* file_ptr; //open file for writing
#endif

/*MAIN FUNCTION*/
int main(int argc, char const *argv[])
{

	printf("\n\n\n*** 18869804 VU PHUC THANH TRAN ASSIGNMENT 3 ***\n\n\n");

	#ifdef	BONUS3
		printf(RED_B "\n\n\n*** 18869804 VU PHUC THANH TRAN ASSIGNMENT 3 ***\n\n\n" RESET);
	#endif

	/*VARIABLE DECLARATION*/
	struct Game game;
	game.mines_flagged = 0;
	game.flags = 0;

	struct Location** mine_board;
	int i; //using in loop

	int x, y; //coordinates in game board
	int flag_status; //is it a flag or not?

	#ifdef BONUS1
		/*CHECKING ARGUMENTS OF FUNCTION MAIN*/
		if(2 != argc)
		{
			printf("One argument is expected.\n");
			return 1;
		}
	#endif

	#ifdef BONUS1
		/*OPENING THE FILE*/
		file_ptr = fopen(argv[1], "w");
		if(NULL == file_ptr)
		{
			printf("Can not open the file.\n");
			return 1;
		}
	#endif

	/*USER INPUT FOR WIDTH OF THE BOARD*/
	game.width = getWidthOfTheBoard();

	/*USER INPUT FOR HEIGHT OF THE BOARD*/
	game.height = getHeightOfTheBoard();

	/*USER INPUT FOR NUMBER OF MINE*/
	game.mines = getNumberofMine(game.width, game.height);

	#ifdef BONUS2
		 moveToTopLeftCorner();
		 clearScreen();
	#endif

	#ifdef BONUS1
		/*WRITE WIDTH, HEIGHT AND NUMBER OF MINE INTO FILE*/
		fprintf(file_ptr, "%d\n", game.width);
		fprintf(file_ptr, "%d\n", game.height);
		fprintf(file_ptr, "%d\n", game.mines);
	#endif

	/*TESTING USER INPUT*/
	#ifdef TESTMODE
	printf("%d and %d and %d\n", game.width, game.height, game.mines);
	#endif

	/*CREATING BOARD GAME*/
	//row
	mine_board = (struct Location**)malloc((game.height + 2) * sizeof(struct Location));
	if(NULL == mine_board)
	{
		printf("Error of malloc() function\n");
		return 1;
	}
	//column
	for(i = 0; i < (game.height + 2); i++)
	{
		mine_board[i] = (struct Location*)malloc((game.width + 2) * sizeof(struct Location));
		if(NULL == mine_board[i])
		{
			printf("Error of malloc() function\n");
			return 1;
		}
	}

	/*CREATING GAME FIELD*/
	initialize_mine_board(game, mine_board);

	/*SETTING GAME FIELD*/
	initialize_mine_value(game, mine_board);

	/*PRINTING THE FIELD AT THE FIRST TIME*/
	print_mine_board(game, mine_board);

	/*PLAYING GAME*/
	//if the player is still alive, mine_flagged is not equal to mines and mines are not equal to flags --> continue to play (else: printing out the result base on status of the player)
	while((1 == statusOfPlayer) && (game.mines != game.mines_flagged) && (game.mines != game.flags))
	{
		//get coordinates from users
		get_move(&x, &y, &flag_status, game.height, game.width);
		//check if this location is uncovered or not || already has flag and flag_status is not equal to 1 --> get coordinates from the user again
		if(mine_board[x][y].status_byte & (1 << REVEALED) || ((mine_board[x][y].status_byte & (1 << FLAG)) && (1 != flag_status)))
		{
			printf("This location has been uncovered or already has a flag\n");
		}
		else
		{
			//if do not set flag --> call make_move() function and printing out updated mine field
			if(0 == flag_status)
			{
				make_move(game, mine_board, x, y);
				print_mine_board(game, mine_board);
			}
			else
			{
				if(mine_board[x][y].status_byte & (1 << FLAG))
				{
					mine_board[x][y].status_byte &= ~(1 << FLAG);
					game.flags--;
					if(mine_board[x][y].status_byte & (1 << MINE))
					{
						game.mines_flagged--;
					}
					print_mine_board(game, mine_board);
				}
				else
				{
					mine_board[x][y].status_byte |= (1 << FLAG);
					game.flags++;
					if(mine_board[x][y].status_byte & (1 << MINE))
					{
						game.mines_flagged++;
					}
					print_mine_board(game, mine_board);
				}
			}
		}
	}
	/*FINISHING PLAYING GAME*/

	/*ANNOUNCEMENT AFTER FINISH GAME*/
	if(1 == statusOfPlayer)
	{
		printf("****************************************************************************\n");
		printf("*WE HAVE THE WINNER HERE. GOOD LUCK TO YOU NEXT TIME. YOU ARE GONNA NEED IT*\n");
		printf("****************************************************************************\n\n");
	}
	else
	{
		printf("*******************************\n");
		printf("*NOT LUCKY! DEADMAN IS WALKING*\n");
		printf("*******************************\n\n");
	}

	/*FREE ARRAYS ARE CREATED BY MALLOC FUNCTION*/
	for(i = 0; i < (game.height + 2); i++)
	{
		free(mine_board[i]);
	}
	free(mine_board);

	#ifdef BONUS1
		/*CLOSING THE FILE*/
		fclose(file_ptr);
	#endif

	return 0;
}

/*FUNCTION IMPLEMENTATION*/

/*
	Name:
		getWidthOfTheBoard
	Description:
		get width of board game from users
	Note:
		N/A
	Parameters:
		N/A
	Returns:
		value of width of board game in int
*/
int getWidthOfTheBoard()
{
	int check; //validating return value from scanf() function
	int widthOfTheBoard; //variable for storing input (width of the board) from user
	do
	{
		printf("Enter the width of the board: ");
		check = scanf("%d", &widthOfTheBoard);
		while(getchar() != '\n');
		if(0 == check || 0 > widthOfTheBoard)
		{
			printf("Invalid Input\n");
		}
	}
	while(0 == check || 0 > widthOfTheBoard);
	#ifdef TESTMODE
	printf("Width of the board = %d\n", widthOfTheBoard);
	#endif
	return widthOfTheBoard;
}

/*
	Name:
		getHeightOfTheBoard
	Description:
		get height of the board from users
	Note:
		N/A
	Parameters:
		N/A
	Returns:
		value of height of the board in int
*/
int getHeightOfTheBoard()
{
	int check; //validating return value from scanf() function
	int heightOfTheBoard; //variable for storing input (height of the board) from user
	do
	{
		printf("Enter the height of the board: ");
		check = scanf("%d", &heightOfTheBoard);
		while(getchar() != '\n');
		if(0 == check || 0 > heightOfTheBoard)
		{
			printf("Invalid Input\n");
		}
	}
	while(0 == check || 0 > heightOfTheBoard);
	#ifdef TESTMODE
	printf("Height of the board = %d\n", heightOfTheBoard);
	#endif
	return heightOfTheBoard;
}

/*
	Name:
		getNumberofMine
	Description:
		get value of number of mine from users
	Note:
		N/A
	Parameters:
		int widthOfTheBoard			value of width of board game
		int heightOfTheBoard		value of height of board game
	Returns:
		value of number of mine of board game from users
*/
int getNumberofMine(int widthOfTheBoard, int heightOfTheBoard)
{
	int check; //validating return value from scanf() function
	int numberOfMine; //variable for storing input (height of the board) from user
	int temp = widthOfTheBoard * heightOfTheBoard;
	do
	{
		printf("Enter the number of mine: ");
		check = scanf("%d", &numberOfMine);
		while(getchar() != '\n');
		if(0 == check || 0 >= numberOfMine || temp < numberOfMine)
		{
			printf("Invalid Input\n");
		}
	}
	while(0 == check || 0 >= numberOfMine || temp < numberOfMine);
	#ifdef TESTMODE
	printf("Number of mine = %d\n", numberOfMine);
	#endif
	return numberOfMine;
}

/*
	Name:
		initialize_mine_board
	Description:
		this function will set mines to board game rondomly base on number of mine from users
	Note:
		N/A
	Parameters:
		struct Game game								value of struct Game game (which is initialized in the beginning of the program and storing values from users)
		struct Location** mine_board    address of variable mine_board which is initialized in the beginning (creating two dimensions array using malloc())
	Returns:
		N/A
*/
void initialize_mine_board(struct Game game, struct Location** mine_board)
{
	int i; //using in loop
	int j; //using in loop
	int temp_mines = game.mines; //using for setting mines in game board
	int x; //using as coorinate (height)
	int y; //using as coordinate (width)
	for(i = 0; i <= (game.height + 1); i++)
	{
		for(j = 0; j <= (game.width + 1); j++)
		{
			mine_board[i][j].status_byte = 0;
		}
	}
	while(0 < temp_mines)
	{
		x = rand() % game.height + 1;
		y = rand() % game.width + 1;
		if(!(mine_board[x][y].status_byte & (1 << MINE)))
		{
			mine_board[x][y].status_byte = mine_board[x][y].status_byte | (1 << MINE);
			#ifdef TESTMODE
				printf("Mine has been set at %d %d\n", x, y);
			#endif
			temp_mines--; //one mine has been set
		}
	}
}

/*
	Name:
		initialize_mine_value
	Description:
		this function will set value of boxes around mines in board game via pointer named mine_board
	Note:
		N/A
	Parameters:
		struct Game game								value of struct Game game (which is initialized in the beginning of the program and storing values from users)
		struct Location** mine_board    address of variable mine_board which is initialized in the beginning (creating two dimensions array using malloc())
	Returns:
		N/A
*/
void initialize_mine_value(struct Game game, struct Location** mine_board)
{
	int i, j; //using in loop
	for(i = 0; i <= (game.height + 1); i++)
	{
		for(j = 0; j <= (game.width + 1); j++)
		{
			//setting border for game board
			if(0 == i || 0 == j || (game.height + 1) == i || (game.width + 1) == j)
			{
				mine_board[i][j].value = 0;
				mine_board[i][j].status_byte = mine_board[i][j].status_byte | (1 << REVEALED);
			}
			//setting value for each box based on mines around it
			else
			{
				int loop; //using in loop
				int count = 0;
				int height_location[] = {i-1, i-1, i-1, i, i, i+1, i+1, i+1};
				int width_location[] = {j-1, j, j+1, j-1, j+1, j-1, j, j+1};
				//check if this location is a mine or not --> if it is a mine, skip it
				if(!(mine_board[i][j].status_byte & (1 << MINE)))
				{
					for(loop = 0; loop < 8; loop++)
					{
						if((mine_board[height_location[loop]][width_location[loop]].status_byte & (1 << MINE)))
						{
							count += 1;
						}
					}
				}
				mine_board[i][j].value = count;
				#ifdef TESTMODE
				printf("Value at this position %d %d is %d\n", i, j, count);
				#endif
			}
		}
	}
}

#ifdef TESTMODE
/*
	Name:
		testGameBoard
	Description:
		this function will set all values of elements in 2 dimensions array to 1 and printing to test
	Note:
		This function is a test function which operates in TESTMODE and does not run in real playing of users
	Parameters:
		struct Game game								value of struct Game game (which is initialized in the beginning of the program and storing values from users)
		struct Location** mine_board    address of variable mine_board which is initialized in the beginning (creating two dimensions array using malloc())
	Returns:
		N/A
*/
void testGameBoard(struct Game game, struct Location** mine_board)
{
	int i, j; //using in loop
	for(i = 0; i < (game.height + 2); i++)
	{
		for(j = 0; j < (game.width + 2); j++)
		{
			mine_board[i][j].value = 1;
		}
	}
	for(i = 0; i < (game.height + 2); i++)
	{
		for(j = 0; j < (game.width + 2); j++)
		{
			printf(" %d ", mine_board[i][j].value);
		}
		printf("\n");
	}
}
#endif

/*
	Name:
		print_mine_board
	Description:
		this function will print game board (at the beginning of the game) and (after every step from users)
	Note:
		N/A
	Parameters:
		struct Game game								value of struct Game game (which is initialized in the beginning of the program and storing values from users)
		struct Location** mine_board    address of variable mine_board which is initialized in the beginning (creating two dimensions array using malloc())
	Returns:
		N/A
*/
void print_mine_board(struct Game game, struct Location** mine_board)
{
	int i, j; //using in loop
	char temp; //using in printf for printing in exact format

	#ifdef BONUS2
		moveToTopLeftCorner();
	#endif

	printf("\n\n");

	for(i = 0; i <= (game.height); i++)
	{
		for(j = 0; j <= (game.width); j++)
		{
			if(0 == i && 0 == j)
			{
				printf("   ");
			}
			else if(0 == i)
			{
				printf("%-3d", j);
			}
			else if(0 == j)
			{
				printf("%-3d", i);
			}
			else
			{
				if((mine_board[i][j].status_byte & (1 << FLAG)))
				{
					temp = 'F';
					#ifndef BONUS3
						printf("%-3c", temp);
					#else
						printf(GREY_B "%-3c" RESET, temp);
					#endif
					//printf("%-3c", temp);
				}
				else if((mine_board[i][j].status_byte & (1 << REVEALED)))
				{
					if((mine_board[i][j].status_byte & (1 << MINE)))
					{
						temp = 'M';
						#ifndef BONUS3
							printf("%-3c", temp);
						#else
							printf(RED_B "%-3c" RESET, temp);
						#endif
					}
					else if(0 == mine_board[i][j].value)
					{
						temp = '-';
						#ifndef BONUS3
							printf("%-3c", temp);
						#else
							printf(GREY_B "%-3c" RESET, temp);
						#endif
					}
					else
					{
						#ifndef BONUS3
							printf("%-3d", mine_board[i][j].value);
						#else
							printf("%s""%-3d""%s", colour(mine_board[i][j].value), mine_board[i][j].value, RESET);
						#endif
					}
				}
				else
				{
					temp = '#';
					printf("%-3c", temp);
				}
			}
		}
		printf("\n");
	}

	printf("\n\n");

	#ifdef BONUS2
		clearScreen();
	#endif

	#ifdef TESTMODE
		testPrintPlayingField(game, mine_board);
	#endif
}

#ifdef TESTMODE
/*
	Name:
		testPrintPlayingField
	Description:
		this function will print test game board which including location of mines and values of every box in game board
	Note:
		N/A
	Parameters:
		struct Game game								value of struct Game game (which is initialized in the beginning of the program and storing values from users)
		struct Location** mine_board    address of variable mine_board which is initialized in the beginning (creating two dimensions array using malloc())
	Returns:
		N/A
*/
void testPrintPlayingField(struct Game game, struct Location** mine_board)
{
	int i, j; //using in loop

	for(i = 1; i <= (game.height); i++)
	{
		for(j = 1; j <= (game.width); j++)
		{
			printf("%d|%-3d", mine_board[i][j].value, mine_board[i][j].status_byte);
		}
		printf("\n");
	}

	for(i = 0; i <= (game.height); i++)
	{
		for(j = 0; j <= (game.width); j++)
		{
			if(0 == i && 0 == j)
			{
				printf("   ");
			}
			else if(0 == i)
			{
				printf("%-3d", j);
			}
			else if(0 == j)
			{
				printf("%-3d", i);
			}
			else
			{
				if((mine_board[i][j].status_byte & (1 << FLAG)))
				{
					char temp = 'F';
					printf("%-3c", temp);
				}
				else if((mine_board[i][j].status_byte & (1 << REVEALED)))
				{
					if((mine_board[i][j].status_byte & (1 << MINE)))
					{
						char temp = 'M';
						printf("%-3c", temp);
					}
					else if(0 == mine_board[i][j].value)
					{
						char temp = '-';
						printf("%-3c", temp);
					}
					else
					{
						printf("%-3d", mine_board[i][j].value);
					}
				}
				else
				{
					char temp = '#';
					printf("%-3c", temp);
				}
			}
		}
		printf("\n");
	}
}
#endif

/*
	Name:
		make_move
	Description:
		this function will change status bit of box in game board base on coordinates from users
	Note:
		N/A
	Parameters:
		struct Game game								value of struct Game game (which is initialized in the beginning of the program and storing values from users)
		struct Location** mine_board    address of variable mine_board which is initialized in the beginning (creating two dimensions array using malloc())
	Returns:
		N/A
*/
void make_move(struct Game game, struct Location** mine_board, int x, int y)
{
	mine_board[x][y].status_byte = mine_board[x][y].status_byte | (1 << REVEALED);	//open this location to revealed
	#ifdef TESTMODE
	printf("Just reveal position at %d %d\n", x, y);
	#endif
	//if this location contains a bomb
	if(mine_board[x][y].status_byte & (1 << MINE))
	{
		statusOfPlayer = 0; //game over //deadman is walking
		#ifdef TESTMODE
		printf("Just jump on the mine at %d %d\n", x, y);
		#endif
	}
	else
	{
		//if already has a flag, remove that flag
		if(mine_board[x][y].status_byte & (1 << FLAG))
		{
			mine_board[x][y].status_byte = mine_board[x][y].status_byte & ~(1 << FLAG);
			#ifdef TESTMODE
			printf("Just removing flag at position %d %d\n", x, y);
			#endif
		}
		//if value of that box is zero, then call uncover_zeros() function
		else if(0 == mine_board[x][y].value)
		{
			uncover_zeros(mine_board, x, y);
			#ifdef TESTMODE
			printf("Calling uncover_zeros() function %d %d\n", x, y);
			#endif
		}
	}
}

/*
	Name:
		get_move
	Description:
		this function will display requirement statement to ask users to enter inputs for x, y (coordinates) and flag
	Note:
		N/A
	Parameters:
		int* x_ptr				address of variable which is storing value of coordinate x
		int* y_ptr				address of variable which is storing value of coordinate y
		int* flag_ptr			address of variable which is storing value of flag
		int max_x					max value of coordinate x
		int max_y					max value of coordinate y
	Returns:
		N/A
*/
void get_move(int* x_ptr, int* y_ptr, int* flag_ptr, int max_x, int max_y)
{
	int check; //for validating return result from scanf() function
	//user input for coordinates and flag
	do
	{
		printf("Enter the x (1 - %d) and y (1 - %d) (coordinates) values and flag (1/0): ", max_x, max_y);
		check = scanf("%d %d %d", x_ptr, y_ptr, flag_ptr);
		while(getchar() != '\n');
		if(0 == check)
		{
			printf("Invalid input, please enter again!\n");
		}
		else if(1 > *x_ptr || max_x < *x_ptr || 1 > *y_ptr || max_y < *y_ptr)
		{
			printf("Invalid coordinate, please enter again\n");
		}
		else if((0 != *flag_ptr) && (1 != *flag_ptr))
		{
			printf("Invalid flag status, please enter again\n");
		}
	}
	while(0 == check || ((0 != *flag_ptr) && (1 != *flag_ptr)) || 1 > *x_ptr || max_x < *x_ptr || 1 > *y_ptr || max_y < *y_ptr);

	#ifdef TESTMODE
	printf("%d %d %d\n", *x_ptr, *y_ptr, *flag_ptr);
	#endif

	#ifdef BONUS1
		fprintf(file_ptr, "%d %d %d\n", *x_ptr, *y_ptr, *flag_ptr);
	#endif
}

/*
	Name:
		uncover_zeros
	Description:
		this function will set REVEALED bit for boxes which are have value is zero recursively
	Note:
		N/A
	Parameters:
		struct Location** mine_board			value of struct Game game (which is initialized in the beginning of the program and storing values from users)
		int x															start coordinate x
		int y															start coordinate y
	Returns:
		N/A
*/
void uncover_zeros(struct Location** mine_board, int x, int y)
{
	int height_coordinates[] = {x-1, x-1, x-1, x, x, x+1, x+1, x+1};
	int width_coordinates[] = {y-1, y, y+1, y-1, y+1, y-1, y, y+1};

	int i;
	for(i = 0; i < 8; i++)
	{
		if(!(mine_board[height_coordinates[i]][width_coordinates[i]].status_byte & (1 << REVEALED)) && !(mine_board[height_coordinates[i]][width_coordinates[i]].status_byte & (1 << FLAG)))
		{
			mine_board[height_coordinates[i]][width_coordinates[i]].status_byte = mine_board[height_coordinates[i]][width_coordinates[i]].status_byte | (1 << REVEALED);
			if(0 == mine_board[height_coordinates[i]][width_coordinates[i]].value)
			{
				uncover_zeros(mine_board, height_coordinates[i], width_coordinates[i]);
			}
		}
	}

	#ifdef TESTMODE
	//checking coordinates
	for(i = 0; i < 8; i++)
	{
		printf("Coordinates: %d %d\n", height_coordinates[i], width_coordinates[i]);
	}
	#endif
}

#ifdef BONUS3
/*
	Name:
		colour
	Description:
		this function will set colour for each boxes base on value of that box
	Note:
		N/A
	Parameters:
		int input		value of that box
	Returns:
		code of colour as a string (char*)
*/
	char* colour(int input)
	{
		if(1 == input)
		{
			return BLUE_B;
		}
		else if(2 == input)
		{
			return GREEN_B;
		}
		else if(3 == input)
		{
			return RED_B;
		}
		else if(4 == input)
		{
			return MAGENTA;
		}
		else if(5 == input)
		{
			return YELLOW;
		}
		else if(6 == input)
		{
			return CYAN_B;
		}
		else if(7 == input)
		{
			return MAGENTA_B;
		}
		return YELLOW_B;
	}
#endif
