/* Task 4 */

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t count;
volatile uint8_t currentLight;
volatile uint8_t carsCaught;
volatile bool captureBreach;
volatile uint8_t pulse;
volatile uint8_t timer2Loops;
volatile uint8_t timer0Loops;
volatile uint8_t signal_flag;

void setup() 
{
  // set IO pins 10 (B2), 9 (B1), 8(B0) as outputs 
  DDRB |= (1<<DDB2);
  DDRB |= (1<<DDB1);
  DDRB |= (1<<DDB0); 
   
  DDRC |= (1<<DDC4); //Blue, setting A1 as output
  
  // Push button interrupt set up 
  EIMSK |= (1<<INT0);
  EICRA |= (1<<ISC00) | (1<<ISC01); //detect rising edge
 
	//Variable initialisation
	count = 1;
	currentLight = 1;
	carsCaught = 0;
	captureBreach = false;
	pulse = 0;
	timer2Loops = 0;
	timer0Loops = 0;
	signal_flag = 0;
  
  // Timer for cycling traffic lights  
  TCNT1 = 0x0000;// buttom value of timer/counter1
  TCCR1A = 0;// normal mode to emulate CTC mode
  TCCR1B = 0;
   
  OCR1A = 15625;// make sure that timer/counter1 matches COMPA every second
  // note that the clock frequency is 16MHz and timer/conuter1 is 16-bit.
  
  PORTB |= (1 << PB2);//red light should be on first
  TCCR1B |= (1 << CS10) | (1 << CS12) | (0 << CS11);// prescaler of 1024
  TIMSK1 |= (1 << OCIE1A);//enable the COMPA interrupt
   
  //8-bit timer for blue image capture LED
  TCCR2A = 0x00; // normal mode to emulate CTC mode
  TCCR2B = 0x00;
  TCNT2 = 0x00; 
  
  OCR2A = 124;
  TCCR2B |= (1<<CS22) | (1<<CS21)| (0<<CS20); //prescaler = 256
  TIMSK2 |= (1<<OCIE1A); //enable compare A interrupt
  
  //8-bit timer for the oscilloscope
  TCCR0A = 0x00; // normal mode to emulate CTC mode
  TCCR0B = 0x00;
  TCNT0 = 0x00; 
  
  OCR0A = 125;
  TCCR0B |= (1<<CS02) | (0<<CS01)| (0<<CS00); //prescaler = 256
  TIMSK0 |= (1<<OCIE0A); //enable compare A interrupt
  
   //Starts PWM signal if inititial cars caught value was larger than 0
  if (carsCaught != 0) {
    PORTB |= (1 << PB5);// turn the pwm to high
  }
}

void loop()
{
  asm("nop");
}

ISR(TIMER0_COMPA_vect){
  timer0Loops++;
  
  // 5 loops is equivalent to the passing of 10ms
  if(timer0Loops == 5){
    
 	//at the end of 1s, PWM starts again at high
    if (signal_flag == 100 && carsCaught != 0 && carsCaught < 100) {
       PORTB |= (1 << PB5);// turn the pwm to high
       signal_flag = 0;
    // forms the duty cycle based on carsCaught
    } else if (signal_flag == carsCaught && carsCaught < 100){
       PORTB &= ~(1 << PB5);//assign low to the pwm
       signal_flag++;
    } else {
        signal_flag++; // counts the num of increments of 1ms passed
    }
    
      timer0Loops = 0; 
  }
   TCNT0 = 0x00;
}


ISR(TIMER1_COMPA_vect){
  if(count == 1){
    PORTB &= ~(1 << PB2);//red light off
    PORTB |= (1 << PB0);//green light on
    currentLight = 3;
    count++;// every time the timer count to 1 second, add one to the variable
  }
  else if (count == 2){
    PORTB &= ~(1 << PB0);//green light off
    PORTB |= (1 << PB1);// orange light on
    currentLight = 2;
    count++;
  }
  else{
    PORTB &= ~(1 << PB1);//orange light off
    PORTB |= (1 << PB2);// red light on
    currentLight = 1;
    count = 1;// reaches the end of the loop, go back to the original point
  }
 
   // reset the timer/counter1 to initial value
  TCNT1 = 0x0000;
}

// car passed button is pressed
ISR(INT0_vect){
  
  if (currentLight == 1){ //red light
    captureBreach = true;
    carsCaught++;
    
    //set up for blue light
    PORTC |= (1 << PC4); // turn blue light on
    TCNT2 = 0x0000; // reset timer for blue light
    pulse = 0; 
    timer2Loops = 0;
    
    //turn PWM high for first 
    if (carsCaught == 1) {
      PORTB |= (1 << PB5);// turn the pwm to high
      TCNT1 = 0x0000;
      signal_flag = 0;
    }
  }
} 

//blue light flashing
ISR(TIMER2_COMPA_vect){
  timer2Loops++;
  if (timer2Loops > 125) {         
      if (captureBreach == true){
        if (pulse == 0) {
          PORTC &= ~(1 << PC4); //blue light off
          pulse++;
        } else if (pulse == 1) {
          PORTC |= (1 << PC4); //blue light on
          pulse++;
        } else {
          PORTC &= ~(1 << PC4); //blue light off
          captureBreach = false;
        }
      }
    timer2Loops = 0;
  }
  TCNT2= 0x00;
}
