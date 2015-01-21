#define VERSION "Chase lots of lights"

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

unsigned long time = 0;
unsigned long loopcount = 0;
#define REPORT_INTERVAL 1000
unsigned long next_report_time = 0;


#define PIN_PIXEL 13
#define NUM_PIXELS 150
Adafruit_NeoPixel strip( NUM_PIXELS, PIN_PIXEL, NEO_GRB|NEO_KHZ800 );


void setup() {
	Serial.begin(57600);
	Serial.println(VERSION);
	strip.begin();
	strip.show(); // Initialize all pixels to 'off'
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

int button_push_count = 0;
bool prev_button_state = 0;  // false=>closed; true=>open
bool check_button() {
	pinMode( PIN_PIXEL, INPUT_PULLUP );
	bool cur_button_state = digitalRead( PIN_PIXEL );
	bool released = cur_button_state && ! prev_button_state;
	if( released )
		button_push_count++;
	prev_button_state = cur_button_state;
	pinMode( PIN_PIXEL, OUTPUT );
	return( released );
}

void report() {
	Serial.print("After ");
	Serial.print(loopcount);
	Serial.print("loops:  button:");
	Serial.print(prev_button_state);
	Serial.print(", pushed ");
	Serial.print(button_push_count);
	Serial.print("times");
	Serial.println();
}

int sprite_pos = 0;
void loop() {
	time = millis();
	//draw_strip( loopcount%NUM_PIXELS );
	//draw_bits( loopcount );
	//fast_chase( loopcount%NUM_PIXELS );
	//halves( loopcount );
	sprite( sprite_pos );
	sprite_pos = check_button() ? 0 : (sprite_pos+1)%NUM_PIXELS;
	if( time > next_report_time ) {
		report();
		next_report_time += REPORT_INTERVAL;
	}
	loopcount++;
	delay(10);
}
