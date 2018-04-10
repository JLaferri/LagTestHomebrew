## Download
If all you're interested in is a compiled version of the app, browse to the compiled folder and download the files in there.

## Making Changes
1. First download a full copy of the source folder. The easiest way to do this is probably to download the entire repo. Either clone it or download from https://github.com/JLaferri/LagTestHomebrew/archive/master.zip
2. You're going to need devkitPPC and Programmer's Notepad, follow the instructions at http://wiibrew.org/wiki/Devkitppc_setup_(Windows)
3. There are three dependencies (libwiisprite, libpng, and zlib) to build this project, you'll need to "install" them
	1. Browse to the folder of the dependency
	2. Copy the contents of the include folder to C:\devkitPro\libogc\include
	3. Copy the .a file in the lib folder into C:\devkitPro\libogc\lib\wii
4. Open template.pnproj in Programmer's Notepad
5. You can then open lagtestapp.cpp and make your desired changes
6. To build, simply click on Tools -> make
7. You should now have a .elf file in the build directory. You may need to google how to convert it to a .dol file? Not sure if you can use a .elf file directly in homebrew