// Capital letter is sharp
// Normal letter is normal
// A = 440 is in the third octave


#define BASE_OCTAVE 3
#define OCTAVE_MULTIPLIER 2
#define HALF_STEP 1.059463094359295
#define OUTPUT_PIN 8
#define LED_PIN 13
#define BASE_A 440


char* song1 = "e3d1c2d2e2e2e4d2d2d4e2g2g4e3d1c2d2e2e2e2e2d2d2e2d2c4_4\0";

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
}

void play_note(char note, unsigned int octave, int duration_ms);

void loop() {
  delay(1000);
  parse_song(song1);
}

void parse_song(char* song) {
  int octave = 3;
  int base_duration = 200;
  int duration = base_duration;
  
  // If the character is a number, skip, because the previous
  // letter should have taken care of the instruction
  for (unsigned int i=0; i<1000; i++) {
    char command = song[i];
    char param = song[i+1];
    if (command == '\0' || param == '\0') return;
    
    if (command == ' ') continue;
    if (param == 'O') {
      octave = param;
    } else if (command == '_') {
      duration = base_duration * param;
      play_rest(duration);
    } else if ((command >= 'a' && command <= 'g') || (command >= 'A' && command <= 'G')) {
      if (param < '1' || param > '9') continue;
      param = param - '1' + 1;
      duration = base_duration * param;
      play_note(command, octave, duration);
    }
  }
}

void play_rest(int duration_ms) {
  delay(duration_ms+100);
}

void play_note(char note, unsigned int octave, int duration_ms) {
  boolean sharp = (note - 'a' < 0);
  int half_steps[7] = {0, 2, -9, -7, -5, -4, -2};
  
  // Convert note to lower case
  if (sharp) note += 'a' - 'A';
  int hs_index = note - 'a';
  int hs = half_steps[hs_index];
  
  // Take into account the octave
  double freq = BASE_A;
  freq *= pow(OCTAVE_MULTIPLIER, 
  octave - BASE_OCTAVE);
  
  if (hs > 0) {
    for (int i=0; i<hs; i++) {
      freq *= HALF_STEP;
    }
  } else if (hs < 0) {
    for (int i=0; i>hs; i--) {
      freq /= HALF_STEP;
    }
  }
  
  int int_freq = int(freq);
  tone(OUTPUT_PIN, int_freq, duration_ms);
  delay(duration_ms + 100);
}

