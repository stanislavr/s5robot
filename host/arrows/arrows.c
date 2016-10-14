#include <ncurses.h>

int main()
{
int ch, last_ch;

/* Curses Initialisations */
initscr();
raw();
keypad(stdscr, TRUE);
noecho();

printw("Press E to Exit\n");

last_ch = ERR; // dummy

while((ch = getch()) != 'E')
{	
    switch(ch)
    {
    case KEY_UP:        if(last_ch == ch) {
    					printw("\nUp Arrow");
    					timeout(600); //wait ~0.6s if no input then switch to case ERR
					    }
					    else
					    {
					    	last_ch = ch;
					    	printw("\n*****timedout");	
    						timeout(-1);	
					    }
                break;
    case KEY_DOWN:      if(last_ch == ch) {
    					printw("\nDown Arrow");
					    timeout(600); //wait ~0.6s if no input then switch to case ERR
					    }
					    else
					    {
					    	last_ch = ch;
					    	printw("\n*****timedout");	
    						timeout(-1);
					    }
                break;
    case KEY_LEFT:      if(last_ch == ch) {
    					printw("\nLeft Arrow");
    					timeout(600); //wait ~0.6s if no input then switch to case ERR
					    }
					    else
					    {
					    	last_ch = ch;
					    	printw("\n*****timedout");	
    						timeout(-1);	
					    }
                break;
    case KEY_RIGHT:    if(last_ch == ch) {
    					printw("\nRight Arrow");
    					timeout(600); //wait ~0.6s if no input then switch to case ERR
					    }
					    else
					    {
					    	last_ch = ch;
					    	printw("\n*****timedout");	
    						timeout(-1);	
					    }
                break;
    case ERR:			printw("\n$$$$$timedout");	
    					timeout(-1);
    			break;
    default:    
                printw("\nThe pressed key is %c",ch);
    }
}

printw("\n\Exiting Now\n");
endwin();

return 0;
}