void setup() {
  pinMode(8, OUTPUT);
}

void play_note(char note, unsigned int octave, int duration_ms);

void loop() {
  unsigned int octave = 1;
  playnote('a', 3, 500);
  playnote('a', 3, 500);
  playnote('e', 3, 500);
  playnote('e', 3, 500);
  playnote('f', 3, 500);
  playnote('f', 3, 500);
  playnote('e', 3, 1000);
  
  playnote('d', 3, 500);
  playnote('d', 3, 500);
  playnote('c', 3, 500);
  playnote('c', 3, 500);
  playnote('b', 3, 500);
  playnote('b', 3, 500);
  playnote('a', 3, 1000);

  playnote('e', 3, 500);
  playnote('e', 3, 500);
  playnote('d', 3, 500);
  playnote('d', 3, 500);
  playnote('c', 3, 500);
  playnote('c', 3, 500);
  playnote('b', 3, 1000);

  playnote('e', 3, 500);
  playnote('e', 3, 500);
  playnote('d', 3, 500);
  playnote('d', 3, 500);
  playnote('c', 3, 500);
  playnote('c', 3, 500);
  playnote('b', 3, 1000);
  
  playnote('a', 3, 500);
  playnote('a', 3, 500);
  playnote('e', 3, 500);
  playnote('e', 3, 500);
  playnote('f', 3, 500);
  playnote('f', 3, 500);
  playnote('e', 3, 1000);
  
  playnote('d', 3, 500);
  playnote('d', 3, 500);
  playnote('c', 3, 500);
  playnote('c', 3, 500);
  playnote('b', 3, 500);
  playnote('b', 3, 500);
  playnote('a', 3, 1000);
}

void playnote(char note, unsigned int octave, int duration_ms) {
  int freqs[7] = {220, 247, 277, 294, 330, 370, 415};
  int index = note - 'a';
  int freq = freqs[index];
  tone(8, freq*octave, duration_ms);
  delay(duration_ms + 100);
}
