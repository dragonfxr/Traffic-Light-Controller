#include <avr/io.h>
#include <avr/interrupt.h>


volatile uint8_t state = 1;
volatile uint8_t count = 1;
volatile uint8_t blue = 1;
volatile uint8_t ADCconversion;
volatile uint16_t count1 = 0;
bool pushed = false;
bool config = false;
volatile uint16_t num_loop = 0;

void setup()
{
  cli();
  
  //Open comm for serial debugging
  Serial.begin(9600);
 
  // set IO pins 10 (B2), 9 (B1), 8(B0) as outputs 
  DDRB |= (1<<DDB2); //Red
  DDRB |= (1<<DDB1); //Orange
  DDRB |= (1<<DDB0); //Green
  DDRC |= (1<<DDC1); //Blue, setting A1 as output
  
  //Initial light status
  PORTB |= (1 << PORTB2);//red light on
  PORTC |= (1 << PC1); // blue light on
  
  //set timer/counter2 to normal mode
  TCNT2 = 0x00;//initial value of the timer/counter0
  TCCR2A = 0;
  TCCR2B = 0;
  
  TCCR2B |= (1 << CS20) | (0 << CS21) | (1 << CS22);// prescaler of 128
  //TIMSK2 |= (1 << TOIE0);//enable the overflow interrupt
  TIMSK2 |= (1<<OCIE2A);
  OCR2A = 125;
  
  //set timer/counter1 to normal mode
  TCNT1 = 0x0000;//initial value of the timer/counter1
  TCCR1A = 0x0000;
  TCCR1B = 0x0000;
  
  OCR1A = 15625*state;//15625 is one second(by calculation)
  // OCR1A will change according to the state number
  TCCR1B |= (1 << CS10) | (0 << CS11) | (1 << CS12);// prescaler of 1024
  TIMSK1 |= (1 << OCIE1A);//enable the COMPA interrupt
  
  // Interrupt set up 
  EIMSK |= (1<<INT0);
  EICRA |= (1<<ISC00) | (1<<ISC01); //detect rising edge
  
  //ADC Set Up
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //8 prescaler
  ADCSRA |= (1 << ADEN)| (1<<ADIE); // enable ADC and its conversion interrupt  
  ADMUX |= (1 << REFS0); //use 5V-vcc as reference voltage
 
  sei();
}

void loop()
{
 asm volatile ("nop");  
}
  
  
//state cycling ISR 
ISR(TIMER1_COMPA_vect){
  
  //if button is pressed on red, stay red and enter config
  if (pushed == true && count == 1) {
    PORTB = (1 << PB2);
    count = 3;
    config = true;
  }
  
  // matches and changes colour
  if(count == 1){
    PORTB &= ~(1 << PB2);//red light off
    PORTB |= (1 << PB0);//green light

    count++;
  }
  else if (count == 2){
    PORTB &= ~(1 << PB0);//green light off
    PORTB |= (1 << PB1);// orange light
    
    count++;
  }
  else if (count == 3 && pushed == true){
    PORTB &= ~(1 << PB1);//orange light off
    PORTB |= (1 << PB2);// red light
    
    config = true;    
    count = 3;
  }
  else {
    PORTB &= ~(1 << PB1);//orange light off
    PORTB |= (1 << PB2);// red light
    
    count = 1;
  }
    
  // reset the timer/counter1 to initial value
  TCNT1 = 0x0000;
  
}

//occurs every 1ms
//blue light control
ISR(TIMER2_COMPA_vect){
    TCNT2 = 0x00;

  //if(config == false){
  //  PORTC |= (1 << PC1);//blue on
  //} else
  if (config == true){
    if(num_loop < 500/state){
      num_loop++;//by calculation. The number of loops increases every 1ms
    }
    else{
      num_loop = 0;
      
      if(blue % 2 == 1 && blue < state * 2){// if it's less than one second
        PORTC |= (1 << PC1); //turn blue light on
        blue++;
      }
      else if ((blue < state *6)){// turn the blue off for the following 2 seconds
        PORTC &= ~(1 << PC1); //switch the blue light off
        blue++;
      }
      else if (blue == state *6){//reach the end of the period
        PORTC &= ~(1 << PC1); //switch the blue light off
        blue = 1;
      }
    }
    
  }
}

// pushbutton ISR
ISR(INT0_vect){
  if (pushed == false) { // entering config
  	pushed = true; // indicates that the button is pressed
    TCNT2 = 0x00;

  } else {
    if (count == 3){ // exiting config and setting state 
     ADCSRA |= (1 << ADSC); // perform ADC conversion
    }
  }
  
  blue = 2;// reset the blue to 2 make sure that the blue light is off once the putton is pressed again.

} 
 
// potentiometer ISR
ISR (ADC_vect) {
    
  //Map digital words to state
  //change states according to the ADC value
    if (ADC < 256){
      	state = 1;// update the state number to global 
    } else if (ADC < 512) {
      	state = 2;
    } else if (ADC < 768) {
        state = 3;
    } else {
        state = 4; 
    } 
  
    pushed = false;// once the button is pushed for the second time
    // change the state of 'pushed' to false
    config = false;// exit the config mode
	PORTC = (1 << PC1); // blue remains on
    TCNT1 = 0x0000;// reset the timer to zero
  
  	//update timer according to new state  
    OCR1A = 15625*state;
    count = 1;//make sure that when exiting the config mode,
    // the red light is on first.
}
  
  