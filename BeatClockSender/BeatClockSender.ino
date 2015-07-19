#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
#include "timerModule.h"
#include "TimeCode.h"
#include "stdint.h"

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

//HOW TO OPERATE
//  Make TimerClass objects for each thing that needs periodic service
//  pass the interval of the period in ticks
//  Set MAXINTERVAL to the max foreseen interval of any TimerClass
//  Set MAXTIMER to overflow number in the header.  MAXTIMER + MAXINTERVAL
//    cannot exceed variable size.

//Connect listener pin 2 to sender pin 1 (and ground)

#define LEDPIN 13

#define FRAMERATE 25 //24, 25, 29.97 30
//Globals
TimerClass ledTimer( 500 );
TimerClass ledDwell( 200 );
TimerClass inputTimer( 100 );
TimerClass qBeatTimer( 1000 );

uint16_t msTicks = 0;
uint8_t msTicksMutex = 1; //start locked out

BeatCode playHead;

uint8_t framePiece;

//MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiPort);
MIDI_CREATE_DEFAULT_INSTANCE();

#define MAXINTERVAL 2000

uint8_t frameNumber = 0;
uint16_t myBPM = 100;
//uint16_t time64th = 200;

void setup()
{
  //Test button pin
  pinMode(7, INPUT_PULLUP);
  //Test LED pins
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  //Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  // initialize Timer1
  cli();          // disable global interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B

  // set compare match register to desired timer count:
  OCR1A = 16000;

  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);

  // Set CS10 and CS12 bits for 1 prescaler:
  TCCR1B |= (1 << CS10);


  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);

  // enable global interrupts:
  sei();
}

int i = 0;

void loop()
{
  // main program
  digitalWrite( 8, digitalRead(7));
  if( digitalRead(7) == 0)
  {
    playHead.zero();
    framePiece = 0;
    frameNumber = 0;
    digitalWrite( 9, 0 );
  }  
  if( msTicksMutex == 0 )  //Only touch the timers if clear to do so.
  {
    ledTimer.update(msTicks);
    ledDwell.update(msTicks);
    inputTimer.update(msTicks);
    qBeatTimer.update(msTicks);
    //Done?  Lock it back up
    msTicksMutex = 1;
  }  //The ISR should clear the mutex.
  
  
  if(ledTimer.flagStatus() == PENDING)
  {
    //Turn on LED
    digitalWrite( LEDPIN, 1 );
    //Reset the timer
    ledDwell.set(200);
    
  }
  if(ledDwell.flagStatus() == PENDING)
  {
    // If the light is on, turn it off
    if( digitalRead(LEDPIN) == 1 )
    {
      digitalWrite(LEDPIN, 0);
    }
  }
  if(inputTimer.flagStatus() == PENDING)
  {
    uint32_t tempKnobValue = (analogRead( A0 ) >> 2 );
    myBPM = ((tempKnobValue * 200) >> 8) + 40;
    //time64th = ((1875 / myBPM ) >> 1 );// 60000 / myBPM / 64; //ms per 1/64th beat
    qBeatTimer.setInterval( 60000 / myBPM / 64 );
    //LED gen
    ledTimer.setInterval( 60000 / myBPM );

  }
  if(qBeatTimer.flagStatus() == PENDING)
  {
    qBeatTick();
    
  }
  i++;

 
}

void qBeatTick()
{
  playHead.addFramePiece();
  uint8_t data;
  switch( playHead.framePiece )
  {
    case 0x00:
    data = playHead.frames & 0x0F;
    break;
    case 0x01:
    data = playHead.frames >> 4;
    break;
    case 0x02:
    data = playHead.beats;
    break;
    case 0x03:
    data = playHead.beats >> 4;
    break;
    case 0x04:
    data = playHead.bars;
    break;
    case 0x05:
    data = playHead.bars >> 4;
    break;
    case 0x06:
    data = playHead.eightBars;
    break;
    default:
    case 0x07:
    data = playHead.eightBars >> 4;
    break;
  }
  
  //Send here
  MIDI.sendTimeCodeQuarterFrame( playHead.framePiece, data );
}

ISR(TIMER1_COMPA_vect)
{
  uint32_t returnVar = 0;
  if(msTicks >= ( MAXTIMER + MAXINTERVAL ))
  {
    returnVar = msTicks - MAXTIMER;

  }
  else
  {
    returnVar = msTicks + 1;
  }
  msTicks = returnVar;
  msTicksMutex = 0;  //unlock
}


