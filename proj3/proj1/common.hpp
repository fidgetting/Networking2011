/*
 * common.hpp
 *
 *  Created on: Aug 28, 2011
 *      Author: mallal, norton, savage, sorenson
 */

#ifndef COMMON_HPP_INCLUDE
#define COMMON_HPP_INCLUDE

#include <stdint.h>
#include <stdio.h>
#include <vector>

#pragma pack(1)
struct header{
    uint16_t version;
    uint16_t flags;
    uint16_t queries;
    uint16_t answers;
    uint16_t nservers;
    uint16_t addservers;
};
#pragma pack(1)
struct question{
	int numBytes;
	char QNAME[FILENAME_MAX];
	uint16_t QTYPE;	
	uint16_t QCLASS;
};
    

#pragma pack()
struct answer {
   char ans[512] ;
};


#endif /* COMMON_HPP_INCLUDE */
