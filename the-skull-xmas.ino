/*
           .--._.--.--.__.--.--.__.--.--.__.--.--._.--.
         _(_      _Y_      _Y_      _Y_      _Y_      _)_
        [___]    [___]    [___]    [___]    [___]    [___]
        /:' \    /:' \    /:' \    /:' \    /:' \    /:' \
       |::   |  |::   |  |::   |  |::   |  |::   |  |::   |
       \::.  /  \::.  /  \::.  /  \::.  /  \::.  /  \::.  /
        \::./    \::./    \::./    \::./    \::./    \::./
         '='      '='      '='      '='      '='      '='

     ___________.__               ___________         .__  .__
     \__    ___/|  |__   ____    /   _____/  | ____ __|  | |  |
       |    |   |  |  \_/ __ \   \_____  \|  |/ /  |  \  | |  |
       |    |   |   Y  \  ___/   /        \    <|  |  /  |_|  |__
       |____|   |___|  /\___  > /_______  /__|_ \____/|____/____/
                     \/     \/          \/     \/
                                                 Festive Edition


  This firmware is released under an open-curse license.
  If you don't play fair, you'll be punished. 👿

  https://skullctf.com/start

  Copyright (C) 2020, Uri Shaked. Holiday lights art by jgs.

*/

#include <avr/sleep.h>

int buzzerPin = PB0;
int led1Pin = PB4;
int led2Pin = PB3;
int touchPin = PB2;

typedef enum {
  PAUSE, C, D, E, F, G
} note;

int pitches[] = {
  0, 261, 293, 329, 349, 392
};

#define NOTE(note, len) (note | ((len - 1) << 4))
#define D_2 NOTE(D, 2)
#define E_2 NOTE(E, 2)
#define G_2 NOTE(G, 2)

const int playbackSpeed = 150; // Slower == faster

uint8_t tune[] = {
  E, E, E_2, E, E, E_2,
  E, G, C, D, E_2, NOTE(PAUSE, 2),
  F, F, F, F, F, E, E,
  E, D, D, E, D_2, G_2,
};

ISR(INT0_vect) {
  bitClear(GIMSK, INT0);  // Disable INT0 interrupt
  sleep_disable();
}

void sleep() {
  // Disable ADC - saves about 324.5uA in sleep mode!
  ADCSRA = 0;
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  noInterrupts();
  bitSet(GIMSK, INT0);
  interrupts();
  sleep_enable();
  sleep_cpu();
}

void beep(int freq, long ms) {
  long baseDelay = 1000000L / freq;
  long iterations = (ms * 1000) / (baseDelay * 2);
  digitalWrite(led1Pin, HIGH);
  for (int i = 0; i < iterations; i++) {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(baseDelay + i);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(baseDelay + i);
  }
  digitalWrite(led1Pin, LOW);
}

void setup() {
  pinMode(touchPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
}

void loop() {
  sleep();

  while (digitalRead(touchPin) == LOW) {
    uint8_t duty = random(99);
    for (int j = 0; j < 42; j++) {
      digitalWrite(led1Pin, HIGH);
      digitalWrite(led2Pin, HIGH);
      delayMicroseconds(duty * 2);
      digitalWrite(led1Pin, LOW);
      digitalWrite(led2Pin, LOW);
      delayMicroseconds((255 - duty) * 2);
    }
  }

  for (int i = 0; i < sizeof(tune); i++) {
    int pitch = pitches[tune[i] & 0xf];
    int duration = 1 + (tune[i] >> 4);
    beep(pitch, duration * playbackSpeed);
    if (i != sizeof(tune) - 1) {
      delay(duration * playbackSpeed);
    }
  }

  beep(10000, 200);
}
