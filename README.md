# Frame Rate Independent Input Example
This is a basic example that shows how to use the SUPPORT_CUSTOM_FRAME_CONTROL build flag to decouple rendering from input, while still using a single thread.

# How it works
By setting SUPPORT_CUSTOM_FRAME_CONTROL=1 in the build options, raylib will no longer automaticly update the frame buffer and pollinput during calls to EndDrawing.
This allows a game to determine when and where it actually draws a frame.

This example runs the input loop as fast as posible, and tracks the keys and mouse buttons that are pressed and released in an input queue.
It uses an accumulator to compute a fixed step tick timestep, and processes game logic and renders only on that frame rate, independent of the actual update frame rate.


# Building
This repository uses the build system from game premake.
https://github.com/raylib-extras/game-premake/

# Generate Projects
For windows users, there are two batch files you can use depending on what compiler you are using. For linux users you can simply use a terminal.
Only do ONE of these options depending on your compiler and platform.
## Visual Studio (MSVC) Users
Visual Studio users should run

    premake-VisualStudio.bat
	
This will generate a Visual Studio project.

## Visual Studio Code (VSC) Users
Visual Studio code uses the makefile systems for MinGW-w64, gcc, or clang on Windows, linux, and OSX respectivly. Simply follow the steps below for your OS and then continue to the VSC section below. Please make sure that you have the C/C++ extensions installed for VSC. Please make sure your compiler is in your path in order to use Visual Studio Code.
	
## MinGW-w64 Users
Please make sure you have a recent version of MinGW-W64. The older versons from mingw.org will not work.
We recommend the W64Devkit. It has everything needed to build raylib. It can be downloaded from here https://github.com/skeeto/w64devkit/releases

Once you have MinGW-W64
Run the batch file.

    premake-mingw.bat

This will generate a makefile for you.

## Linux users
cd to the root folder and run

    ./premake5 gmake

This will generate a makefile for you.

## macOS users
cd to the root folder and run

    ./premake5.osx gmake
	
This will generate a makefile for you.

# Build the example
Only do ONE of these options depending on your compiler and platform.

## Visual Studio (MSVC) Users
Double click the .sln file that was generated in the folder. From here you can use the project as normal.

#Visual Studio Code (VSC) Users
Game-premake comes with a visual studio code project file, simply open the folder in vscode and it should be setup to run the makefiles build for gcc or MinGW-w64. Just use the build, debug, or run functions in VSC

## MinGW-w64 Users
Open your compiler terminal (w64devkit if you are using it), change to the game folder and type.

    make
	
This will build your game
	
## Linux/macOS users
Open your terminal, change to the root folder and type.

    make
	
This will build the example
	

# License
This example by Jeffery Myers is marked CC0 1.0. To view a copy of this mark, visit https://creativecommons.org/publicdomain/zero/1.0/
