const int hardwareCounterPin = 5;   // input pin fixed to internal Timer


const int samplePeriod = 1000;   // the sample period in milliseconds

#define ledPin 13

void setup()
{
  noInterrupts(); 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

//enable this block if you want to an interupt every 100 counts and counter reset to 0:w
//  OCR1A = 100;            // compare match register 16MHz/256/2Hz
//  TCCR1B |= (1 << WGM12);   // CTC mode
//  TCCR1B |= (1 << CS12);    // 256 prescaler 
//  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt


  bitSet(TCCR1B ,CS12);  // Counter Clock source is external pin
  bitSet(TCCR1B ,CS11);  // Clock on rising edge  
  TCCR1A=0;        // reset timer/counter control register A

  Serial.begin(115200);
  Serial.println("Pulse Counter");
    Serial.println("Counts/hour, total counts since restart");
  interrupts();  
 
}  


ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
   Serial.println("Compare match");
   digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
   count += 100;
     
}


// every 65536 counts
ISR(TIMER1_OVF_vect)          
{
   Serial.println("Timer Overflow");
}

void loop()
{
  unsigned long start;
  unsigned long elapsed;
  unsigned long total=0L;
  
  start = millis();
  for(;;) {
  // start the counting
   delay(samplePeriod);
  
  total +=TCNT1
  TCNT1 = 0;
  elapsed = millis()-start;
  Serial.print((total/elapsed)/(1000*60*60));
  Serial.print(",");
  Serial.println(total);
  
  }
}


