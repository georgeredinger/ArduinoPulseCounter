#include <LiquidCrystal.h>
//#include <LCDKeypad.h>
#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define LCD_BACKLIGHT_PIN         10  // D10 controls LCD backlight
// ADC readings expected for the 5 buttons on the ADC input
#define RIGHT_10BIT_ADC           0  // right
#define UP_10BIT_ADC            145  // up
#define DOWN_10BIT_ADC          329  // down
#define LEFT_10BIT_ADC          505  // left
#define SELECT_10BIT_ADC        741  // right
#define BUTTONHYSTERESIS         10  // hysteresis for valid button sensing window
//return values for ReadButtons()
#define BUTTON_NONE               0  // 
#define BUTTON_RIGHT              1  // 
#define BUTTON_UP                 2  // 
#define BUTTON_DOWN               3  // 
#define BUTTON_LEFT               4  // 
#define BUTTON_SELECT             5  // 
//some example macros with friendly labels for LCD backlight/pin control, tested and can be swapped into the example code as you like
#define LCD_BACKLIGHT_OFF()     digitalWrite( LCD_BACKLIGHT_PIN, LOW )
#define LCD_BACKLIGHT_ON()      digitalWrite( LCD_BACKLIGHT_PIN, HIGH )
#define LCD_BACKLIGHT(state)    { if( state ){digitalWrite( LCD_BACKLIGHT_PIN, HIGH );}else{digitalWrite( LCD_BACKLIGHT_PIN, LOW );} }
int buttonJustPressed  = false;         //this will be true after a ReadButtons() call if triggered
int buttonJustReleased = false;         //this will be true after a ReadButtons() call if triggered
int buttonWas          = BUTTON_NONE;   //used by ReadButtons() for detection of button events

const int hardwareCounterPin = 5;   // input pin fixed to internal Timer
const int samplePeriod = 1000;   // the sample period in milliseconds
const int RedLED = 13;
const int GreenLED = 12;
unsigned long count = 0L;
unsigned long lastcount = 0;
unsigned long lastminutes = 0;

volatile const uint8_t sensPin = 0;
//volatile uint8_t analog_reference = DEFAULT;

volatile int prevVal = 0;
volatile uint8_t irqFlag = 0;

const char key_map[]={' ','R','U','D','L','S'};

LiquidCrystal lcd( 8, 9, 4, 5, 6, 7 );

void setup()
{
  noInterrupts(); 
    PCICR |= (1 << PCIE1);     // enable pin change interrupt for PCINT14..8
    PCMSK1 |= (1 << PCINT9);   // Enable Pin Change interrupt for ADC0 (analog PIN 1) INT9
  interrupts();  
  
  lcd.begin(2,16);
  lcd.setCursor( 0, 0 );
  lcd.print("Min: Rate: Tot:");
}  

ISR(PCINT1_vect) {
  count += digitalRead(A1);
  //  ADMUX = (DEFAULT << 6) | (sensPin & 0x07);  // ADMUX - ADC Mulitplexer Selection Register
  //  ADCSRA |= (1<<ADSC);                                 // ADCSRA: Bit 6 – ADC Start Conversion
  //  while (bit_is_set(ADCSRA, ADSC));                    // While conversion in progress loop
  //  count += (ADCH == 3);
  }

/*--------------------------------------------------------------------------------------
  ReadButtons()
  Detect the button pressed and return the value
  Uses global values buttonWas, buttonJustPressed, buttonJustReleased.
--------------------------------------------------------------------------------------*/
int ReadButtons()
{
   unsigned int buttonVoltage;
   byte button = BUTTON_NONE;   // return no button pressed if the below checks don't write to btn
 
   //read the button ADC pin voltage
   buttonVoltage = analogRead( BUTTON_ADC_PIN );
   //sense if the voltage falls within valid voltage windows
   if( buttonVoltage < ( RIGHT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_RIGHT;
   }
   else if(   buttonVoltage >= ( UP_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( UP_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_UP;
   }
   else if(   buttonVoltage >= ( DOWN_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( DOWN_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_DOWN;
   }
   else if(   buttonVoltage >= ( LEFT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( LEFT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_LEFT;
   }
   else if(   buttonVoltage >= ( SELECT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( SELECT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_SELECT;
   }
   //handle button flags for just pressed and just released events
   if( ( buttonWas == BUTTON_NONE ) && ( button != BUTTON_NONE ) )
   {
      //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
      //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
      buttonJustPressed  = true;
      buttonJustReleased = false;
   }
   if( ( buttonWas != BUTTON_NONE ) && ( button == BUTTON_NONE ) )
   {
      buttonJustPressed  = false;
      buttonJustReleased = true;
   }
 
   //save the latest button value, for change event detection next time round
   buttonWas = button;
 
   return( button );
}

void loop()
{
  unsigned long minutes;
  unsigned long rate;
  int key;
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
    lcd.setCursor( 0, 1 );
    lcd.print("                    ");  
    lcd.setCursor( 0, 1 );
    lcd.print(minutes);
    lcd.setCursor(5,1);
    lcd.print(rate);
    lcd.setCursor(11,1);
    lcd.print(count);
  }
    key=ReadButtons();
    if(key !=0 ){
      lcd.setCursor(15,1);
      lcd.print(key_map[key]);
    }
  }



