
/*PROGRAM DESCRIPTION*/
/*THIS PROGRAM DEMONSTRATES GAME MINESWEEPER*/

#include <stdio.h>
#include <stdlib.h>

#define MINE 0 //location of MINE bit in status_byte
#define FLAG 1 //location of FLAG bit in status_byte
#define REVEALED 2 //location of REVEALED bit in status_byte

//#define TESTMODE

#define BONUS1
#define BONUS3
#define BONUS2

#ifdef BONUS2
	#define clearScreen() printf("\033[0J") //clear screen from the top to the bottom
	#define moveToTopLeftCorner() printf("\033[H");
#endif

#ifdef BONUS3
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
void testGameBoard(struct Game game, struct Location** mine_board);
void testPrintPlayingField(struct Game game, struct Location** mine_board);
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
int statusOfPlayer = 1;

#ifdef BONUS1
	FILE* file_ptr;
#endif

/*MAIN FUNCTION*/
int main(int argc, char const *argv[])
{
	
	printf("\n\n\n*** 18869804 VU PHUC THANH TRAN ASSIGNMENT 3 ***\n\n\n");

	#ifdef	BONUS3
		printf(GREY_B "\n\n\n*** 18869804 VU PHUC THANH TRAN ASSIGNMENT 3 ***\n\n\n" RESET);
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

	/*TESTING BOARD GAME*/
	//testGameBoard(game, mine_board);

	/*CREATING GAME FIELD*/
	initialize_mine_board(game, mine_board);

	/*SETTING GAME FIELD*/
	initialize_mine_value(game, mine_board);

	/*PRINTING THE FIELD AT THE FIRST TIME*/
	print_mine_board(game, mine_board);

	/*TESTING GAME FIELD*/
	//print_mine_board(game, mine_board);
	//testPrintPlayingField(game, mine_board);
	//printf("%d %d %d %d %d\n", game.height, game.width, game.mines, game.mines_flagged, game.flags);

	/*TESTING GET MOVE FROM USERS*/
	//get_move(&x, &y, &flag_status, game.height, game.width);
	//printf("%d %d %d\n", x, y, flag_status);

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
	Function getWidthOfTheBoard() will print instruction and get width of the board from the user
	Input: N/A
	Output: return the value which is entered from user for width of the board
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
	Function getHeightOfTheBoard() will print instruction and get height of the board from the user
	Input: N/A
	Output: return the value which is entered from user for height of the board
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
	Function getNumberofMine() will print instruction and get number of mine from the user
	Input: N/A
	Output: return the value which is entered from user for number of mine
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
	initialize_mine_board() function has a main purpose is setting mine in the game field
	Inputs:
	1 - struct Game for information of the game
	2 - a pointer which pointer to a two dimension array (the game)
	Output:
	This function will create a clean game field, then set mines to random location based on number of mine from users
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
	initialize_mine_value() function will set border for the game, and creating number for each box on the field based on mines around it
	Inputs:
	1 - struct Game for information about width and height
	2 - a pointer which points to two dimensions array (mine_board)
	Output:
	Game board will be set and is ready for users to play
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
				
				//check if this location is a mine or not --> if it is a mine, skip it
				/*
				if(!(mine_board[i][j].status_byte & (1 << MINE)))
				{
					//location1
					if((mine_board[i-1][j-1].status_byte & (1 << MINE)))
					{
						count += 1;
					}
					//location2
					if((mine_board[i-1][j].status_byte & (1 << MINE)))
					{
						count += 1;
					}
					//location3
					if((mine_board[i-1][j+1].status_byte & (1 << MINE)))
					{
						count += 1;
					}
					//location4
					if((mine_board[i][j-1].status_byte & (1 << MINE)))
					{
						count += 1;
					}
					//location5
					if((mine_board[i][j+1].status_byte & (1 << MINE)))
					{
						count += 1;
					}
					//location6
					if((mine_board[i+1][j-1].status_byte & (1 << MINE)))
					{
						count += 1;
					}
					//location7
					if((mine_board[i+1][j].status_byte & (1 << MINE)))
					{
						count += 1;
					}
					//location8
					if((mine_board[i+1][j+1].status_byte & (1 << MINE)))
					{
						count += 1;
					}
				}
				*/
				mine_board[i][j].value = count;
				#ifdef TESTMODE
				printf("Value at this position %d %d is %d\n", i, j, count);
				#endif
			}
		}
	}
}

/*
	testGameBoard() function just for test purposes.
	After creating two dimension array game board, this function will be run to test
	Inputs:
	1 - struct Game for information about width and height
	2 - a pointer which points to a two dimension array
	Output:
	Set value for that two dimensions array and print on screen
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

/*
	print_mine_board() function will print playing field on screen based on information from game and each location
	Inputs:
	1 - struct Game for information
	2 - a pointer which points to two dimensions array (mine_board)
	Output:
	Print playing field to screen
*/
void print_mine_board(struct Game game, struct Location** mine_board)
{
	int i, j; //using in loop
	char temp; //using in printf for printing in exact format

	moveToTopLeftCorner();

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

	clearScreen();

	#ifdef TESTMODE
		testPrintPlayingField(game, mine_board);
	#endif
}

/*
	testPrintPlayingField() function will test printing function by printing all original values of two dimenstion array (struct Location**) and comparing with playing field (symbols)
	Inputs:
	1 - struct Game for information
	2 - a pointer which points to two dimensions array (mine_board)
	Output:
	Print playing field to screen (original values)
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

/*
	make_move() function takes 2 coordinates x and y, then set REVEALED bit at this position.
	If value at this position is zero, uncover_zero() function will be called
	Inputs:
	1 - struct Game game (for information)
	2 - address of two dimensions array (mine_board)
	3 - coordinate x (height)
	4 - coordinate y (width)
	Output:
	Set revealed bit for status_byte at position where user choose.
	Running uncover_zeros() function to uncover zero boxes surrounding.
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
	get_move() function get coordinates of two dimensions from users, validating it --> return results to main program
	Inputs:
	1 - address of variable x coordinate in main program
	2 - address of variable y coordinate in main program
	3 - address of variable which stores status of flag or not in main program
	4 - max coordinate of x coordinate (game.height)
	5 - max coordinate of y coordinate (game.width)
	Outputs:
	Thoes pointers from this function will return values to thoes variables in main program
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
	uncover_zeros() function will open (set REVEALED bit) boxes surround a box has value is zero.
	If user choose a box and its value is not zero, uncover_zeros() function won't be called
	Inputs:
	1 - address of two dimensions array (mine_board)
	2 - coordinate x
	3 - coordinate y
	Output:
	Boxes around box which has value is zero will be revealed
*/
void uncover_zeros(struct Location** mine_board, int x, int y)
{
	//int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, x7, y7, x8, y8; //for 8 different coordinates
	/*
	x1 = x2 = x3 = x - 1;
	x4 = x5 = x;
	x6 = x7 = x8 = x + 1;
	y1 = y4 = y6 = y - 1;
	y2 = y7 = y;
	y3 = y5 = y8 = y + 1;
	*/

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

	/*
	//location is not revealed, value is zero, does not have flag --> revealed and call uncover_zeros() function to check surrounding boxes
	//location1
	if(!(mine_board[x1][y1].status_byte & (1 << REVEALED)) && !(mine_board[x1][y1].status_byte & (1 << FLAG)))
	{
		mine_board[x1][y1].status_byte = mine_board[x1][y1].status_byte | (1 << REVEALED);
		if(0 == mine_board[x1][y1].value)
		{
			uncover_zeros(mine_board, x1, y1);
		}
	}
	//location2
	if(!(mine_board[x2][y2].status_byte & (1 << REVEALED)) && !(mine_board[x2][y2].status_byte & (1 << FLAG)))
	{
		mine_board[x2][y2].status_byte = mine_board[x2][y2].status_byte | (1 << REVEALED);
		if(0 == mine_board[x2][y2].value)
		{
			uncover_zeros(mine_board, x2, y2);
		}
	}
	//location3
	if(!(mine_board[x3][y3].status_byte & (1 << REVEALED)) && !(mine_board[x3][y3].status_byte & (1 << FLAG)))
	{
		mine_board[x3][y3].status_byte = mine_board[x3][y3].status_byte | (1 << REVEALED);
		if(0 == mine_board[x3][y3].value)
		{
			uncover_zeros(mine_board, x3, y3);
		}
	}
	//location4
	if(!(mine_board[x4][y4].status_byte & (1 << REVEALED)) && !(mine_board[x4][y4].status_byte & (1 << FLAG)))
	{
		mine_board[x4][y4].status_byte = mine_board[x4][y4].status_byte | (1 << REVEALED);
		if(0 == mine_board[x4][y4].value)
		{
			uncover_zeros(mine_board, x4, y4);
		}
	}
	//location5
	if(!(mine_board[x5][y5].status_byte & (1 << REVEALED)) && !(mine_board[x5][y5].status_byte & (1 << FLAG)))
	{
		mine_board[x5][y5].status_byte = mine_board[x5][y5].status_byte | (1 << REVEALED);
		if(0 == mine_board[x5][y5].value)
		{
			uncover_zeros(mine_board, x5, y5);
		}
	}
	//location6
	if(!(mine_board[x6][y6].status_byte & (1 << REVEALED)) && !(mine_board[x6][y6].status_byte & (1 << FLAG)))
	{
		mine_board[x6][y6].status_byte = mine_board[x6][y6].status_byte | (1 << REVEALED);
		if(0 == mine_board[x6][y6].value)
		{
			uncover_zeros(mine_board, x6, y6);
		}
	}
	//location7
	if(!(mine_board[x7][y7].status_byte & (1 << REVEALED)) && !(mine_board[x7][y7].status_byte & (1 << FLAG)))
	{
		mine_board[x7][y7].status_byte = mine_board[x7][y7].status_byte | (1 << REVEALED);
		if(0 == mine_board[x7][y7].value)
		{
			uncover_zeros(mine_board, x7, y7);
		}
	}
	//location8
	if(!(mine_board[x8][y8].status_byte & (1 << REVEALED)) && !(mine_board[x8][y8].status_byte & (1 << FLAG)))
	{
		mine_board[x8][y8].status_byte = mine_board[x8][y8].status_byte | (1 << REVEALED);
		if(0 == mine_board[x8][y8].value)
		{
			uncover_zeros(mine_board, x8, y8);
		}
	}
	*/
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
		function colour() return colour value of location in mine_board
		Input: value in that location
		Output: a string which will define colour of value in that location
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
