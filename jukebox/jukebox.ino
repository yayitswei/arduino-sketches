// Capital letter is sharp, lowercase letter is normal, A = 440 Hz is in the third octave
#include "songs.h"

#define OCTAVE_MULTIPLIER 2
#define HALF_STEP 1.059463094359295
#define OUTPUT_PIN 8
#define LED_PIN 13
#define BASE_A 440
#define TEMPO_PIN A1
#define MODE_ROTATE -1
#define MIDDLE_OCTAVE 3

int g_song_num = MODE_ROTATE;
int g_song_num_changed = false;

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT); 
  attachInterrupt(0, change_song, RISING);
}

void loop() {
  static int i = 0; // current song number
  parse_song(songs[i]);
  i = (g_song_num == MODE_ROTATE) ? i+1 : g_song_num;
  i = i % g_num_songs;
  delay(1000);
}

void parse_song(char* song) {
  // If the character is a number, skip, because the previous
  // letter should have taken care of the instruction
  for (unsigned int i=0; i<1000; i++) {
    char command = song[i];
    char param = song[i+1];
    if (command == '\0' || param == '\0') return;
    if (command == ' ' || param < '1' || param > '9') continue;
    
    param = param - '1' + 1;
    
    if (g_song_num_changed) {
      g_song_num_changed = false;
      break;
    }

    // Read tempo
    int base_tempo_ms = analogRead(TEMPO_PIN)/10 + 100; // Minimum will be 100 ms    
    process_command(command, param, base_tempo_ms * param);
  }
}

void process_command(char command, int param, int tempo) {
  static unsigned int octave = 3;
  if (command == 'O') {
    octave = param;
  } else if (command == '_') {
    play_rest(tempo);
  } else if ((command >= 'a' && command <= 'g') || (command >= 'A' && command <= 'G')) {
    play_note(command, octave, tempo);
  }
}

void play_rest(int duration_ms) {
  delay(duration_ms+20);
}

void play_note(char note, unsigned int octave, int duration_ms) {
  int sharp = (note - 'a' < 0) ? 1 : 0; // Check if letter is capital
  if (sharp) note += 'a' - 'A'; // Convert note to lower case
  
  int half_steps[7] = {0, 2, -9, -7, -5, -4, -2};
  int hs = half_steps[note - 'a']; // number of half steps from 'a' to the correct note
  
  // Take into account the octave, and then shift frequency by the correct number of half steps
  double freq = BASE_A * pow(OCTAVE_MULTIPLIER, octave - MIDDLE_OCTAVE) * pow(HALF_STEP, hs) * (HALF_STEP * sharp);
  
  digitalWrite(LED_PIN, HIGH); // Visual representation of a note being played
  tone(OUTPUT_PIN, int(freq), duration_ms);
  
  delay(duration_ms);
  digitalWrite(LED_PIN, LOW);
  
  delay(20);
}

void change_song() {
  g_song_num = (g_song_num == g_num_songs-1) ? MODE_ROTATE : g_song_num+1;
  g_song_num_changed = true;
}

