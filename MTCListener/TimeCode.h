//**********************************************************************//
//  BEERWARE LICENSE
//
//  This code is free for any use provided that if you meet the author
//  in person, you buy them a beer.
//
//  This license block is BeerWare itself.
//
//  Written by:  Marshall Taylor
//  Created:  March 21, 2015
//
//**********************************************************************//

#ifndef TIMECODE_H_INCLUDED
#define TIMECODE_H_INCLUDED

#include "stdint.h"

//Classes
//**********************************************************************

class TimeCode
{
  public:
    void zero( void );
    void addms( uint16_t );
    uint8_t frameNumber;
    uint16_t milliseconds;
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

};

#endif // TIMECODE_H_INCLUDED

