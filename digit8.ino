#include "display.h"

#define LINE_BUF_SIZE 16

long s = 12345678;
char buf[LINE_BUF_SIZE];
int ind = 0;

void Set_timer_2() {
  //set timer2 interrupt at 64Hz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 16;//249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 1024 prescaler
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

}

ISR(TIMER2_COMPA_vect) { //Display Service
  refresh_display();
  TCNT2 = 0;
}

void setup() {
  init_display();
  Set_timer_2();
  Serial.begin(19200);

  display_number(s);


}

void loop() {
  delay(10);
  read_line();

  //animate_points2();
  //display_date(20, 04, 25);
  //delay(1000);
  //s++;
  //display_clock(16, 13, s);
  //delay(1000);
}


//String line;
void read_line() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '<')
    {
      buf[0] = c;
      ind = 1;
    }
    else if ( (c == 'S' || c == 'T' || c == 'D' || c == 'C') && ind == 1)
    {
      buf[1] = c;
      ind = 2;
    }
    else if (c == '>' && ind > 3)
    {
      buf[ind] = 0;// string terminate
      parse_line();
    }
    else
    {
      if (ind <  LINE_BUF_SIZE - 1)
      {
        buf[ind] = c;
        ind ++;
      }
    }
  }
}

void parse_line()
{
  if (buf[0] == '<')
  {
    switch ( buf[1])
    {
      case 'S':
        {
          long i;
          sscanf(&buf[2], "%ld", &i);
          Serial.println(i);
          if (i > 0)
          {
            s = i;
            display_number(i);
          } else
          {
            display_number(10);
          }
          break;
        }
      case 'C':
        {
          int h, m, s;
          sscanf(&buf[2], "%d:%d:%d", &h, &m, &s);
           Serial.print(h);
           Serial.print(":");
           Serial.print(m);
           Serial.print(":");
           Serial.println(s);
          if (h >= 0 && m >= 0 && s >= 0)
          {
            display_clock(h, m, s);
          }
          break;
        }
        case 'D':
        {
          int y, m, d;
          sscanf(&buf[2], "%d/%d/%d", &y, &m, &d);
          if (y >= 0 && m >= 0 && d >= 0)
          {
            display_date(y, m, d);
          }
          break;
        }
        case 'T':
        {
          int i;
          sscanf(&buf[2], "%d", &i);
          if (i > 0)
          {
            display_temp(i);
          } 
          break;
        }
    };

  }
}
