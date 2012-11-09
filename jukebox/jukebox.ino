// Capital letter is sharp
// Normal letter is normal
// A = 440 is in the third octave


#define OCTAVE_MULTIPLIER 2
#define HALF_STEP 1.059463094359295
#define OUTPUT_PIN 8
#define LED_PIN 13
#define BASE_A 440
#define TEMPO_PIN A1
#define MODE_ROTATE -1


char* songs[] = {"O4c1c1c1c3  O3G3A3  O4c1_1O3A1O4c3_6  _6_6\0",
                 "e3 d1 c2 d2 e2 e2 e4 d2 d2 d4 e2 g2 g4 e3 d1 c2 d2 e2 e2 e2 e2 d2 d2 e2 d2 c4 _4",
                 "O3 g1 _1 g1 a3 g3 O4 c3 O3 b3 _3 | O3g1_1g1a3g3O4d3c3_3  O3g1_g1O4g3e3c3O3b3a3_3_3  O4f1_1f1e3c3d3c3_3_3_3_3",
               
                 "O3e2g1f5O2b2O3d1c5  O4c1_1c1O3b5a1_1a1g5  O3e2g1f5O2b2O3d1c5  O4c1_1c1O3b2a1g1_1a1b2_1O4c1O3g1e1c1",
                 "O3_1_1g1F1f1D2e2O2G1a1c2a1O3c1d1  _1_1g1F1f1D2e2c1_1c1c1_3  O3_1_1g1F1f1D2e2O2G1a1c2a1O3c1d  _1D3d3c3_2_4",
                 "O3d5f1a1O4c1O3b1O4c1O3b3a1f1e1  O3d5f1a1O4c1O3b1O4c1O3b3a1b1O4c1  O4d5f1a1O4c1O3b1O4c1O3b3a1f1e1  O3d5f1a1O4c1e1e1d1g3_2",
                 "O3g2O4e2d2c2O3g4_2g1g1  g2O4e2d2c2O3a4_2a1a1  a2O4f2e2d2O3b4  O4g2g2f2d2e4_4  O3g2O4e2d2c2O3g4_2g1g1  g2O4e2d2c2O3a4_2a1a1  a2O4f2e2d2g2g2g2g2  a2g2f2d2c4_4 _4_4",
               
                 "O3e2e2e2_2  e2e2e2_2  e2g2c3d1e4_4  f2f2f2_1f1  f2e2e2_1e1  e2d2d2e2d4g4  O3e2e2e2_2  e2e2e2_2  e2g2c3d1e4_4  f2f2f2_1f1  f2e2e2e1e1  g2g2a2b2O4c2_2  O2c1_3",
                 "O2c3d3D3e3  f1g1f4  _6  O2c3d3D3e3  f1g1f4  _6  O3c6  O2b6  O3d6c6  c3O2b3a3g3  e1d1c4  _6"};

int g_num_songs = 9;
int g_song_num = MODE_ROTATE;
int g_song_num_changed = false;

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT); 
  
  attachInterrupt(0, change_song, RISING);
  Serial.begin(9600);
}

void play_note(char note, unsigned int octave, int duration_ms);

void loop() {
  static int i = 0; // current song number
  Serial.print(songs[i]);
  
  parse_song(songs[i]);
  i = (g_song_num == MODE_ROTATE) ? i+1 : g_song_num;
  i = i % g_num_songs;
  Serial.print(i); Serial.print(" ");
  Serial.println(g_song_num);
  delay(1000);
}

void parse_song(char* song) {
  int octave = 3;
  int base_duration = 100;
  int duration = base_duration;
  
  // If the character is a number, skip, because the previous
  // letter should have taken care of the instruction
  for (unsigned int i=0; i<1000; i++) {
    char command = song[i];
    char param = song[i+1];
    if (command == '\0' || param == '\0') return;
    if (command == ' ') continue;
    
    if (param < '1' || param > '9') continue;
    param = param - '1' + 1;
    
    // Read tempo
    
    base_duration = analogRead(TEMPO_PIN)/10 + 100; // Minimum will be 100 ms
    
    if (g_song_num_changed) {
      g_song_num_changed = false;
      break;
    }
      
    if (command == 'O') {
      octave = param;
    } else if (command == '_') {
      duration = base_duration * param;
      play_rest(duration);
    } else if ((command >= 'a' && command <= 'g') || (command >= 'A' && command <= 'G')) {
      duration = base_duration * param;
      play_note(command, octave, duration);
    }
  }
}

void play_rest(int duration_ms) {
  delay(duration_ms+20);
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
  freq *= pow(OCTAVE_MULTIPLIER, octave)/8.0;
  
  if (hs > 0) {
    for (int i=0; i<hs; i++) {
      freq *= HALF_STEP;
    }
  } else if (hs < 0) {
    for (int i=0; i>hs; i--) {
      freq /= HALF_STEP;
    }
  }
  
  if (sharp) freq *= HALF_STEP;
  
  int int_freq = int(freq);
  digitalWrite(LED_PIN, HIGH);
  tone(OUTPUT_PIN, int_freq, duration_ms);
  delay(duration_ms);
  digitalWrite(LED_PIN, LOW);
  delay(20);
}

void change_song() {
  g_song_num = (g_song_num == g_num_songs-1) ? MODE_ROTATE : g_song_num+1;
  g_song_num_changed = true;
  Serial.println(g_song_num);
  
}

