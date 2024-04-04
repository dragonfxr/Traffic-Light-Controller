/* Task 3 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define ms_per_inc 0.016
#define ms_per_loop 4.096

volatile double loops_since_LB1; 
volatile double kmh_speed;
volatile double total_elapsed;
bool LB1pressed; 
double val;

void setup()
{  
  //outputs setup
  DDRB |= (1<<DDB1); //Oscilloscope
  DDRC |= (1<<DDB3); //LED2
  DDRC |= (1<<DDB4); //LED1
  
  cli();
  
  EIMSK |= (1<<INT0) | (1<<INT1); //enable interrupt for LB1 (PB2/IO2);
  EICRA |= (1<<ISC00) | (1<<ISC01) | (1<<ISC10) | (1<<ISC11); // detect rising edge (PB3/IO3);

  //8bit time initialisation
  TCCR2A = 0x00; // normal mode
  TCCR2B = 0x00;
  TCNT2 = 0x00; 
  
  TCCR2B |= (1<<CS22) | (1<<CS21)| (0<<CS20); //prescale = 256
  TIMSK2 |= (1<<TOIE2);//enable TC2 overflow interrupt
  
  //initialisation
  TCCR1A = 0x00; // normal mode
  TCCR1B = 0x00;
  TCNT1 = 0x00; 

  TCCR1B |= (1<<CS12) | (0<<CS11) | (0<<CS10); //prescale = 256;

  TIMSK1 |= (1<<OCIE1A); //enable compare A;
  TIMSK1 |= (1<<OCIE1B); //enable compare B;
	
  OCR1B = 62500; //1s duration
  OCR1A = 0;
  
  sei();
  
  //LB1 and LB2 on
  PORTC |= (1<<PC3); //LED2
  PORTC |= (1<<PC4); //LED1
  
  LB1pressed = false;
    
}

void loop()
{
	asm volatile ("nop"); 
}


//Push button LB1
ISR(INT0_vect) {
    TCNT2 = 0x00; //start timer/counter2
  	loops_since_LB1 = 0x00;
  	LB1pressed = 1;
  
  	//Turn off LB1
  	PORTC &= ~(1<<PC4);

}

//PushButton LB2
ISR(INT1_vect) {
  
  //Turn off LB2
  PORTC &= ~(1<<PC3);
  
  total_elapsed = (ms_per_loop*loops_since_LB1) + (ms_per_inc * TCNT2);
  
  kmh_speed = 20/total_elapsed*3600;
  
  if (kmh_speed > 100){
    OCR1A = 0; // if travelling 100km/hr +, PWM remains at 100% duty
  }
  else{ 
    val = (kmh_speed/100)/(ms_per_inc/1000);
    OCR1A = val;
  }
  
  //start the new pwm speed display
  TCNT1=0x00;
  
  //first high of pwm
  PORTB |= (1<<PB1);
}

ISR(TIMER2_OVF_vect){
  
  if (LB1pressed) {
    if (loops_since_LB1 <65536) { //to avoid 16 bit from overflowing
    	loops_since_LB1++;
    }
  }
}

// pwm counters
ISR(TIMER1_COMPA_vect){
 
  if (OCR1A != 0) {
	PORTB &= ~(1<<PB1); //signal goes low
  }
}


//PWM 1s period timer
ISR(TIMER1_COMPB_vect){
  
  //turn the blue lights back on
  PORTC |= (1<<PC4);
  PORTC |= (1<<PC3);
  
  TCNT1 = 0x0000;

  if (OCR1A != 0) {
    PORTB |= (1<<PB1); //signal goes high;
  }

}