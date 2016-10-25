/*
 * control_keys.c
 *
 *  Created on: October 19, 2016
 *      Author: Stanislav Rashevskyi
 *      Purpose: Accept and send to the robot commands via keyboard pressed keys 
 *      Reference: http://www.cyberciti.biz/faq/linux-install-ncurses-library-headers-on-debian-ubuntu-centos-fedora/
 *
 *      HOW TO:
 *          Arrows - drive robot (forward, backward, turn left, turn right)
 *          WASD (no caps) - control camera position (up, down, left, right)
 *          Shift+M (i.e. Capital M) - exits this mode and brings u back to main menu
 */

#include "platform_supervisor.h"

#ifndef CONTROL_KEYS_H_ 
	#define CONTROL_KEYS_H_
 	#include "control_keys.h"
 //	#include "platform_supervisor.h"
#endif


#ifdef TEST_KEYS 
	int main()
	{
		acceptArrowKey(23322);
		return 0;
	}
#endif

/*
 * acceptArrowKey()
 * Function that detects pressed and released keys using Ncurses library and sends 
 * communication commands to robot (see Communication Protocol Document)
 *
 *
 * Inputs: 
 *          socket_client - id of the socket to use for data transmission
 *  
 * Outputs:
 *          Non-zero for error
 *
 * Note: for timeout() when argument < 600 then you get timedout after 1st key press
 *          no functions can be binded for '0' (it's a dummy value for timedout keys)
 */
int acceptArrowKey(int socket_client) {

int ch;             /* buffer for pressed key */
int last_ch;        /* buffer for previously pressed key */
int menu_ch = 'M';  /* when pressed this key you exit arrow mode */

/* Curses Initialisations */
initscr();              /* Start curses mode        */
raw();                  /* Line buffering disabled  */
keypad(stdscr, TRUE);   /* We get F1, F2 etc..      */
noecho();               /* disable echo of captured key */
scrollok(stdscr,TRUE);  /* enable auto-scrolling */


printw("\n*** INSTRUCTIONS ***\n");
printw("Arrows - drive robot (forward, backward, turn left, turn right)\n");
printw("WASD - control camera position (up, down, left, right)\n");
printw("Shift+M - exits this mode and brings u back to main menu\n");
printw("\nP.S. make sure CAPSLOCK is Off\n");


last_ch = '0'; // init value of prev pressed key (unused value)


/* Accepting keys until "menu_ch" is pressed */
while((ch = getch()) != menu_ch)
{	
    switch(ch)
    {
    case KEY_UP:        
                    if(last_ch == ch)
                     {
                        /* key remains pressed */

    					#ifdef DEBUG_ARROWS 
                         printw("Up Arrow - Drive Forward\n");
                        
                        #endif

    					timeout(600); //wait ~0.6s if no input then switch to case ERR
    			    }
    			    else
    			    {   
                        /* new key has been pressed */

                        /* stop motor cmd to EPL */
                        if(stopArrowKey(socket_client, last_ch, ch)) {
                            return -1;
                        }

                        /* drive dorward cmd to EPL */
                        if(cmd_send(socket_client,"<R1>")) {
                            return -1;
                        }

                        last_ch = ch; //update last key pressed;
    			    }
                    break;


    case KEY_DOWN:                
                    if(last_ch == ch)
                     {
                        /* key remains pressed */

                        #ifdef DEBUG_ARROWS 
                         printw("Down Arrow - Drive Backward\n");
                        
                        #endif

                        timeout(600); //wait ~0.6s if no input then switch to case ERR
                    }
                    else
                    {   
                        /* new key has been pressed */

                        /* stop motor cmd to EPL */
                        if(stopArrowKey(socket_client, last_ch, ch)) {
                            return -1;
                        }

                        /* drive backward cmd to EPL */
                        if(cmd_send(socket_client,"<R2>")) {
                            return -1;
                        }

                        last_ch = ch; //update last key pressed;
                    }
                    break;


    case KEY_LEFT:      
                    if(last_ch == ch)
                     {
                        /* key remains pressed */

                        #ifdef DEBUG_ARROWS 
                         printw("Left Arrow - Turn Left\n");
                        
                        #endif

                        timeout(600); //wait ~0.6s if no input then switch to case ERR
                    }
                    else
                    {   
                        /* new key has been pressed */

                        /* stop motor cmd to EPL */
                        if(stopArrowKey(socket_client, last_ch, ch)) {
                            return -1;
                        }

                        /* turn left cmd to EPL */
                        if(cmd_send(socket_client,"<R3>")) {
                            return -1;
                        }
                        timeout(100);

                        last_ch = ch; //update last key pressed;
                    }
                    break;

    case KEY_RIGHT:    
                      if(last_ch == ch)
                     {
                        /* key remains pressed */

                        #ifdef DEBUG_ARROWS 
                         printw("Right Arrow - Turn Right\n");
                        
                        #endif

                        timeout(600); //wait ~0.6s if no input then switch to case ERR
                    }
                    else
                    {   
                        /* new key has been pressed */

                        /* stop motor cmd to EPL */
                        if(stopArrowKey(socket_client, last_ch, ch)) {
                            return -1;
                        }

                        /* turn right cmd to EPL */
                        if(cmd_send(socket_client,"<R4>")) {
                            return -1;
                        }
                        timeout(100);

                        last_ch = ch; //update last key pressed;
                    }
                    break;
   
    case 'w':
                    if(last_ch == ch)
                     {
                        /* key remains pressed */

                        #ifdef DEBUG_ARROWS 
                         printw("'w' - Tilt Camera Up\n");
                        
                        #endif

                        timeout(600); //wait ~0.6s if no input then switch to case ERR
                    }
                    else
                    {   
                        /* new key has been pressed */

                        /* stop motor cmd to EPL */
                        if(stopArrowKey(socket_client, last_ch, ch)) {
                            return -1;
                        }

                        /* tilt camera up cmd to EPL */
                        if(cmd_send(socket_client,"<W1>")) {
                            return -1;
                        }

                        last_ch = ch; //update last key pressed;
                    }
                    break;

    case 's':
                    if(last_ch == ch)
                     {
                        /* key remains pressed */

                        #ifdef DEBUG_ARROWS 
                         printw("'s' - Tilt Camera Down\n");
                        
                        #endif

                        timeout(600); //wait ~0.6s if no input then switch to case ERR
                    }
                    else
                    {   
                        /* new key has been pressed */

                        /* stop motor cmd to EPL */
                        if(stopArrowKey(socket_client, last_ch, ch)) {
                            return -1;
                        }

                        /* tilt camera down cmd to EPL */
                        if(cmd_send(socket_client,"<W2>")) {
                            return -1;
                        }

                        last_ch = ch; //update last key pressed;
                    }
                    break;

     case 'a':
                    if(last_ch == ch)
                     {
                        /* key remains pressed */

                        #ifdef DEBUG_ARROWS 
                         printw("'a' - Pan Camera Left\n");
                        
                        #endif

                        timeout(600); //wait ~0.6s if no input then switch to case ERR
                    }
                    else
                    {   
                        /* new key has been pressed */

                        /* stop motor cmd to EPL */
                        if(stopArrowKey(socket_client, last_ch, ch)) {
                            return -1;
                        }

                        /* pan camera left cmd to EPL */
                        if(cmd_send(socket_client,"<W3>")) {
                            return -1;
                        }

                        last_ch = ch; //update last key pressed;
                    }
                    break;

    case 'd':
                    if(last_ch == ch)
                     {
                        /* key remains pressed */

                        #ifdef DEBUG_ARROWS 
                         printw("'d' - Pan Camera Right\n");
                        
                        #endif

                        timeout(600); //wait ~0.6s if no input then switch to case ERR
                    }
                    else
                    {   
                        /* new key has been pressed */

                        /* stop motor cmd to EPL */
                        if(stopArrowKey(socket_client, last_ch, ch)) {
                            return -1;
                        }

                        /* pan camera right cmd to EPL */
                        if(cmd_send(socket_client,"<W4>")) {
                            return -1;
                        }

                        last_ch = ch; //update last key pressed;
                    }
                    break;

    case ERR:			
                /* key was released */
                /* stop motor cmd to EPL */
                if(stopArrowKey(socket_client, last_ch, ch)) {
                    return -1;
                }

                last_ch = '0'; // value to show timedout key
                ch = '0';
		      	break;

    default:    
                printw("No function under %c \n",ch);

                last_ch = ch; //update last key pressed;               
    }
}

printw("\n Going back to Main Menu...\n");
sleep(1);

endwin(); /* End curses mode         */

return 0;
}

/*
 * stopArrowKey()
 * When key is released function sends command to stop motor that was used 
 * (see Communication Protocol Document)
 *
 * Inputs: 
 *          socket_client - id of the socket to use for data transmission
 *          last_key - indicates what key was released
 *          new_key - indicates new key
 *
 *  
 * Outputs:
 *          Non-zero for error
 *
 */
int stopArrowKey(int socket_client, int last_key, int new_key)
{


    if (last_key == KEY_UP      ||
        last_key == KEY_DOWN    ||
        last_key == KEY_RIGHT   || 
        last_key == KEY_LEFT)
     {

            #ifdef DEBUG_ARROWS 
             printw("\nDifferent Key - Stop Driving Motor\n");   
            
            #endif

            /* send Stop driving motors cmd to EPL */
            if(cmd_send(socket_client,"<R0>")) {
            return -1;
            }
     }

     else
        {
            if (last_key == 'w' ||
                last_key == 'a' ||
                last_key == 's' || 
                last_key == 'd')
             {

                   #ifdef DEBUG_ARROWS 
                     printw("\nDifferent Key - Stop Camera Motor\n");   
                    
                    #endif         

                     /* send Stop camera movement cmd to EPL */
                    if(cmd_send(socket_client,"<W0>")) {
                    return -1;
                    }
             }

        }

    timeout(-1); //wait for the input 

    return 0;
}