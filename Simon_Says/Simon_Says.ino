
// max value from Analog-digital converter
# define MAX_ADC_VALUE 1023

// setup led pins
# define LED_0  P1_6
# define LED_1  P2_5
# define LED_2  P2_4
# define LED_3  P2_3

// setup push-button pins
# define PUSH_0 P1_2
# define PUSH_1 P1_3
# define PUSH_2 P1_4
# define PUSH_3 P1_5

// setup start push-button pin
# define PUSH_START  P2_0

// setup pot pin for change speed
# define POT    P1_0

// setup buzzer pin
# define BUZZER P1_7

// array for leds and push-buttons
const int leds[]    = {LED_0, LED_1, LED_2, LED_3};
const int buttons[] = {PUSH_0, PUSH_1, PUSH_2, PUSH_3};
byte sequence[10];
int sequence_max;
int sequence_index;
int sequence_count;
int speed_leds;
boolean start;
boolean wait_for_answer;

void fill_sequence() {
  randomSeed(analogRead(P1_0));
  for(int i = 0; i < sizeof(sequence)/sizeof(byte); i++) {
    sequence[i] = (byte)random(100)%4;
    if(i != 0 && sequence[i-1] == sequence[i]) {
      i--;
    }
  }
  sequence_index = 0;
}

void blinkLed(int led, int time) {
   digitalWrite(led, HIGH);
   delay(time);
   digitalWrite(led, LOW);
}

void play_sequence() {
  for(int i = 0; i < sequence_max; i++) {
    blinkLed(leds[sequence[i]], 100);
    delay(speed_leds);
  }
}

void test() {
  for(int i = 0; i < 4; i++) {
    digitalWrite(leds[i], HIGH); 
  }
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  for(int i = 0; i < 4; i++) {
    digitalWrite(leds[i], LOW); 
  }
}

void reset() {
  fill_sequence();
  wait_for_answer = false;
  sequence_index = 0;
}

void setup() {
  // set pin leds as OUTPUTS
  for(int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW); 
  }
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  // set pin buttons as Input with PULL DOWN
  for(int i = 0; i < 4; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  
  pinMode(PUSH_START, INPUT_PULLUP);
  pinMode(POT, INPUT);
  
  // setup the speed of leds
  int pot_val = analogRead(POT);
  speed_leds = pot_val*(4800/MAX_ADC_VALUE) + 250;
  
  sequence_max = 4;
  sequence_count = 0;
  start = false;
  reset();
  test();
}

void loop() {
  if(start){
    if(wait_for_answer) {
      // if the user complete the sequence add 1 to the counter
       if(sequence_index == sequence_max) {
         sequence_count++;
         digitalWrite(BUZZER, HIGH);
         delay(500);
         digitalWrite(BUZZER, LOW);
         reset();
       }
       // if the user complete 3 times the sequence add 1 led more to the sequence
       if(sequence_count == 3) {
         sequence_count = 0;
         sequence_max++;
         reset();
       }
      
      // check the inputs with sequence
      for(int i = 0; i < 4; i++) {
       if(digitalRead(buttons[i]) == LOW) {
         blinkLed(leds[i], 50);
         if(i == sequence[sequence_index]) {
           sequence_index++;
         } else {
           // the player lose
           digitalWrite(BUZZER, HIGH);
           delay(2000);
           digitalWrite(BUZZER, LOW);
           reset();
           start = false;
           test();
         }
       }
      }
      delay(100);
    } else {
      play_sequence();
      wait_for_answer = true;
    }
  } 
  if(digitalRead(PUSH_START) == LOW) {
    start = !start;
    sequence_max = 4;
    reset();
  } 
  // read value from pot and calculate the speed between 250ms to 5000ms
  int pot_val = analogRead(POT);
  speed_leds = pot_val*(4800/MAX_ADC_VALUE) + 250;
}
