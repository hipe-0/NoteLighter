# NoteLighter
Piano learning device that you can use with any piano or keyboard to play a piece, even without knowing how to play or read notes.
The design is by me and is released with an open source license (read below)
The colored leds shows you the notes to play along on the keyboard.

A fun device to discover the piano, and learn piece of music.

![20240424_080407](https://github.com/user-attachments/assets/3fd846a6-ea32-4e1e-ab59-feb5836b7a59)


https://github.com/user-attachments/assets/676a4949-13e3-48ed-bcb9-873c211a0ec9



https://github.com/user-attachments/assets/14993cb8-82d8-4a3e-a8aa-3aab47217e36

![20240310_192418](https://github.com/user-attachments/assets/16d1434c-4288-4541-8853-775883b43c2a)

![20240310_163554](https://github.com/user-attachments/assets/d536087f-5813-4166-8ad3-a31fc206ea18)

# How to use
- Fill the device with midi files using the provided SD card reader.
- Position the led strip on your keyboard, align the blue reference leds on the black keys
- Extend or retract your blue reference leds using the "Width" knob
- Select your speed on the "tempo" knob. (100% is right in the middle of the cursor)
- Select your music on the touch screen
- the leds will initially  light in red = Red means get your finger ready to strike a note
- the led turns from red to white = White means you need to press the key now
- the leds turns off = you need to release the key now (accounts for the duration)
- playing all notes executes the music
- at the end of the song he song pauses and the lights starts the song again.
- you can select any other song at any moment
  
# Code
Please find the code in this repository

# Shematic
tbd

# Bill of materials
- Teensy 4.1 microcontroler (has SD card reader)
- WS2812B led strip of density 144 leds/m, cut after 110 leds, 5 volts
- 3D printed parts (you can print on your own, I will post my 3D models for free later)
- USB Cable ( USB A male to micro USB male )
- 2x Potentiometers 10K
- ILI9341 240x320 2.4 TFT SPI LCD screen with resistive touch controller XPT2046
- 1x SD card, any size 
- Some midi files to play
- ...

# 3D print
tbd

# License

CC BY-NC, 

see https://creativecommons.org/licenses/by-nc/4.0/ 
This license enables reusers to distribute, remix, adapt, and build upon the material in any medium or format for noncommercial purposes only, and only so long as attribution is given to the creator. CC BY-NC includes the following elements:

BY: credit must be given to the creator. NC: Only noncommercial uses of the work are permitted.
