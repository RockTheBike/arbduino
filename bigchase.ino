#define VERSION "Chase lots of lights"

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

unsigned long time = 0;
unsigned long tickcount = 0;
#define REPORT_INTERVAL 1000
unsigned long next_report_time = 0;


#define PIN_PIXEL 11
#define OTHER_PIN_PIXEL 12
#define NUM_PIXELS 60
Adafruit_NeoPixel strip( NUM_PIXELS, PIN_PIXEL, NEO_GRB|NEO_KHZ800 );
Adafruit_NeoPixel otherstrip( NUM_PIXELS, OTHER_PIN_PIXEL, NEO_GRB|NEO_KHZ800 );


void setup() {
	Serial.begin(57600);
	Serial.println(VERSION);
	strip.begin();
	strip.show(); // Initialize all pixels to 'off'
	otherstrip.begin();
	otherstrip.show(); // Initialize all pixels to 'off'
}

void draw_strip( int spot ) {
	for( int i=0; i<NUM_PIXELS; i++ )
		if( i==spot )
			strip.setPixelColor( i, 255, 255, 0 );
		else
			strip.setPixelColor( i, 0, 0, 0 );
	strip.show();
}

void draw_bits( unsigned long n ) {
	static const uint32_t dark = Adafruit_NeoPixel::Color(0,0,0);
	static const uint32_t bright = Adafruit_NeoPixel::Color(255,255,255);
	for( int i=0; i<sizeof(n)*8; i++ )
		strip.setPixelColor( i, n>>i & 1 ? bright : dark );
	strip.show();
}

void fast_chase( int n ) {
	strip.setPixelColor( (n+1)%NUM_PIXELS, 255, 255, 0 );
	strip.setPixelColor( n, 0, 0, 0 );
	strip.show();
}

const uint32_t SPRITE[] = {
	Adafruit_NeoPixel::Color(0,0,0),
	Adafruit_NeoPixel::Color(255,0,0),
	Adafruit_NeoPixel::Color(255,255,0),
	Adafruit_NeoPixel::Color(0,255,0),
	Adafruit_NeoPixel::Color(0,255,255),
	Adafruit_NeoPixel::Color(0,0,255),
	Adafruit_NeoPixel::Color(255,0,255) };
void sprite( int n ) {
	for( int i=0; i<sizeof(SPRITE)/sizeof(*SPRITE); i++ )
		strip.setPixelColor( n+i, SPRITE[i] );
	strip.show();
}

void halves( unsigned long offset ) {
	static const uint32_t colors[] = {
	  Adafruit_NeoPixel::Color(0,0,255),
	  Adafruit_NeoPixel::Color(0,255,0),
	  Adafruit_NeoPixel::Color(255,0,0) };
	for( unsigned int i=0; i<NUM_PIXELS; i++ ) {
		int idx = (offset+i) % (3*NUM_PIXELS/2) / (NUM_PIXELS/2);
		strip.setPixelColor( i, colors[idx] );
	}
	strip.show();
}

void otherhalves( unsigned long offset ) {
	static const uint32_t colors[] = {
	  Adafruit_NeoPixel::Color(0,128,128),
	  Adafruit_NeoPixel::Color(128,128,0),
	  Adafruit_NeoPixel::Color(128,0,128) };
	for( unsigned int i=0; i<NUM_PIXELS; i++ ) {
		int idx = (offset+i) % (3*NUM_PIXELS/2) / (NUM_PIXELS/2);
		otherstrip.setPixelColor( i, colors[idx] );
	}
	otherstrip.show();
}

void report() {
	Serial.print("Still alive at ");
	Serial.print(tickcount);
	Serial.println("...");
}

void loop() {
	time = millis();
	//draw_strip( tickcount%NUM_PIXELS );
	//draw_bits( tickcount );
	//fast_chase( tickcount%NUM_PIXELS );
	//sprite( tickcount%NUM_PIXELS );
	halves( tickcount );
	otherhalves( tickcount );
	if( time > next_report_time ) {
		report();
		next_report_time = time + REPORT_INTERVAL;
	}
	tickcount++;
	delay(300);
}
