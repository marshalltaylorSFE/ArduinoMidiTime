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

//Includes
#include "TimeCode.h"

//**********************************************************************//
//  Constructor
//
void TimeCode::zero( void )
{
  frameNumber = 0;
  milliseconds = 0;
  seconds = 0;
  minutes = 0;
  hours = 0;

}

void TimeCode::addms( uint16_t msVar )
{
  milliseconds += msVar;
  if ( milliseconds >= 1000 )
  {
    milliseconds -= 1000;
    seconds++;
    if ( seconds >= 60 )
    {
      seconds -= 60;
      minutes++;
      if ( minutes >= 60 )
      {
        minutes -= 60;
        if ( hours < 24 )
        {
          hours++;
        }
      }
    }
  }

}
