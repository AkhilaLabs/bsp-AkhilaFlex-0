/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef AKHILAFLEX_IO_H
#define AKHILAFLEX_IO_H

#include "mbed.h"
#include "AKHILAFLEXConfig.h"
#include "AKHILAFLEXComponent.h"
#include "AKHILAFLEXPin.h"

/**
  * Class definition for AKHILAFLEX IO.
  *
  * Represents a collection of all I/O pins on the edge connector.
  */
class AKHILAFLEXIO
{
    public:

	AKHILAFLEXPin			 pin[0];
	AKHILAFLEXPin          P0;
    AKHILAFLEXPin          P1;
    AKHILAFLEXPin          P2;
    AKHILAFLEXPin          P3;
    AKHILAFLEXPin          P4;
    AKHILAFLEXPin          P5;
    AKHILAFLEXPin          P6;
    AKHILAFLEXPin          P7;
    AKHILAFLEXPin          P8;
    AKHILAFLEXPin          P9;
    AKHILAFLEXPin          P10;
    AKHILAFLEXPin          P11;
    AKHILAFLEXPin          P12;
    AKHILAFLEXPin          P13;
    AKHILAFLEXPin          P14;
    AKHILAFLEXPin          P15;
    AKHILAFLEXPin          P16;
    AKHILAFLEXPin          P19;
    AKHILAFLEXPin          P20;

    /**
      * Constructor.
      *
      * Create a representation of all given I/O pins on the edge connector
      *
      * Accepts a sequence of unique ID's used to distinguish events raised
      * by AKHILAFLEXPin instances on the default EventModel.
      */
    AKHILAFLEXIO(int ID_P0, int ID_P1, int ID_P2,
               int ID_P3, int ID_P4, int ID_P5,
               int ID_P6, int ID_P7, int ID_P8,
               int ID_P9, int ID_P10,int ID_P11,
               int ID_P12,int ID_P13,int ID_P14,
               int ID_P15,int ID_P16,int ID_P19,
               int ID_P20);
};

#endif
