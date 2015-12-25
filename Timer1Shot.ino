
#define OSP_SET_WIDTH(cycles) (OCR1A = 0xffff-(cycles-1))

// Setup the one-shot pulse generator and initialize with a pulse width that is (cycles) clock counts long

void osp_setup(uint16_t cycles) {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  TCCR1B = 0;    // Halt counter by setting clock select bits to 0 (No clock source).
  TCCR1A = 0;    // This keeps anyhting from happening while we get set up

  TCNT1 = 0;     // Start counting at bottom.
   
  OCR1B = 0;  // Set TOP to 0. This effectively keeps us from counting because the counter just keeps reseting back to 0.
  OCR1A = 0;  // We break out of this by manually setting the TCNT higher than 0, in which case it will count all the way up to MAX and then overflow back to 0 and get locked up again.
  OSP_SET_WIDTH(cycles);    // This also makes new OCR values get loaded frm the buffer on every clock cycle. 

  TCCR1A = _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1) | _BV(WGM10) | _BV(WGM11); // OC1B=Set on Match, clear on BOTTOM. Mode 15 Fast PWM.
  TCCR1B = _BV(WGM12)| _BV(WGM13) | _BV(CS10);         // Start counting now. WGM22=1 to select Fast PWM mode 15
  
 }

// Setup the one-shot pulse generator

void osp_setup() {

  osp_setup(1);

}

// Fire a one-shot pulse. Use the most recently set width. 

#define OSP_FIRE() (TCNT1 = OCR1A - 1)

// Test there is currently a pulse still in progress

#define OSP_INPROGRESS() (TCNT1>0)

// Fire a one-shot pusle with the specififed width. 
// Order of operations in calculating m must avoid overflow of the unint16_t.
// TCNT1 starts one count lower than the match value becuase the chip will block any compare on the cycle after setting a TCNT. 

#define OSP_SET_AND_FIRE(cycles) { uint16_t m = 0xffff-(cycles-1); OCR1A=m;TCNT1= m-1;}


void setup()
{
  osp_setup();
}

void loop()
{
  // Step though 0-19 cycle long pulses for demo purposes 

  for (uint16_t o = 0; o < 10000; o+=100) {
    OSP_SET_AND_FIRE(o);
    while (OSP_INPROGRESS());         // This just shows how you would wait if necessary - not necessary in this application. 
    delay(1000);      // Wait a sec to let the audience clap
  }
}
