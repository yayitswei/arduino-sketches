// Capital letter is sharp, lowercase letter is normal, A = 440 Hz is in the third octave
#include <stdlib.h>
#include <string.h>
#include "songs.h"

// #define DEBUG

#define OCTAVE_MULTIPLIER 2
#define HALF_STEP 1.059463094359295
#define OUTPUT_PIN 8
#define LED_PIN 13
#define BASE_A 440
#define TEMPO_PIN A1
#define MODE_ROTATE -1
#define MIDDLE_OCTAVE 3
#define MODE_ABC 0
#define MODE_CLJ 1
#define MODE_BAD -1
#define MIDI_CONSTANT 8.1757989156

#define NUM_WORDS 3
#define MAX_WORD_LENGTH 12
#define MAX_SONG_LENGTH 512
#define MAX_NOTES 42

int g_song_num = MODE_ROTATE;
int g_song_num_changed = false;

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT); 
  attachInterrupt(0, change_song, RISING);

  #ifdef DEBUG
    Serial.begin(9600);
  #endif
}

void loop() {
  static int i = 0; // current song number
  parse_song(songs[i]);
  i = (g_song_num == MODE_ROTATE) ? i+1 : g_song_num;
  #ifdef DEBUG
    Serial.print("Song num: "); Serial.println(i);
  #endif
  i = i % g_num_songs;
  delay(1000);
}

void parse_song(char* song) {
  // Songs that
  int mode = get_song_mode(song);

  if (mode == MODE_CLJ) {
    #ifdef DEBUG
      Serial.println("CLJ mode!");
    #endif
    parse_clj(song);
  } else if (mode == MODE_ABC) {
    #ifdef DEBUG
      Serial.println("ABC mode");
    #endif
    parse_abc(song);
  } // Skip songs that have MODE_BAD
}

int get_song_mode(char* song) {
  char command = song[0];
  char param = song[1];
  if (command == '\0' || param == '\0') return MODE_BAD;
  if (command == 'M') {
    return (param == '0') ? MODE_ABC : MODE_CLJ;
  } else {
    return MODE_ABC;
  }
}

void parse_abc(char* song) {
  // If the character is a number, skip, because the previous
  // letter should have taken care of the instruction
  for (unsigned int i=0; i<MAX_SONG_LENGTH; i++) {
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

void parse_clj(const char* song_orig) {
  char song[MAX_SONG_LENGTH];
  #ifdef DEBUG
    Serial.println("Copying song");
  #endif
  strcpy(song, song_orig);

  // Not terribly efficient, but loads the file first before playing anything
  char* tokens[MAX_NOTES];

  #ifdef DEBUG
    Serial.println("Tokenizing");
  #endif
  tokens[0] = strtok(song, " ");
  if (tokens[0] == NULL) return;
  for (int i=1; i<MAX_NOTES; i++) {
    tokens[i] = strtok(NULL, " ");
    if (tokens[i] == NULL) break;
    #ifdef DEBUG
      Serial.println(tokens[i]);
    #endif
  }
  #ifdef DEBUG
    Serial.println("Done parsing.");
  #endif

  // Now the tokens should be loaded
  for (int i=0; i<MAX_NOTES; i++) {
    if (tokens[i] == NULL) break;

    if (g_song_num_changed) {
      g_song_num_changed = false;
      break;
    }

    int freq, duration_ms, vol;
    boolean split_success = split_clj(tokens[i], freq, duration_ms, vol);
    if (!split_success) continue;

    #ifdef DEBUG
      Serial.print(i); Serial.print(" Note split: "); Serial.print(freq); Serial.print("\t"); Serial.print(duration_ms); Serial.print("\t"); Serial.println(vol);
    #endif

    play_clj(freq, duration_ms, vol);
  }
}

// Volume is not implemented yet
boolean split_clj(const char* triplet_orig, int& freq, int& duration_ms, int& volume) {
  char triplet[MAX_WORD_LENGTH];
  strcpy(triplet, triplet_orig);

  char* tokens[NUM_WORDS];
  tokens[0] = strtok(triplet, ",");
  if (tokens[0] == NULL) return false;
  freq = atoi(tokens[0]);

  tokens[1] = strtok(NULL, "/");
  if (tokens[1] == NULL) return false;
  duration_ms = atoi(tokens[1]);

  tokens[2] = strtok(NULL, "*"); // Shouldn't find it
  if (tokens[2] == NULL) return true;
  volume = atoi(tokens[2]);

  return true;
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

void play_rest(int duration_ms_ms) {
  make_tone(0, duration_ms_ms, 0);
}

void play_note(char note, unsigned int octave, int duration_ms_ms) {
  int sharp = (note - 'a' < 0) ? 1 : 0; // Check if letter is capital
  if (sharp) note += 'a' - 'A'; // Convert note to lower case
  
  int half_steps[7] = {0, 2, -9, -7, -5, -4, -2};
  int hs = half_steps[note - 'a']; // number of half steps from 'a' to the correct note
  
  // Take into account the octave, and then shift frequency by the correct number of half steps
  double freq = BASE_A * pow(OCTAVE_MULTIPLIER, (float)octave - MIDDLE_OCTAVE) * pow(HALF_STEP, hs) * pow(HALF_STEP, sharp);
  #ifdef DEBUG
    Serial.print("Octave: "); Serial.print(octave); Serial.print("\tHalf steps: "); Serial.print(hs); Serial.print("\tSharp: "); Serial.print(sharp); Serial.print("\t");
  #endif
  make_tone(int(freq), duration_ms_ms, 1);
}

// IMPORTANT: Volume is currently unused
void play_clj(int midi_note, int duration_ms_ms, int volume) {
  double freq = pow(OCTAVE_MULTIPLIER, (midi_note / 12.0)) * MIDI_CONSTANT;
  make_tone(int(freq), duration_ms_ms, volume);
}

void change_song() {
  g_song_num = (g_song_num == g_num_songs-1) ? MODE_ROTATE : g_song_num+1;
  g_song_num_changed = true;
}

// IMPORTANT: Volume is currently unused
void make_tone(int freq, int duration_ms_ms, int volume) {
  #ifdef DEBUG
    Serial.print("Tone: "); Serial.print(freq); Serial.print("\t"); Serial.print(duration_ms_ms); Serial.print("\t"); Serial.println(volume);
  #endif
  digitalWrite(LED_PIN, HIGH); // Visual representation of a note being played
  tone(OUTPUT_PIN, freq, duration_ms_ms);

  delay(duration_ms_ms);
  digitalWrite(LED_PIN, LOW);
  delay(20);
}
