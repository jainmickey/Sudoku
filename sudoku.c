/*
 *****************************************************************
 *       A simple SUDOKU game build by Mayank Jain.              * 
 *                                                               *
 *****************************************************************
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<ncurses.h>
#include<signal.h>
#include<stdbool.h>
#include<ctype.h>
#include<string.h>
#include "sudoku.h"

//  these are the functions prototypes used in the game
int clearscr(void);
int check(int ch);
int draw_borders(void);
int draw_grid(void);
int draw_logo(void);
bool draw_menu(void);
int draw_numbers(void);
bool draw_pref(void);
int handle_signal(int signum);    //  to handle the change in size of screen
int matrix(void);
int redraw_all(void);
bool restart_game(void);
int show_cursor(void);
int shutdown(void);
bool startup(void);
int show_menu(void);
int show_pref(void);
int welcome(void); 
int wining(void);
int winscr(void);

// wrapper for our game's globals
struct
{
    
    // the game's board
    int board[9][9], check[9][9];

    // the board's number
    int number;

    // the board's top-left coordinates
    int top, left;

    // the cursor's current location between (0,0) and (8,8)
    int y, x;

	// the cursor's current location of main menu and prefrence menu
	int my, py;

} g;

// here starts our main function
int main()
{
	int ch, win, pref;	//  ch for getting user input other are for main and prefrences menu
	time_t seconds;
    time(&seconds);
    srand((unsigned int) seconds);
    g.number = rand()%(3-1+1)+1;  //  the board's number taken random
    
	tag:

	// start up ncurses
    if (!startup())
    {
       	printw("Error starting up ncurses!\n");
       	return 5;
    }

    // register handler for SIGWINCH (SIGnal WINdow CHanged)
    signal(SIGWINCH, (void (*)(int)) handle_signal);

    clearscr();
    welcome();
    
	//Initializes the menu
    if (!draw_menu())
    {
		shutdown();
		printw("Could not load board from disk!\n");
		return 3;	
    }
 
    while(win != 'E')
    {
	refresh();  //  print on the real screen
	win = getch();  //  wait for user input
	switch (win)
        {
	    case KEY_UP:
			if(g.my == 1)
			{
				mvaddch(g.top+5, g.left+3, ' ');
				mvaddch(g.top+8, g.left+3, '*');
				g.my = 4;
				show_menu();  //  shows the moving * in menu
	       	} 
			else
			{
				mvaddch(g.top+4+g.my, g.left+3, ' ');
				g.my--;
				mvaddch(g.top+4+g.my, g.left+3, '*');
				show_menu();
			}
		break;

	    case KEY_DOWN:
			if(g.my == 4)
			{
				mvaddch(g.top+8, g.left+3, ' ');
				mvaddch(g.top+5, g.left+3, '*');
			        g.my = 1;
				show_menu();
			}
			else
			{
				mvaddch(g.top+4+g.my, g.left+3, ' ');
				g.my++;
				mvaddch(g.top+4+g.my, g.left+3, '*');
				show_menu();
			}
		break;

	    case 10: /* Enter */
	    	if(g.my == 4)	//  selection is exit 
			    win = 'E';	//  stops the loop and exit

			if(g.my != 4)
			{
	   	    	clearscr();
				
		    	if(g.my == 3)	//  selection is prefrences
		    	{	   
					//Initializes the preferences
		        	if (!draw_pref())
		        	{
						shutdown();
						printw("Could not load board from disk!\n");
						return 4;	
		    		}
					int eof = 0;	//  checking condition for below do while loop
					do
					{
						refresh();
						pref = getch();
						pref = toupper(pref); //  convert character into capital letters
						switch (pref)
						{
							case KEY_UP:
								if(g.py == 1)
								{
									mvaddch(g.top+5, g.left+3, ' ');
									mvaddch(g.top+7, g.left+3, '*');
									g.py = 3;
									show_pref();
								} 
								else
								{
									mvaddch(g.top+4+g.py, g.left+3, ' ');
									g.py--;
									mvaddch(g.top+4+g.py, g.left+3, '*');
									show_pref();
								}
							break;

							case KEY_DOWN:
								if(g.py == 3)
								{
									mvaddch(g.top+7, g.left+3, ' ');
									mvaddch(g.top+5, g.left+3, '*');
									g.py = 1;
									show_pref();
								}
								else
								{
									mvaddch(g.top+4+g.py, g.left+3, ' ');
									g.py++;
									mvaddch(g.top+4+g.py, g.left+3, '*');
									show_pref();
								}
							break;

						case 'B':
							goto tag;
						break;

						case 'E':
							shutdown();
							system("clear");
							exit(2);
							break;

						case 10: /* Enter */
							g.number = g.py;
							eof = 1;
						break;
						}

					}while(eof == 0);
		    	}

				if(g.my == 1)
				{
					g.number = rand()%(3-1+1)+1;
				}
			
				if (!restart_game())
				{
					shutdown();
					printw("Could not load board from disk!\n");
					return 6;
				}

				//  starting the game
		    	matrix();
		    	redraw_all();
		
				do
				{	
					refresh();
					ch = getch();
					if(ch>47 && ch<58)
					{
						if(g.check[g.y][g.x] == 0) //  check if user not changing the default number
						{	
							if(check(ch) == -1)
							{
								g.board[g.y][g.x] = ch-48;
								draw_numbers();
								show_cursor();
								if( wining() == 9)
									winscr();
							}
						}
						show_cursor();
					}

					else
					{
						ch = toupper(ch);
						switch (ch)
						{
							// start a new game
							case 'B': 
								goto tag;
							break;
						
							case 'E':
								clearscr();
								shutdown();
								printf("\nThanxxx for Playing..\n\n");
								exit(1);
							break;

							case KEY_UP:
								if (g.y == 0)
									g.y = 8;
								else
									g.y--;
								show_cursor();
							break;

							case KEY_DOWN:
								if (g.y == 8)
									g.y = 0;
								else
									g.y++;
								show_cursor();
							break;

							case KEY_LEFT:
								if (g.x == 0)
									g.x = 8;
								else
									g.x--;
								show_cursor();
							break;

							case KEY_RIGHT:
								if (g.x == 8)
									g.x = 0;
								else
									g.x++;
								show_cursor();
							break;
						}
					}
		
				}while ( ch != 'E' );
		}
		break;
	}
    }
    shutdown();
    clearscr();
    printf("\nThanxxx for Playing..\n\n");
    return 0;
}


/*
 * check the number if it is not present already
 */
int check(int ch)
{
	int i, j;
	for(i=0; i<9; i++)
	{
		if(g.board[g.y][i] == ch-48)
			return 9;
	}
	for(i=0; i<9; i++)
	{
		if(g.board[i][g.x] == ch-48)
			return 9;
	}
	if(g.y>=0 && g.y<3)
	{
		if(g.x>=0 && g.x<3)
		{
			for(i=0; i<3; i++)
			{
				for(j=0; j<3; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
		if(g.x>=3 && g.x<6)
		{
			for(i=0; i<3; i++)
			{
				for(j=3; j<6; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
		if(g.x>=6 && g.x<9)
		{
			for(i=0; i<3; i++)
			{
				for(j=6; j<9; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
		
	}
	if(g.y>=3 && g.y<6)
	{
		if(g.x>=0 && g.x<3)
		{
			for(i=3; i<6; i++)
			{
				for(j=0; j<3; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
		if(g.x>=3 && g.x<6)
		{
			for(i=3; i<6; i++)
			{
				for(j=3; j<6; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
		if(g.x>=6 && g.x<9)
		{
			for(i=3; i<6; i++)
			{
				for(j=6; j<9; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
	}
	if(g.y>=6 && g.y<9)
	{
		if(g.x>=0 && g.x<3)
		{
			for(i=6; i<9; i++)
			{
				for(j=0; j<3; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
		if(g.x>=3 && g.x<6)
		{
			for(i=6; i<9; i++)
			{
				for(j=3; j<6; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
		if(g.x>=6 && g.x<9)
		{
			for(i=6; i<9; i++)
			{
				for(j=6; j<9; j++)
				{
					if(g.board[i][j] == ch-48)
						return 9;
				}
			}
		}
	}
	return -1;	
}


/*
 *  clear the screen
 */
int clearscr(void)
{
     int i,j;
    // get window's dimensions
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    for ( j = 0; j < maxy; j++)
    {
    for ( i = 0; i < maxx; i++)
        mvaddch(j, i, ' ');
    }
    return 2;
}


/*
 * Draws game's borders.
 */
int
draw_borders(void)
{
    // get window's dimensions
    int maxy, maxx, i;
    getmaxyx(stdscr, maxy, maxx);

    // enable color if possible (else b&w highlighting)
    if (has_colors())
    {
        attron(A_PROTECT);
        attron(COLOR_PAIR(PAIR_BORDER));
    }
    else
        attron(A_REVERSE);

    // draw borders
    for ( i = 0; i < maxx; i++)
    {
        mvaddch(0, i, ' ');
        mvaddch(maxy-1, i, ' ');
    }
	// draw header
    char header[maxx+1];
    sprintf(header, "%s by %s", TITLE, AUTHOR);
    mvaddstr(0, (maxx - strlen(header)) / 2, header);

    // draw footer
    mvaddstr(maxy-1, 1, "[B]ack");
    mvaddstr(maxy-1, maxx-13, "[E]xit");

    // disable color if possible (else b&w highlighting)
    if (has_colors())
        attroff(COLOR_PAIR(PAIR_BORDER));
    else
        attroff(A_REVERSE);
    return 2;
}


/*
 *  draws the grid or framework of the game
 */
int
draw_grid(void)
{
    // get window's dimensions
    int maxy, maxx, i;
    getmaxyx(stdscr, maxy, maxx);

    // determine where top-left corner of board belongs 
    g.top = maxy/2 - 7;
    g.left = maxx/2 - 30;

    // enable color if possible
    if (has_colors())
        attron(COLOR_PAIR(PAIR_GRID));

    // print grid
    for ( i = 0 ; i < 3 ; ++i )
    {
        mvaddstr(g.top + 0 + 4 * i, g.left, "+-------+-------+-------+");
        mvaddstr(g.top + 1 + 4 * i, g.left, "|       |       |       |");
        mvaddstr(g.top + 2 + 4 * i, g.left, "|       |       |       |");
        mvaddstr(g.top + 3 + 4 * i, g.left, "|       |       |       |");
    }
    mvaddstr(g.top + 4 * 3, g.left, "+-------+-------+-------+" );

    // remind user of level and #
    char reminder[maxx+1];
    sprintf(reminder, "   Playing Puzzle #%d", g.number);
    mvaddstr(g.top + 14, g.left + 25 - strlen(reminder), reminder);

    // disable color if possible
    if (has_colors())
        attroff(COLOR_PAIR(PAIR_GRID));
    return 2;
}


/*
 * draw the game's logo
 */ 
int
draw_logo(void)
{
    // determine top-left coordinates of logo
    int top = g.top + 2;
    int left = g.left + 30;

    // enable color if possible
    if (has_colors())
        attron(COLOR_PAIR(PAIR_LOGO));

    // draw logo
    mvaddstr(top + 0, left, "               _       _          ");
    mvaddstr(top + 1, left, "              | |     | |         ");
    mvaddstr(top + 2, left, " ___ _   _  __| | ___ | | ___   _ ");
    mvaddstr(top + 3, left, "/ __| | | |/ _` |/ _ \\| |/ / | | |");
    mvaddstr(top + 4, left, "\\__ \\ |_| | (_| | (_) |   <| |_| |");
    mvaddstr(top + 5, left, "|___/\\__,_|\\__,_|\\___/|_|\\_\\\\__,_|");

    // sign logo
    char signature[3+strlen(AUTHOR)+1];
    sprintf(signature, "by %s", AUTHOR);
    mvaddstr(top + 7, left + 35 - strlen(signature) - 1, signature);

    // disable color if possible
    if (has_colors())
        attroff(COLOR_PAIR(PAIR_LOGO));
    return 2;
}


/*
 *  draw the game's main menu
 */
bool draw_menu(void)
{
    int maxx,maxy;
    getmaxyx(stdscr, maxy, maxx);
    g.top = (maxy-10)/2;
    g.left = (maxx-42)/2;
    if (has_colors())
        	attron(COLOR_PAIR(PAIR_GRID));

	mvaddstr(g.top+2, g.left, "+---------------------------------------+");
	mvaddstr(g.top+3, g.left, "|                  Menu                 |");
	if (has_colors())
		attron(COLOR_PAIR(PAIR_BORDER));
	mvaddstr(g.top+3, g.left+19, "Menu");
	attroff(COLOR_PAIR(PAIR_BORDER));
		
	mvaddstr(g.top+4, g.left, "|---------------------------------------|");
	mvaddstr(g.top+5, g.left, "|  *             New Game               |");
	mvaddstr(g.top+6, g.left, "|              Restart Game             |");
	mvaddstr(g.top+7, g.left, "|               Prefrences              |");
	mvaddstr(g.top+8, g.left, "|                  Exit                 |");
	mvaddstr(g.top+9, g.left, "+---------------------------------------+");

	if (has_colors())
        attroff(COLOR_PAIR(PAIR_GRID));
	draw_borders(); 
	g.my = 1;
	show_menu();
	return true;
}


/*
 *  draw the numbers in the game's framework
 */
int
draw_numbers(void)
{
    int i, j;
    // iterate over board's numbers
    for ( i = 0; i < 9; i++)
    {
        for ( j = 0; j < 9; j++)
        {
            // determine char
            char c = (g.board[i][j] == 0) ? '.' : g.board[i][j] + '0';
            mvaddch(g.top + i + 1 + i/3, g.left + 2 + 2*(j + j/3), c);
            refresh();
        }
    }
    return 2;
}


/*
 *  draw the prefernces menu
 */
bool draw_pref(void)
{
    int maxx,maxy;
    getmaxyx(stdscr, maxy, maxx);
    g.top = (maxy-10)/2;
    g.left = (maxx-42)/2;
    if (has_colors())
        	attron(COLOR_PAIR(PAIR_GRID));

	mvaddstr(g.top+2, g.left, "+---------------------------------------+");
	mvaddstr(g.top+3, g.left, "|              Preferences              |");
	if (has_colors())
		attron(COLOR_PAIR(PAIR_BORDER));
	mvaddstr(g.top+3, g.left+15, "Preferences");
	attroff(COLOR_PAIR(PAIR_BORDER));
		
	mvaddstr(g.top+4, g.left, "|---------------------------------------|");
	mvaddstr(g.top+5, g.left, "|  *             Puzzle 1               |");
	mvaddstr(g.top+6, g.left, "|                Puzzle 2               |");
	mvaddstr(g.top+7, g.left, "|                Puzzle 3               |");
	mvaddstr(g.top+8, g.left, "|                                       |");
	mvaddstr(g.top+9, g.left, "+---------------------------------------+");

	if (has_colors())
        attroff(COLOR_PAIR(PAIR_GRID)); 
	draw_borders();
	g.py = 1;
	show_pref();
	refresh();
	return true;   
}


/*
 *  it will handle the changes in the screen
 */
int
handle_signal(int signum)
{
    // handle a change in the window (i.e., a resizing)
    if (signum == SIGWINCH)
        redraw_all();

    // re-register myself so this signal gets handled in future too
    signal(signum, (void (*)(int)) handle_signal);
    return 2;
}


/*
 *  it is the main matrix of all sudoku
 */  
int matrix(void)
{
	int i,j,c=0;	
	int mat[][81]={
	{2,5,0,8,0,0,0,6,7,
	6,1,0,2,0,7,3,0,0,
	0,0,4,0,0,1,0,0,2,
	4,0,0,0,0,0,5,3,0,
	7,0,0,4,0,3,0,0,6,
	0,2,9,0,0,0,0,0,8,
	5,0,0,1,0,0,9,0,0,
	0,0,7,9,0,4,0,8,5,
	8,9,0,0,0,6,0,4,1},
	{5,9,4,0, 0,8,0,0,6,
	0,2,6,1,0,0,8,9,0,
	8,0,0,0,6,0,7,0,0,
	0,0,0,0,8,2,4,6,0,
	0,0,0,4,0,1,0,0,0,
	0,8,2,6,5,0,0,0,0,
	0,0,7,0,2,0,0,0,5,
	0,6,8,0,0,4,3,1,0,
	3,0,0,9,0,0,2,7,8},
	{8,1,0,0,0,0,2,0,9,
	0,9,7,3,0,0,0,5,4,
	3,0,0,5,8,0,0,6,0,
	0,0,0,8,0,2,9,4,0,
	0,0,8,0,1,0,6,0,0,
	0,2,3,7,0,4,0,0,0,
	0,8,0,0,7,5,0,0,6,
	5,6,0,0,0,8,1,2,0,
	7,0,1,0,0,0,0,8,5}};
	
	for (i=0;i<9;i++)
	{
		for(j=0;j<9;j++)
		{
			g.board[i][j] = mat[g.number-1][c];
			g.check[i][j] = mat[g.number-1][c];
			c++;
		}
	}
	return 0;
}


/*
 *  it will redraw all the game
 */
int
redraw_all(void)
{
    // reset ncurses
    endwin();
    refresh();

    // clear screen
    clear();

    // re-draw everything
    draw_borders();
    draw_grid();
    draw_logo();
    draw_numbers();

    // show cursor
    show_cursor();
    return 2;
}


/*
 *  it will restart the whole game
 */
bool
restart_game(void)
{
    matrix();
    // redraw board
    draw_grid();
    draw_numbers();

    /*// get window's dimensions
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);*/

    // move cursor to board's center
    g.y = g.x = 4;
    show_cursor();

    return true;
}


/*
 *  it will shutdown the ncurses
 */
int shutdown(void)
{
	clearscr();
	endwin();
	return 2;
}


/*
 *  it shows the cursor in the game after any move
 */
int
show_cursor(void)
{
    // restore cursor's location
    move(g.top + g.y + 1 + g.y/3, g.left + 2 + 2*(g.x + g.x/3));
    return 2;
}

int show_menu(void)
{
    move(g.top+4+g.my, g.left+3);
    return 2;
}

int show_pref(void)
{
    move(g.top+4+g.py, g.left+3);
    return 2;
}


/*
 *  starts the ncurses library
 */
bool
startup(void)
{
    // initialize ncurses
    if (initscr() == NULL)
        return false;

    // prepare for color if possible
    if (has_colors())
    {
        // enable color
        if (start_color() == ERR || attron(A_PROTECT) == ERR)
        {
            endwin();
            return false;
        }

        // initialize pairs of colors
        if (init_pair(PAIR_BANNER, FG_BANNER, BG_BANNER) == ERR ||
            init_pair(PAIR_GRID, FG_GRID, BG_GRID) == ERR ||
            init_pair(PAIR_BORDER, FG_BORDER, BG_BORDER) == ERR ||
            init_pair(PAIR_LOGO, FG_LOGO, BG_LOGO) == ERR)
        {
            endwin();
            return false;
        }
    }

    // don't echo keyboard input
    if (noecho() == ERR)
    {
        endwin();
        return false;
    }

    // disable line buffering and certain signals
    if (raw() == ERR)
    {
        endwin();
        return false;
    }

    // enable arrow keys
    if (keypad(stdscr, true) == ERR)
    {
        endwin();
        return false;
    }

    // wait 1000 ms at a time for input
    timeout(1000);

    // w00t
    return true;
}


/*
 *  it draws the welcome logo of main screen
 */
int welcome(void)
{
	int maxx, maxy;
	getmaxyx(stdscr,maxy,maxx);
	if (has_colors())
        	attron(COLOR_PAIR(PAIR_BANNER));
	mvaddstr((maxy/maxy), (maxx-52)/2, "___              _______ _    ___ _____ _     _ ____ ");
	mvaddstr(2, (maxx-52)/2, "\\  \\            /  /  __| |  |  _|  _  | \\   / |  __|");
	mvaddstr(3, (maxx-52)/2, " \\  \\  ______  /  /| |_ | |  | | | | | |  \\_/  | |_  ");
	mvaddstr(4, (maxx-52)/2, "  \\  \\/  __  \\/  / |  _|| |  | | | | | |       |  _| ");
	mvaddstr(5, (maxx-52)/2, "   \\    /  \\    /  | |__| |__| |_| |_| | |\\_/| | |__ ");
	mvaddstr(6, (maxx-52)/2, "    \\__/    \\__/   |____|____|___|_____|_|   |_|____|");
	if (has_colors())
        attroff(COLOR_PAIR(PAIR_BANNER)); 
	return 2;
}

int wining(void)
{
	int i, j;
	for(i=0; i<9; i++)
	{
		for(j=0; j<9; j++)
		{
			if(g.board[i][j] == 0)
				return -1;
		}
	}
	return 9;
}

int winscr(void)
{
	clearscr();
	draw_borders();
	int maxx,maxy;
	getmaxyx(stdscr, maxy,maxx);
	maxy = (maxy-9)/2;
	maxx = (maxx-68)/2;
	if (has_colors())
        	attron(COLOR_PAIR(PAIR_BANNER));
	mvaddstr(maxy+0, maxx, "___      ___ ______ _    _    __                 __ _ ___       _         ");
	mvaddstr(maxy+1, maxx, "\\  \\    /  /|  __  | |  | |   \\ \\               / /| |   \\     | |   ");
	mvaddstr(maxy+2, maxx, " \\  \\  /  / | |  | | |	| |    \\ \\             / / | | |\\ \\    | |");
	mvaddstr(maxy+3, maxx, "  \\  \\/  /  | |  | | |  | |	\\ \\           / /  | | | \\ \\   | |    ");
	mvaddstr(maxy+4, maxx, "   \\    /   | |  | | |  | |	 \\ \\  _____  / /   | | |  \\ \\  | |    ");
	mvaddstr(maxy+5, maxx, "    |  |    | |  | | |  | |	  \\ \\/  _  \\/ /    | | |   \\ \\ | |       ");
	mvaddstr(maxy+6, maxx, "    |  |    | |__| | |__| |        \\   / \\   /     | | |    \\ \\| |    ");
	mvaddstr(maxy+7, maxx, "    |__|    |______|______|         \\_/   \\_/      |_|_|     \\___|     ");
	if (has_colors())
        attroff(COLOR_PAIR(PAIR_BANNER)); 
	return 2;
	
}
