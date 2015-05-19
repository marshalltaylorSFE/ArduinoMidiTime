#include <MIDI.h>
#include <SoftwareSerial.h>
#include "TimeCode.h"

SoftwareSerial softSerial(2, 3);
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial, SoftMIDI);

TimeCode timeBuffer;
uint32_t framesPassed = 0;


// -----------------------------------------------------------------------------

// This function will be automatically called when a NoteOn is received.
// It must be a void-returning function with the correct parameters,
// see documentation here:
// http://arduinomidilib.fortyseveneffects.com/a00022.html

uint8_t previousFramePiece = 0;

void handleQFrame( byte dataVar )
{
  uint8_t payload = dataVar & 0x0F;
  uint8_t thisFramePiece = ( dataVar >> 4 ) & 0x0F;
  // Check for partial stamps
  if (( thisFramePiece == previousFramePiece + 1 ) || ( thisFramePiece == 0 ))
  {
    // Decode the MTC
    uint8_t framePiece = ( dataVar >> 4 ) & 0x0F;
    switch ( framePiece )
    {
      case 0x00:
        timeBuffer.frameNumber = payload;
        break;
      case 0x01:
        timeBuffer.frameNumber |= ( payload << 4 );
        break;
      case 0x02:
        timeBuffer.seconds = payload;
        break;
      case 0x03:
        timeBuffer.seconds |= ( payload << 4 );
        break;
      case 0x04:
        timeBuffer.minutes = payload;
        break;
      case 0x05:
        timeBuffer.minutes |= ( payload << 4 );
        break;
      case 0x06:
        timeBuffer.hours = payload;
        break;
      default:
      case 0x07:
        timeBuffer.hours |= ( payload << 4 );
        //Big hit
        Serial.print( timeBuffer.hours );
        Serial.print(",");
        Serial.print( timeBuffer.minutes );
        Serial.print(",");
        Serial.print( timeBuffer.seconds );
        Serial.print(",");
        Serial.print( timeBuffer.frameNumber );
        Serial.print(",");
        Serial.print( framesPassed );
        Serial.print("\n");
        //framesPassed = 0;

        
        break;
    }
    previousFramePiece = thisFramePiece;
  }

}

void handleClock( void )
{
  //Serial.print("*");
  framesPassed++;
}


// -----------------------------------------------------------------------------

void setup()
{
  // Connect the handleNoteOn function to the library,
  // so it is called upon reception of a NoteOn.
  SoftMIDI.setHandleTimeCodeQuarterFrame(handleQFrame);  // Put only the name of the function
  SoftMIDI.setHandleClock(handleClock);
  // Initiate MIDI communications, listen to all channels
  SoftMIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(57600);
  Serial.println("MIDI listener");

}

void loop()
{
  // Call MIDI.read the fastest you can for real-time performance.
  SoftMIDI.read();

  // There is no need to check if there are messages incoming
  // if they are bound to a Callback function.
  // The attached method will be called automatically
  // when the corresponding message has been received.
}

// This takes a int input and converts to char
//
// The output will be an char if the int
// is between zero and 0xF
//
// Otherwise, the output is $.
//
char hex2char(int hexin)
{
  int charout;
  charout = 0x24; // default $ state
  if (hexin >= 0x00)
  {
    if (hexin <= 0x09)
    {
      charout = hexin + 0x30;
    }
  }
  if (hexin >= 0x0A)
  {
    if (hexin <= 0x0F)
    {
      charout = hexin - 10 + 0x41;
    }
  }
  return charout;
}


