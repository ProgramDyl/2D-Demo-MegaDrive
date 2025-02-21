# Mega Drive Game Demo
Nov/Dec 2024

## Introduction

Hello future! 

If you are reading this and you are not the author, that means this demo has 
piqued your interest. For that, I graciously thank you. As I write this intro,
I am in my 2nd year of my college programming diploma in Nova Scotia, Canada 
with an aim of moving to Montreal after graduation. Though my primary interest
is full stack web development, I wanted to ensure that I had some experience in
lower-level languages like C.  

This project is a demo game developed for the Sega Mega Drive/Genesis 
using C programming language, SGDK, and emulators BlastEm and Gens. The initial
version of the game is using premade assets but will be replaced by custom art
and sprites... eventually. 

It's a dream come true to make a game.


If you are reading this, thanks for checking it out! 

**_Dylan Cunningham_**


## Installation

### Prerequisites:
- [SGDK (Sega Game Development Kit)](https://github.com/Stephane-D/SGDK)

- [Visual Studio Code](https://code.visualstudio.com/) 

- Sega Genesis extension in VSCode

- Emulators: [Gens](http://www.gens.me/downloads.shtml) and [BlastEm](https://www.retrodev.com/blastem/) 

## Setup

1. **Clone the repository:**
```sh
[git clone](https://github.com/ProgramDyl/2D-Demo-MegaDrive.git) 
```

2. **Install SGDK:** 
    - Follow the instructions on the SGDK GitHub page to install SGDK and set up your environment variables. 

3. **Open the project with Visual Studio Code**
```sh
code .
```

4. **Build the project:** 
```sh
make -f path/to/SGDK/makefile.gen
```

5. **Run the game using an emulator:**
```sh
start /B "C:\Program Files\gens\gens.exe" path/to/your/rom.bin
```

## Usage

- **Movement:** The D-Pad on your controller (When testing, I used an Xbox One controller, GENS recognized it without any trouble).

- **Punch:** Press the B button to make the character punch.

## Current Features: 

- Character movement in all directions

- Basic animations for walking, punching, and idle. 

- Background and foreground layers.

## To Contribute: 

1. Fork the repository

2. Create a new branch `git checkout -b feature-branch`

3. Make your changes

4. Commit your changes `git commit -m 'Add new feature'`

5. Push to the branch `git push origin feature-branch`

6. Open a pull request


## License
Copyright (c) <2024>  Jamware Media Design 

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 






## Acknowledgements:

### Thank You:

- Stephane Dallongeville for writing the SGDK for so 
many to use and enjoy. 

- Nova Scotia Community College for being such an
encouraging force in the outset of my dev career. 

- Bill Kowalski for taking a chance and giving me a
work placement. Cheers! 



