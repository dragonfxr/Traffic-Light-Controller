#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t count = 1;//count is a variable that indicates the current colour of the light

void setup() 
{
  // set IO pins 10 (B2), 9 (B1), 8(B0) as outputs 
  DDRB |= (1<<DDB2);
  DDRB |= (1<<DDB1);
  DDRB |= (1<<DDB0); 
   
   
  TCNT1 = 0x0000;// initialise the buttom value of timer/counter1
  TCCR1A = 0;// set it to normal mode to emulate CTC mode
  TCCR1B = 0;
   
  OCR1A = 15625;// make sure that timer/counter1 matches COMPA every second
  // note that the clock frequency is 16MHz and timer/conuter1 is 16-bit.
  
  PORTB |= (1 << PB2);//red light should be on first
  TCCR1B |= (1 << CS10) | (1 << CS12) | (0 << CS11);// prescaler of 1024
  TIMSK1 = (1 << OCIE1A);//enable the COMPA interrupt
}

void loop()
{
  asm ("nop");
}

ISR(TIMER1_COMPA_vect){
  if(count == 1){
    PORTB = (1 << PB0);//green light on
    // don't have to turn the red of, because PORTB = (1 << PB0)
    // automatically set the other bits comprising the port b
    // data register to 0.
    count++;// every time the timer count to 1 second, add one to the variable
  }
  else if (count == 2){
    PORTB = (1 << PB1);// orange light on
    count++;// looping
  }
  else{
    PORTB = (1 << PB2);// red light on
    count = 1;// reaches the end of the loop, go back to the original point
  }
  // reset the timer/counter1 to initial value
  TCNT1 = 0x0000;
}



