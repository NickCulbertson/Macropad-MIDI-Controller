# Macropad MIDI Controller

A versatile 16-pad MIDI controller in a 3D-printed enclosure powered by the Raspberry Pi Pico. Features multiple play modes, scales, chord voicings, and an strum mode for guitar-like playing.

*“DIY 3D-Printed MIDI Macropad | Simple Design, Hidden Features w/ Raspberry Pi Pico”*
[![Watch the video](https://img.youtube.com/vi/rUteUrWNM18/hqdefault.jpg)](https://youtu.be/rUteUrWNM18)

## Features

### 🎵 Multiple Play Modes
- **Scale Mode**: Play in various musical scales across 16 pads
- **Chromatic Mode**: Full chromatic note access
- **Chord Mode**: Rich chord voicings with scale melodies
- **Generative Mode**: AI-like random note generation
- **Strum Mode**: Guitar-like chord selection and individual note picking

### 🎼 Musical Scales (8 Available)
Organized with major/minor pairs for intuitive selection:

**Major Scales (0 & 4):**
1. **Major** - Happy, classic sound
5. **Pentatonic Major** - Can't play wrong notes

**Minor Scales (1 & 5):**
2. **Minor** - Sad, emotional feel
6. **Pentatonic Minor** - Easy improvisation

**Exotic Scales:**
3. **Blues** - Soulful blues scale
4. **Harmonic Minor** - Dramatic, exotic
7. **Lydian** - Bright, dreamy, raised 4th
8. **Chromatic** - All 16 semitones in sequence

*Note: All scales are linear across 16 pads with no repeating notes for smooth melodic progressions.*

### 🎹 Chord Modes (8 Available)
1. **Major** - Rich diatonic voicings with bass roots
2. **Minor** - Natural minor with bass roots
3. **Blues** - Dominant 7th chords with blue notes
4. **Harmonic** - Dramatic harmonic minor chords
5. **Open** - Cinematic, wide voicings
6. **Minor** (diatonic) - Alternative minor voicings
7. **Jazz 7ths** - Sophisticated rootless jazz voicings
8. **Diatonic** - Standard close voicings with tight groupings

### ⚙️ Advanced Features
- **4 Preset Slots** - Save/load complete configurations
- **Flash Memory Storage** - Settings persist between power cycles
- **Adjustable Parameters**:
  - Transpose (-12 to +12 semitones)
  - Octave shift (-2 to +2 octaves)
  - Velocity (20-127)
  - MIDI Channel (1-16)
- **Menu System** - 4-page interface for all settings

## Hardware Requirements

### Electronics
- 1x Raspberry Pi Pico
- 16x Tactile switches (6x6mm recommended) - I used the Brown version of these, but any switch will do (https://www.aliexpress.us/item/3256809857749295.html?channel=twinner)
- 16x Key caps - I used ones like these (https://www.aliexpress.us/item/3256804338916520.html?channel=twinner)
- 1x LED (optional, for menu indication)
- Hookup wire
- USB-C cable

### 3D Printing
- **Enclosure**: STL files for top and bottom case
- **Print Settings**:
  - Layer Height: 0.2mm
  - Infill: 20%
  - Supports: Yes (for overhangs)
  - Material: PLA or PETG recommended

## Installation

### 1. Hardware Assembly
1. **3D Print** the enclosure parts
2. **Wire the switches** to GPIO pins 0-15 on the Pico
3. **Connect LED** to GPIO pin 25 (optional)
4. **Assemble** the enclosure with switches and Pico

### 2. Software Installation

#### Arduino IDE Setup
1. **Install Arduino IDE** (2.0+ recommended)
2. **Add Pico Board Support**:
   - Go to `File → Preferences`
   - Add to Additional Board Manager URLs:
     ```
     https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
     ```
   - Go to `Tools → Board Manager`
   - Search for "pico" and install "Raspberry Pi Pico/RP2040"

#### Library Installation
Install these libraries via `Tools → Manage Libraries`:
- **Adafruit TinyUSB Library**
- **MIDI Library** by Francois Best

#### Upload Firmware
1. **Open** `Macropad-MIDI-Controller.ino`
2. **Select Board**: `Tools → Board → Raspberry Pi Pico`
3. **Select Port**: Your Pico's USB port
4. **Upload** the code

## Usage Guide

### Basic Operation

#### Menu Access
- **Press all 4 top-right pads simultaneously** (pads 12-15) to enter/exit menu
- **LED turns on** when in menu mode

#### Pad Layout
```
[12] [13] [14] [15]
[ 8] [ 9] [10] [11]
[ 4] [ 5] [ 6] [ 7]
[ 0] [ 1] [ 2] [ 3]
```

### Menu System

#### Page 1: Presets & Settings
- **Pads 0-3**: Load presets 1-4
- **Pads 4-5**: Decrease/Increase velocity
- **Pad 6**: Cycle MIDI channel
- **Pad 7**: Reset to defaults
- **Pads 8-9**: Decrease/Increase octave
- **Pads 10-11**: Decrease/Increase transpose
- **Pads 12-15**: Navigate to pages 1-4
- **Hold pad 0-3 for 2 seconds**: Save current settings to that preset

#### Page 2: Scale Selection
- **Pads 0-7**: Select scale (Major, Minor, Blues, Harmonic, Pent Maj, Pent Min, Lydian, Chromatic)
- **Pads 12-15**: Navigate to pages 1-4

#### Page 3: Chord Mode Selection  
- **Pads 0-7**: Select chord mode (Major, Minor, Blues, Harmonic, Open, Minor, Jazz 7ths, Diatonic)
- **Pads 12-15**: Navigate to pages 1-4

#### Page 4: Example Modes
- **Pad 0**: Activate Generative Mode (random note generation)
- **Pad 1**: Activate Omni Chord Mode (guitar-like playing)
- **Pads 12-15**: Navigate to pages 1-4

### Play Modes Explained

#### Scale Mode
- **All 16 pads** play notes in the selected scale linearly
- **No repeating notes** - smooth melodic progression from pad 0 to 15
- **Each scale spans 2+ octaves** with scale-specific intervals
- **Perfect for melodies and arpeggios** with natural note flow

#### Chromatic Mode
- **All 16 pads** play consecutive chromatic notes
- **Starting from** the root note (default C4)

#### Chord Mode
- **Bottom 8 pads**: Play rich chord voicings (up to 5 notes each)
- **Top 8 pads**: Play single melody notes from the corresponding scale

#### Generative Mode
- **Automatic random note generation** in major scale
- **No pad interaction** - just listen!
- **Adjustable timing** and probability

#### Strum Mode (Guitar-like)
- **Bottom 8 pads**: Select chord (like fretting)
- **Top 8 pads**: Pick individual notes from selected chord (like strings)
- **Select bottom pad + pick top pads** for guitar-like arpeggios

## Default Presets

1. **Preset 1**: Scale Mode, Major scale
2. **Preset 2**: Chromatic Mode  
3. **Preset 3**: Chord Mode, Major chords
4. **Preset 4**: Generative Mode

## Troubleshooting

### Upload Issues
- **Ensure Pico is in bootloader mode**: Hold BOOTSEL while connecting USB
- **Check board selection**: Must be "Raspberry Pi Pico"
- **Verify libraries**: All required libraries installed

### MIDI Not Working
- **Check USB connection**: Device should appear as "Macro MIDI Pad"
- **MIDI software**: Ensure your DAW recognizes USB MIDI devices
- **Channel conflicts**: Try different MIDI channels (pad 6 in menu)

### No Sound
- **Check velocity**: Increase with pad 5 in menu
- **Verify MIDI routing**: Ensure your DAW is receiving on the correct channel
- **Test with external MIDI monitor**

## Technical Specifications

- **MCU**: Raspberry Pi Pico (RP2040)
- **I/O**: 16 GPIO pins for switches, 1 for LED
- **MIDI**: USB MIDI Class Compliant
- **Memory**: Flash storage for presets
- **Power**: USB bus powered
- **Latency**: <1ms response time

## Contributing

Contributions welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

This project is open source. See LICENSE file for details.

---

**Enjoy making music with your Macropad MIDI Controller!** 🎵
