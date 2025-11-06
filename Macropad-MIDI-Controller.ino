/*
 * Macropad MIDI Controller - v1.0
 */

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <hardware/flash.h>
#include <hardware/sync.h>

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

const byte PAD_PINS[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
const byte LED_PIN = 25;

enum PlayStyle {STYLE_SCALED, STYLE_CHROMATIC, STYLE_CHORD, STYLE_GENERATIVE, STYLE_OMNI_CHORD};

// 8 SCALES - Linear across 16 pads, no repeating notes
const int8_t SCALES[][16] = {
  {0,2,4,5,7,9,11,12,14,16,17,19,21,23,24,26},     // 0: Major (happy, classic)
  {0,2,3,5,7,8,10,12,14,15,17,19,20,22,24,26},     // 1: Minor (sad, emotional)
  {0,3,5,6,7,10,12,15,17,18,19,22,24,27,29,30},    // 2: Blues (soulful)
  {0,2,3,5,7,8,11,12,14,15,17,19,20,23,24,26},     // 3: Harmonic Minor (dramatic, exotic)
  {0,2,4,7,9,12,14,16,19,21,24,26,28,31,33,36},    // 4: Pentatonic Major (can't play wrong)
  {0,3,5,7,10,12,15,17,19,22,24,27,29,31,34,36},   // 5: Pentatonic Minor (easy improv)
  {0,2,4,6,7,9,11,12,14,16,18,19,21,23,24,26},     // 6: Lydian (bright, dreamy)
  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}          // 7: Chromatic (all notes)
};

const char* SCALE_NAMES[] = {"Major","Minor","Blues","Harmonic","Pent Maj","Pent Min","Lydian","Chromatic"};

// Map each chord mode to its ideal scale for the upper 8 pads (organized system)
const byte CHORD_MODE_SCALES[8] = {
  0,  // 0: Major -> Major scale
  1,  // 1: Minor -> Minor scale
  2,  // 2: Blues -> Blues scale
  3,  // 3: Harmonic -> Harmonic Minor scale
  0,  // 4: Open -> Major scale
  1,  // 5: Minor (diatonic) -> Minor scale
  0,  // 6: Jazz 7ths -> Major scale
  0   // 7: Diatonic -> Major scale
};

// 8 CHORD MODES - Reordered as requested
const int8_t CHORD_VOICINGS[][8][5] = {
  // 0. Major - Diatonic with bass roots (keep original - it's perfect!)
  {{-12,7,12,14,16},{-10,5,7,12,16},{-8,7,11,12,16},{-7,7,9,12,16},{-5,7,11,12,14},{-3,7,11,12,16},{-1,7,11,12,14},{0,7,12,14,16}},
  
  // 1. Minor - Natural minor with bass roots
  {{-12,3,7,12,15},{-10,5,10,14,17},{-9,3,7,12,15},{-7,5,10,12,17},{-5,7,12,14,19},{-4,3,8,12,15},{-2,5,10,14,17},{0,7,12,15,19}},
  
  // 2. Blues Chords (dominant 7ths with blue notes)
  {{0,4,7,10,127},{2,5,9,12,127},{4,7,11,14,127},{5,9,12,15,127},{7,11,14,17,127},{9,12,16,19,127},{10,14,17,20,127},{0,4,7,10,127}},
  
  // 3. Harmonic Minor (dramatic, exotic)
  {{0,3,7,12,127},{2,5,8,12,127},{3,7,11,14,127},{5,8,12,15,127},{7,11,14,17,127},{8,12,15,19,127},{11,14,17,20,127},{3,7,12,15,127}},
  
  // 4. Open Voicings (cinematic, wide)
  {{-12,0,7,16,19},{-10,2,9,17,21},{-8,4,11,19,23},{-7,5,12,21,24},{-5,7,14,23,26},{-3,9,16,24,28},{-1,12,19,26,31},{0,12,19,28,31}},
  
  // 5. Minor (diatonic) - Simple minor triads with close inversions
  {{0,7,12,15,127},{2,10,14,17,127},{3,7,10,15,127},{5,8,12,17,127},{7,10,14,19,127},{3,8,12,15,127},{5,10,14,17,127},{7,12,15,19,127}},
  
  // 6. Jazz 7ths - Rootless voicings (3rd, 7th, 9th, 5th)
  {{4,11,14,19,127},{5,12,14,21,127},{7,11,16,19,127},{9,12,16,21,127},{11,16,17,23,127},{12,16,19,24,127},{14,17,21,26,127},{16,19,23,28,127}},
  
  // 7. Diatonic - Standard close voicings, tight groupings
  {{0,4,7,12,127},{2,5,9,14,127},{4,7,11,16,127},{5,9,12,17,127},{7,11,14,19,127},{9,12,16,21,127},{11,14,17,23,127},{12,16,19,24,127}}
};

// 8-note voicings specifically for omni chord mode
const int8_t OMNI_CHORD_VOICINGS[][8] = {
  {-12,0,4,7,12,16,19,24},   // C Major expanded
  {-10,2,5,9,14,17,21,26},   // D minor expanded  
  {-8,4,7,11,16,19,23,28},   // E minor expanded
  {-7,5,9,12,17,21,24,29},   // F Major expanded
  {-5,7,11,14,19,23,26,31},  // G Major expanded
  {-3,9,12,16,21,24,28,33},  // A minor expanded
  {-1,11,14,18,23,26,30,35}, // B diminished expanded
  {0,12,16,19,24,28,31,36}   // C Major octave expanded
};

const char* CHORD_NAMES[] = {"Major","Minor","Blues","Harmonic","Open","Minor","Jazz 7ths","Diatonic"};

// Settings
PlayStyle playStyle = STYLE_SCALED;
byte currentScale = 0;
byte currentChordMode = 0;
int8_t transpose = 0;
int8_t octave = -1;
byte velocity = 100;
byte midiChannel = 1;
byte rootNote = 60;

// Use 127 as a marker for "no note" in chord voicings
const int8_t NO_NOTE = 127;

struct Preset {
  PlayStyle style;
  byte scale;
  byte chordMode;
  int8_t transpose;
  int8_t octave;
  byte velocity;
};

Preset presets[4];

// Flash storage (last 4KB of flash)
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define FLASH_MAGIC 0x4D494449  // "MIDI" in hex
struct FlashData {
  uint32_t magic;
  Preset presets[4];
};

// Generative
unsigned long lastGenNoteTime = 0;
unsigned long nextGenNoteDelay = 0;
byte currentGenNote = 255;
const unsigned long MIN_GEN_DELAY = 800;
const unsigned long MAX_GEN_DELAY = 3000;
const byte GEN_TRIGGER_CHANCE = 70;

// Omni Chord Mode
byte omniChordIndex = 0;  // Currently selected chord (bottom 8 pads)
byte omniChordNotes[8] = {255, 255, 255, 255, 255, 255, 255, 255};  // Track actual MIDI notes for each top pad


// Menu
bool inMenu = false;
byte currentPage = 1;
const byte TOP_ROW[4] = {12,13,14,15};
unsigned long topRowPressTime[4] = {0,0,0,0};
bool topRowPressed = false;
unsigned long padHoldStartTime[16] = {0};
const unsigned long PRESET_SAVE_HOLD_TIME = 2000;

bool padStates[16] = {false};
bool lastPadStates[16] = {false};
unsigned long lastDebounceTime[16] = {0};
const unsigned long DEBOUNCE_DELAY = 20;

void loadPresetsFromFlash() {
  const FlashData* flash_data = (const FlashData*)(XIP_BASE + FLASH_TARGET_OFFSET);
  
  if (flash_data->magic == FLASH_MAGIC) {
    // Valid data found, load it
    for (int i = 0; i < 4; i++) {
      presets[i] = flash_data->presets[i];
    }
  } else {
    // No valid data, use defaults
    presets[0] = {STYLE_SCALED, 0, 0, 0, -1, 100};
    presets[1] = {STYLE_CHROMATIC, 7, 0, 0, -1, 100};  // Chromatic scale
    presets[2] = {STYLE_CHORD, 0, 0, 0, -1, 100};
    presets[3] = {STYLE_GENERATIVE, 0, 0, 0, -1, 80};
  }
}

void savePresetsToFlash() {
  FlashData data;
  data.magic = FLASH_MAGIC;
  for (int i = 0; i < 4; i++) {
    data.presets[i] = presets[i];
  }
  
  uint8_t buffer[FLASH_PAGE_SIZE];
  memcpy(buffer, &data, sizeof(FlashData));
  
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_PAGE_SIZE);
  restore_interrupts(ints);
  
  // Flash LED to confirm
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_PIN, i % 2);
    delay(50);
  }
}

void setup() {
  usb_midi.setStringDescriptor("Macro MIDI Pad");
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i < 16; i++) {
    pinMode(PAD_PINS[i], INPUT_PULLUP);
  }
  
  // Load presets from flash
  loadPresetsFromFlash();
  
  // Load first preset
  loadPreset(0);
  
  // Flash LED
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void loop() {
  unsigned long now = millis();
  
  if (playStyle == STYLE_GENERATIVE && !inMenu) {
    updateGenerative(now);
  }
  
  for (int i = 0; i < 16; i++) {
    bool reading = (digitalRead(PAD_PINS[i]) == LOW);
    
    if (reading != lastPadStates[i]) {
      lastDebounceTime[i] = now;
    }
    
    if ((now - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if (reading != padStates[i]) {
        padStates[i] = reading;
        
        if (padStates[i]) {
          padHoldStartTime[i] = now;
          handlePadPress(i, now);
        } else {
          handlePadRelease(i, now);
          padHoldStartTime[i] = 0;
        }
      }
    }
    
    lastPadStates[i] = reading;
  }
  
  // Check for preset save
  if (inMenu && currentPage == 1) {
    for (int i = 0; i < 4; i++) {
      if (padStates[i] && padHoldStartTime[i] > 0) {
        unsigned long holdDuration = now - padHoldStartTime[i];
        if (holdDuration > PRESET_SAVE_HOLD_TIME && holdDuration < PRESET_SAVE_HOLD_TIME + 100) {
          savePreset(i);
        }
      }
    }
  }
  
  MIDI.read();
}

void handlePadPress(byte pad, unsigned long now) {
  for (int i = 0; i < 4; i++) {
    if (pad == TOP_ROW[i]) {
      topRowPressTime[i] = now;
      break;
    }
  }
  
  if (padStates[TOP_ROW[0]] && padStates[TOP_ROW[1]] && 
      padStates[TOP_ROW[2]] && padStates[TOP_ROW[3]]) {
    
    unsigned long minT = topRowPressTime[0], maxT = topRowPressTime[0];
    for (int i = 1; i < 4; i++) {
      if (topRowPressTime[i] < minT) minT = topRowPressTime[i];
      if (topRowPressTime[i] > maxT) maxT = topRowPressTime[i];
    }
    
    if ((maxT - minT) < 150) {
      toggleMenu();
      topRowPressed = true;
      return;
    }
  }
  
  if (inMenu) return;
  
  playNote(pad, true);
}

void handlePadRelease(byte pad, unsigned long now) {
  if (topRowPressed) {
    bool isTopRow = false;
    for (int i = 0; i < 4; i++) {
      if (pad == TOP_ROW[i]) {
        isTopRow = true;
        break;
      }
    }
    if (isTopRow) {
      bool allReleased = true;
      for (int i = 0; i < 4; i++) {
        if (padStates[TOP_ROW[i]]) {
          allReleased = false;
          break;
        }
      }
      if (allReleased) topRowPressed = false;
      return;
    }
  }
  
  if (inMenu) {
    handleMenuPress(pad);
  } else {
    playNote(pad, false);
  }
}

void handleMenuPress(byte pad) {
  if (currentPage == 1) {
    switch(pad) {
      case 0: loadPreset(0); break;
      case 1: loadPreset(1); break;
      case 2: loadPreset(2); break;
      case 3: loadPreset(3); break;
      case 4: velocity -= 20; if (velocity < 20) velocity = 20; break;
      case 5: velocity += 20; if (velocity > 127) velocity = 127; break;
      case 6: midiChannel++; if (midiChannel > 16) midiChannel = 1; break;
      case 7: resetToDefaults(); break;
      case 8: octave--; if (octave < -2) octave = -2; break;
      case 9: octave++; if (octave > 2) octave = 2; break;
      case 10: transpose--; if (transpose < -12) transpose = -12; break;
      case 11: transpose++; if (transpose > 12) transpose = 12; break;
      case 12: currentPage = 1; break;
      case 13: currentPage = 2; break;
      case 14: currentPage = 3; break;
      case 15: currentPage = 4; break;
    }
  }
  else if (currentPage == 2) {
    if (pad < 8) {
      currentScale = pad;
      playStyle = STYLE_SCALED;
    }
    else if (pad >= 12 && pad <= 15) {
      currentPage = pad - 11;
    }
  }
  else if (currentPage == 3) {
    if (pad < 8) {
      currentChordMode = pad;
      currentScale = CHORD_MODE_SCALES[currentChordMode];
      playStyle = STYLE_CHORD;
    }
    else if (pad >= 12 && pad <= 15) {
      currentPage = pad - 11;
    }
  }
  else if (currentPage == 4) {
    // Example modes page
    if (pad == 0) {
      // Example 1: Random note generator
      playStyle = STYLE_GENERATIVE;
    }
    else if (pad == 1) {
      // Example 2: Omni chord mode 
      playStyle = STYLE_OMNI_CHORD;
      currentScale = 0;  // Use major scale
    }
    else if (pad >= 12 && pad <= 15) {
      currentPage = pad - 11;
    }
  }
}

void playNote(byte pad, bool noteOn) {
  if (playStyle == STYLE_GENERATIVE) return;
  
  switch(playStyle) {
    case STYLE_SCALED:
      {
        byte note = calcScaledNote(pad);
        if (noteOn) MIDI.sendNoteOn(note, velocity, midiChannel);
        else MIDI.sendNoteOff(note, 0, midiChannel);
      }
      break;
      
    case STYLE_CHROMATIC:
      {
        byte note = rootNote + pad + transpose + (octave * 12);
        note = constrain(note, 0, 127);
        if (noteOn) MIDI.sendNoteOn(note, velocity, midiChannel);
        else MIDI.sendNoteOff(note, 0, midiChannel);
      }
      break;
      
    case STYLE_CHORD:
      playChord(pad, noteOn);
      break;
      
    case STYLE_OMNI_CHORD:
      playOmniChord(pad, noteOn);
      break;
  }
}

byte calcScaledNote(byte pad) {
  // Linear scales - directly use pad index (0-15) into the 16-note scale array
  int note = rootNote + SCALES[currentScale][pad] + transpose + (octave * 12);
  return constrain(note, 0, 127);
}

void playChord(byte pad, bool noteOn) {
  if (pad < 8) {
    // Bottom 8 pads: Play chords (1-5 notes depending on voicing)
    for (int i = 0; i < 5; i++) {
      int8_t interval = CHORD_VOICINGS[currentChordMode][pad][i];
      
      // Skip if this is a NO_NOTE marker
      if (interval == NO_NOTE) continue;
      
      int note = rootNote + interval + transpose + (octave * 12);
      note = constrain(note, 0, 127);
      if (noteOn) MIDI.sendNoteOn(note, velocity, midiChannel);
      else MIDI.sendNoteOff(note, 0, midiChannel);
    }
  } else {
    // Top 8 pads: Play single notes from the chord mode's scale
    byte noteIndex = pad - 8;
    byte chordScale = CHORD_MODE_SCALES[currentChordMode];
    int note = rootNote + 12 + SCALES[chordScale][noteIndex] + transpose + (octave * 12);
    note = constrain(note, 0, 127);
    if (noteOn) MIDI.sendNoteOn(note, velocity, midiChannel);
    else MIDI.sendNoteOff(note, 0, midiChannel);
  }
}

void playOmniChord(byte pad, bool noteOn) {
  if (pad < 8) {
    // Bottom 8 pads: Select chord (like fretting)
    if (noteOn) {
      omniChordIndex = pad;
    }
  } else {
    // Top 8 pads: Pluck individual notes from the selected chord
    byte stringIndex = pad - 8;  // Which "string" (note in chord) to pluck
    
    if (noteOn) {
      // All 8 top pads can now play notes from the chord
      int8_t interval = OMNI_CHORD_VOICINGS[omniChordIndex][stringIndex];  // Use 8-note omni chord voicings
      
      int note = rootNote + interval + transpose + (octave * 12);
      note = constrain(note, 0, 127);
      
      // Store the actual MIDI note for this pad
      omniChordNotes[stringIndex] = note;
      MIDI.sendNoteOn(note, velocity, midiChannel);
    } else {
      // Note off: use the stored note for this pad
      if (omniChordNotes[stringIndex] != 255) {
        MIDI.sendNoteOff(omniChordNotes[stringIndex], 0, midiChannel);
        omniChordNotes[stringIndex] = 255;  // Clear the stored note
      }
    }
  }
}

void updateGenerative(unsigned long now) {
  if (now - lastGenNoteTime >= nextGenNoteDelay) {
    
    if (currentGenNote != 255) {
      MIDI.sendNoteOff(currentGenNote, 0, midiChannel);
      currentGenNote = 255;
      delay(10);
    }
    
    if (random(0, 100) < GEN_TRIGGER_CHANCE) {
      byte octaveOffset = random(0, 3) * 12;
      byte noteIndex = random(0, 8);
      
      // Use scale 0 (Major) for generative mode instead of user's scale
      int note = rootNote + octaveOffset + SCALES[0][noteIndex] + transpose + (octave * 12);
      currentGenNote = constrain(note, 0, 127);
      
      MIDI.sendNoteOn(currentGenNote, random(60, 90), midiChannel);
    }
    
    nextGenNoteDelay = random(MIN_GEN_DELAY, MAX_GEN_DELAY);
    lastGenNoteTime = now;
  }
}

void toggleMenu() {
  inMenu = !inMenu;
  digitalWrite(LED_PIN, inMenu ? HIGH : LOW);
  
  if (inMenu) {
    // Only send Note Off for the current MIDI channel to avoid lag
    for (byte n = 0; n < 128; n++) {
      MIDI.sendNoteOff(n, 0, midiChannel);
    }
    currentPage = 1;
  }
}

void loadPreset(byte index) {
  playStyle = presets[index].style;
  currentScale = presets[index].scale;
  currentChordMode = presets[index].chordMode;
  transpose = presets[index].transpose;
  octave = presets[index].octave;
  velocity = presets[index].velocity;
}

void savePreset(byte index) {
  presets[index] = {playStyle, currentScale, currentChordMode, transpose, octave, velocity};
  savePresetsToFlash();  // Write to flash
}

void resetToDefaults() {
  playStyle = STYLE_SCALED;
  currentScale = 0;
  currentChordMode = 0;
  transpose = 0;
  octave = -1;
  velocity = 100;
  midiChannel = 1;
  
  // Reset all presets to defaults
  presets[0] = {STYLE_SCALED, 0, 0, 0, -1, 100};
  presets[1] = {STYLE_CHROMATIC, 7, 0, 0, -1, 100};
  presets[2] = {STYLE_CHORD, 0, 0, 0, -1, 100};
  presets[3] = {STYLE_GENERATIVE, 0, 0, 0, -1, 80};
  
  savePresetsToFlash();  // Write to flash
}