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

typedef unsigned char uint8_t;
typedef unsigned long uint32_t;

#pragma pack(1)
typedef struct {
    uint8_t  version;
    uint32_t l_size;
    char     f_name[FILENAME_MAX];
} list_header;

#pragma pack(1)
typedef struct {
    uint8_t version;
    char host[256];
    char port[8];
} list_element;

#pragma pack(1)
typedef struct {
    uint8_t  version;
    uint32_t f_size;
} file_header;

#endif /* AWGET_H_INCLUDE */
