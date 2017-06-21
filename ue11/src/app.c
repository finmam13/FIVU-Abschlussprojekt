#include "global.h"

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "app.h"
#include "mon.h"
#include "sys.h"

// defines
// ...


// declarations and definations

volatile struct App app;


// functions


void app_init (void)
{
  memset((void *)&app, 0, sizeof(app));
  ADMUX = ((1<<REFS0) | (1<<ADLAR)); //AREF, ADLAR links od rechtsbündig
  ADCSRA =((1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)); // ADC enable, Prescaler
  ADCSRA |= (1<<ADSC); //start
}


//--------------------------------------------------------

void app_main (void)
{
}

//--------------------------------------------------------

void app_task_1ms (void) 
{
  app.adc_voltage = ADCH;
  ADCSRA |= (1<<ADSC);
}

void app_task_2ms (void) {}
void app_task_4ms (void) {}
void app_task_8ms (void) {}
void app_task_16ms (void) {}
void app_task_32ms (void) {}
void app_task_64ms (void) {}
void app_task_128ms (void) {}

