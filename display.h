#define CLOCK           A1  // SCK 
#define LATCH           A3  // DIO
#define DATA            A2  // RCK

#define MultiplexDelay  10  //delay for multiplexing between digit on 8x7segment module 


byte dec_h[8]  = {0};    //hour
byte dec_m[8]  = {0};    //month
byte dec_t[8]  = {0};    //temprature
byte display_counter = 0; //index

byte disp_0[8] = {0};
byte disp_1[8] = {0};
byte disp[8] = {0};

#define DIGIT_BLANK 11
#define DIGIT_POINT 12
#define DIGIT_DEGREE 14


byte anode[8]     = {
  0b00010000,  //digit 1 from right
  0b00100000,  //digit 2 from right
  0b01000000,  //digit 3 from right
  0b10000000,  //digit 4 from right

  0b00000001,  //digit 5 from right
  0b00000010,  //digit 6 from right
  0b00000100,  //digit 7 from right
  0b00001000   //digit 8 from right
};
byte cathode[41]  = {
  0b11000000,  // 0
  0b11111001,  // 1
  0b10100100,  // 2
  0b10110000,  // 3
  0b10011001,  // 4
  0b10010010,  // 5
  0b10000010,  // 6
  0b11111000,  // 7
  0b10000000,  // 8
  0b10010000,  // 9
  //symbols map
  0b10111111,  //10  -
  0b11111111,  //11 blank
  0b01111111,  //12 .
  0b11110110,  //13 :
  0b10100101,  //14 °
  
	0b11001111,//A 65 - 50 = 15
	0b10101011,//B
	0b10101100,//C
	0b11100011,//D
	0b10101110,//E
	0b10001110,//F
	0b01101111,//G
	0b11001011,//H
	0b10001000,//I
	0b01100001,//J
	0b10101010,//K
	0b10101000,//L
	0b00100110,//M
	0b10000011,//N
	0b10100011,//O
	0b11001110,//P
	0b01001111,//Q
	0b10001100,//R
	0b00101111,//S
	0b01000101,//T
	0b10100001,//U
	0b11101001,//V
	0b10110001,//W
	0b11001011,//X
	0b01101011,//Y
	0b11110110//Z 89
};

//   --    =    3
//  |  |   =  4   1
//   --    =    6
//  |  |   =  5   2
//   --  . =    0    7
//
void display8x7segment(byte datapin, byte clockpin, byte latchpin, byte digit, byte number) {
  shiftOut(datapin, clockpin, MSBFIRST, digit); // clears the right display
  shiftOut(datapin, clockpin, MSBFIRST, number); // clears the left display
}

void combine_digits()
{
  disp_0[4] = disp[0];
  disp_0[5] = disp[1];
  disp_0[6] = disp[2];
  disp_0[7] = disp[3];

  disp_1[4] = disp[4];
  disp_1[5] = disp[5];
  disp_1[6] = disp[6];
  disp_1[7] = disp[7];
}


void animate_points()
{
	for(int i = 0; i< 8; i++)
	{
		disp[i] = 11;
	}
	combine_digits();
	for(int i = 0; i< 8; i++)
	{
		disp[i] = 12;
		combine_digits();
		delay(50);
	}
	for(int i = 0; i< 8; i++)
	{
		disp[i] = 11;
		combine_digits();
		delay(50);
	}
}



void animate_points2()
{
	for(int i = 0; i< 8; i++)
	{
		disp[i] = 11;
	}
	combine_digits();
	for(int i = 3; i>=0; i--)
	{
		disp[4+i] = 12;
		disp[3-i] = 12;
		
		combine_digits();
		delay(50);
	}
	for(int i = 3; i>=0; i--)
	{
		disp[4+i] = 11;
		disp[3-i] = 11;
		combine_digits();
		delay(50);
	}
	
	
}



//76543210
void display_clock(byte h, byte m, byte s) {

  disp[7] = h / 10;
  disp[6] = h % 10;

  disp[4] = m / 10;
  disp[3] = m % 10;

  disp[1] = s / 10;
  disp[0] = s % 10;

  if (s % 2 == 0)
  {
    disp[5] = 11; //blank
    disp[2] = 11; //blank
  }
  else
  {
    disp[5] = 12; //.
    disp[2] = 12; //.
  }
  
  
  
   combine_digits();

}

void display_date(byte y, byte m, byte d) {

  disp[7] = y / 10;
  disp[6] = y % 10;

  disp[4] = m / 10;
  disp[3] = m % 10;

  disp[1] = d / 10;
  disp[0] = d % 10;

  disp[5] = 10; //-
  disp[2] = 10; //-
  
   combine_digits();
}



void display_number(long v) {
  long value = v;
  for (int i = 0; i < 8; i++)
  {
    disp[i] = value % 10;
    value = value / 10;
  }

  combine_digits();
}


void display_temp(int v) {
  int value = abs(v);
  dec_t[4] = value % 10;;
  value = value / 10;
  dec_t[5] = value % 10;
  value = value / 10;
  dec_t[7] = 11;//empty
  if (v < 0)
    dec_t[6] = 10;//-
  else
    dec_t[6] = 11;//empty
}

void refresh_display()
{
  byte cat_0 = cathode[disp_0[display_counter]];
  byte cat_1 = cathode[disp_1[display_counter]];

  digitalWrite(LATCH, LOW);
  display8x7segment(DATA, CLOCK, LATCH, anode[display_counter], cat_1 );
  display8x7segment(DATA, CLOCK, LATCH, anode[display_counter], cat_0 );
  digitalWrite(LATCH, HIGH);
  display_counter++;
  if (display_counter > 7)
    display_counter = 4;	
	
}
void init_display()
{
	pinMode(LATCH, OUTPUT);
	pinMode(CLOCK, OUTPUT);
	pinMode(DATA , OUTPUT);
	
}
