/*
 * awget.h
 *
 *  Created on: Oct 4, 2011
 *      Author: Alex Norton,
 *              Nick Savage,
 *              Joshua Sorensen,
 *              Jef Mallal
 */

#include <awget.h>

#include <stdio.h>
#include <stdlib.h>

/* ************************************************************************** */
/* *** connection function ************************************************** */
/* ************************************************************************** */

/**
 * This handles an incoming connection. Since the stepping stone doesn't create
 * a request it only handles a request, this is where all the work is really
 * done.
 *
 * It is important to note that the arguments for this function are weird. All
 * that needs to be passed to this function is the number of the socket that it
 * is talking on. Since the socket is a file descriptor, this only needs to be a
 * number between 0 and 1024, meaning that the number can easily fit in the size
 * of a pointer. So the given arguments are not a pointer, but actually an int.
 * This is done this way to avoid using the heap.
 *
 * @param args the socket that this connection should handle.
 */
void* connection(void* args) {
  unsigned int s = (unsigned int)args;
  list_header  list_h;
  list_element list[1024];
  file_header  file;
  char buffer[1024];



  return NULL;
}

/* ************************************************************************** */
/* *** main function ******************************************************** */
/* ************************************************************************** */



