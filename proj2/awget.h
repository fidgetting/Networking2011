/*
 * awget.h
 *
 *  Created on: Sep 30, 2011
 *      Author: Alex Norton,
 *              Nick Savage,
 *              Joshua Sorensen,
 *              Jef Mallal
 */

#ifndef AWGET_H_INCLUDE
#define AWGET_H_INCLUDE

#include <stdio.h>
#include <stdint.h>

typedef struct list_header_s  list_header;
typedef struct list_element_s list_element;
typedef struct file_header_s  file_header;

struct list_header_s {
    uint8_t  version;
    uint32_t l_size;
    char     f_name[FILENAME_MAX];
} __attribute__((__packed__));

struct list_element_s {
    uint8_t version;
    char host[256];
    char port[8];
} __attribute__((__packed__));

struct file_header_s {
    uint8_t  version;
    uint32_t f_size;
} __attribute__((__packed__));

#endif /* AWGET_H_INCLUDE */
