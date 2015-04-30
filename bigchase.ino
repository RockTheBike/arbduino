#define VERSION "Chase lots of lights"

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

unsigned long time = 0;
unsigned long loopcount = 0;
#define REPORT_INTERVAL 1000
unsigned long next_report_time = 0;

#define NUM_STRIPS 2
const int PIN_PIXELS[] = { 12, 13 };
#define NUM_PIXELS 150
Adafruit_NeoPixel strips[] = {
	Adafruit_NeoPixel( NUM_PIXELS, PIN_PIXELS[0], NEO_GRB|NEO_KHZ800 ),
	Adafruit_NeoPixel( NUM_PIXELS, PIN_PIXELS[1], NEO_GRB|NEO_KHZ800 ) };


void setup() {
	Serial.begin(57600);
	Serial.println(VERSION);
	for( int i=0; i<NUM_STRIPS; i++ ) {
		strips[i].begin();
		strips[i].show(); // Initialize all pixels to 'off'
	}
}

const uint32_t SPRITES[][NUM_STRIPS] = {
	{ Adafruit_NeoPixel::Color(0,0,0),	Adafruit_NeoPixel::Color(0,0,0) },
	{ Adafruit_NeoPixel::Color(255,0,0),	Adafruit_NeoPixel::Color(255,0,0) },
	{ Adafruit_NeoPixel::Color(255,255,0),	Adafruit_NeoPixel::Color(255,255,0) },
	{ Adafruit_NeoPixel::Color(0,255,0),	Adafruit_NeoPixel::Color(0,255,0) },
	{ Adafruit_NeoPixel::Color(0,255,255),	Adafruit_NeoPixel::Color(0,255,255) },
	{ Adafruit_NeoPixel::Color(0,0,255),	Adafruit_NeoPixel::Color(0,0,255) },
	{ Adafruit_NeoPixel::Color(255,0,255),	Adafruit_NeoPixel::Color(255,0,255) } };
void sprite( int n ) {
	for( int strip=0; strip<NUM_STRIPS; strip++ ) {
		for( int i=0; i<sizeof(SPRITES)/sizeof(*SPRITES); i++ )
			strips[strip].setPixelColor( n+i, SPRITES[i][strip] );
		strips[strip].show();
	}
}

void halves( unsigned long offset ) {
	static const uint32_t colorss[][NUM_STRIPS] = {
	  { Adafruit_NeoPixel::Color(0,0,255),	Adafruit_NeoPixel::Color(255,255,0) },
	  { Adafruit_NeoPixel::Color(0,255,0),	Adafruit_NeoPixel::Color(255,0,255) },
	  { Adafruit_NeoPixel::Color(255,0,0),	Adafruit_NeoPixel::Color(0,255,255) } };
	for( int strip=0; strip<NUM_STRIPS; strip++ ) {
		for( unsigned int i=0; i<NUM_PIXELS; i++ ) {
			int idx = (offset+i) % (3*NUM_PIXELS/2) / (NUM_PIXELS/2);
			strips[strip].setPixelColor( i, colorss[idx][strip] );
		}
		strips[strip].show();
	}
}

void report() {
	Serial.print("After ");
	Serial.print(loopcount);
	Serial.print("loops");
	Serial.println();
}

int sprite_pos = 0;
void loop() {
	time = millis();
	//halves( loopcount );
	sprite( sprite_pos );
	sprite_pos = (sprite_pos+1)%NUM_PIXELS;
	if( time > next_report_time ) {
		report();
		next_report_time += REPORT_INTERVAL;
	}
	loopcount++;
	delay(10);
}
