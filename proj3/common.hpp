/*
 * common.hpp
 *
 *  Created on: Dec 1, 2011
 *      Author: mallal, norton, savage, sorenson
 */

#ifndef COMMON_HPP_INCLUDE
#define COMMON_HPP_INCLUDE

#include <stdint.h>
#include <stdio.h>
#include <vector>

struct header {
    uint16_t version;
    uint16_t flags;
    uint16_t queries;
    uint16_t answers;
    uint16_t nservers;
    uint16_t addservers;
} __attribute__((__packed__));

struct question{
	int numBytes;
	char QNAME[FILENAME_MAX];
	uint16_t QTYPE;	
	uint16_t QCLASS;
	uint8_t  QODD[11];
} __attribute__((__packed__));

struct answer {
   char ans[512] ;
} __attribute__((__packed__));


#endif /* COMMON_HPP_INCLUDE */
