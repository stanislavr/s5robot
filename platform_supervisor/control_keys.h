/*
 * control_keys.h
 *
 *  Created on: October 19, 2016
 *      Author: Stanislav Rashevskyi
 *      Purpose: header for control_keys.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ncurses.h>
#define DEBUG_ARROWS 1  // enable debug printfs

int acceptArrowKey(int socket_client);
int stopArrowKey(int socket_client, int last_key, int new_key);