const int hardwareCounterPin = 5;   // input pin fixed to internal Timer
const int samplePeriod = 1000;   // the sample period in milliseconds


void setup()
{
  noInterrupts(); 

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
  interrupts();  
  
  Serial.begin(115200);
  Serial.println("Pulse Counter");
  Serial.println("Hours,  Counts/hour, total counts since restart");

 
}  

//called when timer reaches the compare match setting
//won't happen as long as it's being set to 0 every second in the main loop
ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
   Serial.println("Compare match");
}


// every 65536 counts
//won't happen as long as it's being set to 0 in the loop() every second

ISR(TIMER1_OVF_vect)          
{
   Serial.println("Timer Overflow");
}

void loop()
{
  unsigned long start;
  unsigned long elapsed;
  unsigned long total=0L;
  unsigned long counts_per_hour;
  float hours;
  const unsigned long millis_per_hour = (1000*60*60);
  start = millis();
  
  for(;;) {
  // start the counting
   delay(samplePeriod);
  
  total +=TCNT1;
  TCNT1 = 0;
  
  elapsed = millis()-start;
  
  hours = ((float)(millis()-start))/(3600000.0F);//decimal hours since start
  counts_per_hour = total/hours;
  
  Serial.print(hours);
  Serial.print(",");
  Serial.print(counts_per_hour);
  Serial.print(",");
  Serial.println(total);
  
  }
}


