/*
 * common.hpp
 *
 *  Created on: Aug 28, 2011
 *      Author: norton
 */

#ifndef COMMON_HPP_INCLUDE
#define COMMON_HPP_INCLUDE

#pragma pack()
struct message {
    unsigned char version;
    unsigned int num;

    message() : version(1), num(0) { }
    message(unsigned int num) : version(1), num(num) { }
};

#pragma pack()
struct reply {
    unsigned char version;

    reply() : version(1) { }
};


#endif /* COMMON_HPP_INCLUDE */
