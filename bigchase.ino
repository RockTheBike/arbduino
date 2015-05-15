#define VERSION "BRX lights v1.1"

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define ONLY_PLUS  // assume minus rail is 0V

unsigned long time = 0;
unsigned long loopcount = 0;
#define REPORT_INTERVAL 1000
unsigned long next_report_time = 0;

#define AVG_CYCLES 50 // average measured values over this many samples

// Voltages:
#define NUM_RAILS 2
#define VOLTCOEFF 13.179  // larger number interprets as lower voltage
const int VOLT_PRE_OFFSET[NUM_RAILS] = { 0, 1023 };
const int SCALE[NUM_RAILS] = { 1, -1 };
const float VOLT_POST_OFFSET[NUM_RAILS] = { 0, -5 };
#define VOLTPIN A0 // Voltage Sensor Pin
#define MINUSVOLTPIN A1 // Voltage Sensor Input for arbduino v2
const int VOLTPINS[NUM_RAILS] = { VOLTPIN, MINUSVOLTPIN };
int voltsAdc[NUM_RAILS];
float voltsAdcAvg[NUM_RAILS];
// volts[0] holds plus rail, volts[1] holds minus rail but should be positive
float volts[NUM_RAILS] = { 0, 0 };

// pretty lights
#define NUM_STRIPS 2
const int PIN_PIXELS[] = { 12, 13 };
#define NUM_PIXELS 150
Adafruit_NeoPixel strips[] = {
	Adafruit_NeoPixel( NUM_PIXELS, PIN_PIXELS[0], NEO_GRB|NEO_KHZ800 ),
	Adafruit_NeoPixel( NUM_PIXELS, PIN_PIXELS[1], NEO_GRB|NEO_KHZ800 ) };

#define LOW_VOLTAGE 28.0


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
	  { Adafruit_NeoPixel::Color(  0,  0,255),	Adafruit_NeoPixel::Color(255,  0,255) },
	  { Adafruit_NeoPixel::Color(  0,255,255),	Adafruit_NeoPixel::Color(  0,  0,255) },
	  { Adafruit_NeoPixel::Color(  0,255,  0),	Adafruit_NeoPixel::Color(  0,255,255) },
	  { Adafruit_NeoPixel::Color(255,255,  0),	Adafruit_NeoPixel::Color(  0,255,  0) },
	  { Adafruit_NeoPixel::Color(255,  0,  0),	Adafruit_NeoPixel::Color(255,255,  0) },
	  { Adafruit_NeoPixel::Color(255,  0,255),	Adafruit_NeoPixel::Color(255,  0,  0) } };
	static const int num_colors = sizeof(colorss)/sizeof(*colorss);
	for( int strip=0; strip<NUM_STRIPS; strip++ ) {
		for( unsigned int i=0; i<NUM_PIXELS; i++ ) {
			// cycle in NUM_PIXELS*num_colors
			int idx = (offset+i) % (NUM_PIXELS*num_colors*2/3) * 3/2/NUM_PIXELS;
			strips[strip].setPixelColor( i, colorss[idx][strip] );
		}
		strips[strip].show();
	}
}

void blink_low( unsigned long time ) {
	static const uint32_t red = Adafruit_NeoPixel::Color(255,0,0);
	static const uint32_t black = Adafruit_NeoPixel::Color(0,0,0);
	uint32_t color = time%1000 > 500 ? red : black;
	for( int strip=0; strip<NUM_STRIPS; strip++ ) {
		for( unsigned int i=0; i<NUM_PIXELS; i++ )
			strips[strip].setPixelColor( i, color );
		strips[strip].show();
	}
}

void report() {
	Serial.print("After ");
	Serial.print(loopcount);
	Serial.print("loops ");
	Serial.print( volts[0] );
	Serial.print( "+" );
	Serial.print( volts[1] );
	Serial.print( "=" );
	Serial.print( volts[0] + volts[1] );
	Serial.print( "V" );
	Serial.println();
}

int sprite_pos = 0;
void loop() {
	time = millis();

	getVolts();

	if( volts[0]+volts[1] < LOW_VOLTAGE ) {
		blink_low(time);
	} else {
		halves( loopcount );
		//sprite( sprite_pos );
	}

	sprite_pos = (sprite_pos+1)%NUM_PIXELS;
	if( time > next_report_time ) {
		report();
		next_report_time += REPORT_INTERVAL;
	}
	loopcount++;
	delay(10);
}

void getVolts(){
#ifdef ONLY_PLUS
  const int max_rail = 1;
#else
  const int max_rail = 2;
#endif
  for( int rail=0; rail<max_rail; rail++ ) {
    voltsAdc[rail] = analogRead( VOLTPINS[rail] );
    voltsAdcAvg[rail] = average( voltsAdc[rail], voltsAdcAvg[rail] );
    volts[rail] =
      adc2volts( VOLT_PRE_OFFSET[rail] + SCALE[rail]*voltsAdcAvg[rail] ) +
      VOLT_POST_OFFSET[rail];
  }
}
float average(float val, float avg){
  if(avg == 0)
    avg = val;
  return (val + (avg * (AVG_CYCLES - 1))) / AVG_CYCLES;
}
float adc2volts(float adc){
  return adc * (1 / VOLTCOEFF);
}
