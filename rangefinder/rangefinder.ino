// #define DEBUG

#define SIG_PIN 7
#define PIEZO_PIN 8
#define OCTAVE_MULTIPLIER 2
#define HALF_STEP 1.059463094359295
#define MIDI_CONSTANT 8.1757989156
#define OUTPUT_PIN 8

#define TOLERANCE 5 // in mm
#define KEY_SIZE 12 // in mm

void setup() {
  Serial.begin(9600);
}

void loop() {
  static int cur_key = 0;

  send_pulse();
  long pulse_duration = read_pulse();

  long distance = duration_to_mm(pulse_duration);
  int key_num = mm_to_key(distance);
  
  // working in mm
  if (distance - key_to_mm(cur_key) < -TOLERANCE || distance - key_to_mm(cur_key) > KEY_SIZE + TOLERANCE) {
    cur_key = key_num;
  }
  
  if (key_num < 30) {
    play_clj(key_to_note_pentatonic(cur_key), 50, 13);
  }
  Serial.println(duration_to_mm(pulse_duration));
//  delay(20);
}

void send_pulse() {
  pinMode(SIG_PIN, OUTPUT);
  digitalWrite(SIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(SIG_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(SIG_PIN, LOW);
}

long read_pulse() {
  pinMode(SIG_PIN, INPUT);
  long duration = pulseIn(SIG_PIN, HIGH);
  return duration;
}

long duration_to_mm(long duration) {
  // Sound is 331.5 m/s
  // distance = duration * speed_of_sound / 2
  // duration is in microseconds
  return (duration * 3315 / 20000 );
}

long mm_to_key(long mm) {
  return (mm / KEY_SIZE); // truncates
}

long key_to_mm(int key_num) {
  return (key_num * KEY_SIZE); // lower bound
}

long key_to_note_major(int key_num) {
  int steps[7] = {0, 2, 4, 5, 7, 9, 11};

  int octave = (key_num / 7) + 5;
  int scale_tone = steps[key_num % 7];
  return (octave * 12 + scale_tone);
}

long key_to_note_blues(int key_num) {
  int steps[6] = {0, 3, 5, 6, 7, 10};
  int octave = (key_num / 6) + 5;
  int scale_tone = steps[key_num % 6];
  return (octave * 12 + scale_tone);
}

long key_to_note_pentatonic(int key_num) {
  int steps[5] = {0, 2, 4, 7, 9};
  int octave = (key_num / 5) + 5;
  int scale_tone = steps[key_num % 5];
  return (octave * 12 + scale_tone);
}

long key_to_note_chromatic(int key_num) {
  return (key_num + 60);
}


void play_clj(int midi_note, int duration_ms_ms, int volume) {
  double freq = pow(OCTAVE_MULTIPLIER, (midi_note / 12.0)) * MIDI_CONSTANT;
  make_tone(int(freq), duration_ms_ms, volume);
}

void make_tone(int freq, int duration_ms_ms, int volume) {
  #ifdef DEBUG
    Serial.print("Tone: "); Serial.print(freq); Serial.print("\t"); Serial.print(duration_ms_ms); Serial.print("\t"); Serial.println(volume);
  #endif
  tone(OUTPUT_PIN, freq, duration_ms_ms);

  delay(duration_ms_ms);
//  delay(10);
}

