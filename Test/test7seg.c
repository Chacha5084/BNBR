/*
 * 7segments.c:
 *	Simple test program to see if we can drive a 7-segment LED
 *	display using the GPIO and little else on the Raspberry Pi
 *
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 */

#undef	PHOTO_HACK

#include <wiringPi.h>

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

//#include "../../../../../../usr/include/stdlib.h"

/*
 *  Segment mapping
 *
 *	 --a--
 *	|     |
 *	f     b
 *	|     |
 *	 --g--
 *	|     |
 *	e     c
 *	|     |
 *	 --d--  p
 */

// GPIO Pin Mapping

static int digits   [6] = {  7, 11, 10, 13, 12, 14    } ;
static int segments [7] = {  6,  5,  4,  3,  2,  1, 0 } ;
#define     LED    29
#define     BUZZER 1
#define     I2C_ADDRESS 0x70
#define APPUYE LOW
#define RELACHE HIGH

#define UP 25
#define DOWN 23
#define LEFT 6
#define RIGHT 24

#define VIBRATOR 2

static const int segmentDigits [] =
{
// a  b  c  d  e  f  g     Segments
// 6  5  4  3  2  1  0,	// wiringPi pin No.

   1, 1, 1, 1, 1, 1, 0,	// 0
   0, 1, 1, 0, 0, 0, 0,	// 1
   1, 1, 0, 1, 1, 0, 1,	// 2
   1, 1, 1, 1, 0, 0, 1,	// 3
   0, 1, 1, 0, 0, 1, 1,	// 4
   1, 0, 1, 1, 0, 1, 1,	// 5
   1, 0, 1, 1, 1, 1, 1,	// 6
   1, 1, 1, 0, 0, 0, 0,	// 7
   1, 1, 1, 1, 1, 1, 1,	// 8
   1, 1, 1, 1, 0, 1, 1,	// 9
   1, 1, 1, 0, 1, 1, 1,	// A
   0, 0, 1, 1, 1, 1, 1,	// b
   1, 0, 0, 1, 1, 1, 0,	// C
   0, 1, 1, 1, 1, 0, 1,	// d
   1, 0, 0, 1, 1, 1, 1,	// E
   1, 0, 0, 0, 1, 1, 1,	// F
   0, 0, 0, 0, 0, 0, 0,	// blank
} ;

int hexValues[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
void setup()
{
    wiringPiSetup();
    pinMode(LED, OUTPUT) ;
    digitalWrite (LED, HIGH) ;    // On
    delay (500) ;        // mS
    digitalWrite (LED, LOW) ;    // Off
    delay (500) ;
}


void affichage7segments(int fd,int sec, int min){

    int unite_min= min % 10 ;
    int unite_sec= sec % 10;
    int dizaine_min= min / 10;
    int dizaine_sec=  sec / 10;

    wiringPiI2CWriteReg8(fd, 0x00, hexValues[dizaine_min]); // On affiche le chiffre 1 (tout à gauche)
    wiringPiI2CWriteReg8(fd, 0x02, hexValues[unite_min]); // On affiche le chiffre 2
    wiringPiI2CWriteReg8(fd, 0x04, 0x02);//On affiche ":"
    wiringPiI2CWriteReg8(fd, 0x06, hexValues[dizaine_sec]); // On affiche le chiffre 3
    wiringPiI2CWriteReg8(fd, 0x08, hexValues[unite_sec]); // On affiche le chiffre 4 (tout à droite)
}

int main()
{
    setup();
    int fd = wiringPiI2CSetup(I2C_ADDRESS); // Initialisation de l'afficheur 7 segments
    // Configuration de l'afficheur 7 segments
    wiringPiI2CWriteReg8(fd, 0x21, 0x01);
    wiringPiI2CWriteReg8(fd, 0x81, 0x00);
    wiringPiI2CWriteReg8(fd, 0xef, 0x00);
    pinMode(UP, INPUT);
    pullUpDnControl(UP, PUD_UP);
    pinMode(DOWN, INPUT);
    pullUpDnControl(DOWN, PUD_UP);
    pinMode(LEFT, INPUT);
    pullUpDnControl(LEFT, PUD_UP);
    pinMode(RIGHT, INPUT);
    pullUpDnControl(RIGHT, PUD_UP);

    for(int i = 30; i > 0; i--)
    {
        affichage7segments(fd,i,0);
        delay(1000);
        
        if (digitalRead(UP) == APPUYE)
        {
            digitalWrite(VIBRATOR, HIGH);
            printf("UP\n");
        }
        else if (digitalRead(DOWN) == APPUYE)
        {
            digitalWrite(VIBRATOR, HIGH);
            printf("DOWN\n");
        }
        else if (digitalRead(LEFT) == APPUYE)
        {
            digitalWrite(VIBRATOR, HIGH);
            printf("LEFT\n");
        }
        else if (digitalRead(RIGHT) == APPUYE)
        {
            digitalWrite(VIBRATOR, HIGH);
            printf("RIGHT\n");
        }
        else
        {
            printf("\n");
            digitalWrite(VIBRATOR, LOW);
        }
    }
    return 0;
}
