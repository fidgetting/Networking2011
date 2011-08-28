/*
 * common.hpp
 *
 *  Created on: Aug 28, 2011
 *      Author: norton
 */

#ifndef COMMON_HPP_INCLUDE
#define COMMON_HPP_INCLUDE

struct message {
    unsigned char version;
    unsigned int num;

    message(unsigned int num) : version(1), num(num) { }
};

struct reply {
    unsigned char version;

    reply() : version(1) { }
};


#endif /* COMMON_HPP_INCLUDE */
