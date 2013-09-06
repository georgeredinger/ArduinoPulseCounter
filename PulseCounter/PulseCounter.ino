#include <LiquidCrystal.h>
//#include <LCDKeypad.h>

const int hardwareCounterPin = 5;   // input pin fixed to internal Timer
const int samplePeriod = 1000;   // the sample period in milliseconds
const int RedLED = 13;
const int GreenLED = 12;
unsigned long count = 0L;
unsigned long lastcount = 0;
unsigned long lastminutes = 0;

LiquidCrystal lcd( 8, 9, 4, 5, 6, 7 );
volatile const uint8_t sensPin = 0;
volatile uint8_t analog_reference = DEFAULT;

volatile unsigned long IRQ8PrevTime;
volatile int prevVal = 0;
volatile uint8_t irqFlag = 0;

void setup()
{
  noInterrupts(); 

  PCICR |= (1 << PCIE1);      // enable pin change interrupt for PCINT14..8
  PCMSK1 |= (1 << PCINT8);  // Enable Pin Change interrupt for ADC0 (analog PIN 0) INT8
  IRQ8PrevTime = millis();    // Hold current time

  bitSet(TCCR1B ,CS12);  // Counter Clock source is external pin
  bitSet(TCCR1B ,CS11);  // Clock on rising edge  
  TCCR1A=0;        // reset timer/counter control register A
  interrupts();  

  LiquidCrystal lcd( 8, 9, 4, 5, 6, 7 );
  lcd.begin(16, 2);
  lcd.setCursor( 0, 0 );
  lcd.print("Welcome");


}  

ISR(PCINT1_vect) {
  int curVal;

  long unsigned IRQ8ActTime = millis();

  if (IRQ8ActTime - IRQ8PrevTime > 1) {
    ADMUX = (analog_reference << 6) | (sensPin & 0x07);  // ADMUX - ADC Mulitplexer Selection Register
    ADCSRA |= (1<<ADSC);                                 // ADCSRA: Bit 6 – ADC Start Conversion
    while (bit_is_set(ADCSRA, ADSC));                    // While conversion in progress loop
    uint8_t low = ADCL;                                  // read low value
    uint8_t high = ADCH;                                 // read high value
    curVal = (high << 8) | low;                          // combine low + high

    if (curVal != prevVal) {
      IRQ8PrevTime = IRQ8ActTime;
      prevVal = curVal;
      irqFlag = 1;
    }
    if(prevVal > 250) {
      count++;
    }
  }
}


void loop()
{
  unsigned long minutes;
  unsigned long rate;
  const unsigned long millis_per_hour = (1000*60*60);

  minutes = millis() / 60000.0;

  if((minutes != lastminutes) | (count != lastcount) ){
    lastcount=count;
    lastminutes = minutes;
    if(count > 0) {
      if(minutes>0) {
        rate = count/minutes;
      }
      else{
        rate=0;
      }
    }
    else {
      rate=0;
    }
    lcd.setCursor( 0, 0 );
    lcd.print("                    ");  
    lcd.setCursor( 0, 0 );
    lcd.print((long) minutes);
    lcd.setCursor(5,0);
    lcd.print(rate);
    lcd.setCursor(10,0);
    lcd.print(count);
  }
}



