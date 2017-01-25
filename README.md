Right now this readme will contain the very minimal information required to duplicate my lag tester. If anyone wants to make a more detailed guide I'm open to placing it here, or linking to it.

Here is a demonstation video of the device in use: https://www.youtube.com/watch?v=UTNUIQtreXQ

## Theory and Limitations
Currently this method only works using a Wii with homebrew channel installed. In order for the test to work you need to have a display that emits white light synced up with a sound cue. Both are picked up by the board and the offset is used to calculate the lag. The theory behind it is that the audio will never be delayed whereas the video will be. A CRT is used for calibration.

The Wii limitation could be eliminated if someone designed something else that created these same signals on generic displays. The nice thing about using the Wii is that you know you are providing the same output to the monitor that you would playing a smash game on Wii.

## Board
Click the following link to order some boards or download the gerbers:

<a href="https://oshpark.com/shared_projects/O6JRaha8"><img src="https://oshpark.com/assets/badge-5b7ec47045b78aef6eb9d83b3bac6b1920de805e9a0c227658eac6e19a045b9c.png" alt="Order from OSH Park"></img></a>

Another fab that I recommend if you want to get a few more made is https://www.seeedstudio.com/fusion_pcb.html

## Components
http://www.digikey.com/short/3rd243

The above was an order I made to make 3 lag testers... You don't need the photocell, that was experimental and I didn't end up really using it. There's potentially cheaper places to get the light sensor and this cart is pretty old so likely you'll have to find compatible components. If you want to provide an updated cart I can add it to this Wiki.

### Soldering Guide
This isn't really a guide so much as an aid to tell you where to place what components...
https://github.com/JLaferri/LagTestHomebrew/blob/master/SolderGuide.pdf

### Flashing the Processor
Once you have soldered the board together you will need to flash the processor with the code required to do the testing and run the display. The processor design is based on an Arduino and this can be done using the Arduino IDE. Open the IDE, select the correct programmer, and flash using the programmer. You will need a programmer such as https://www.sparkfun.com/products/9825

The Arduino file can be found here:
https://github.com/JLaferri/LagTestHomebrew/blob/master/Arduino%20Code/lagtester_current.ino

## Homebrew App
The following folder contains the app files to be used on homebrew channel on a hacked Wii
https://github.com/JLaferri/LagTestHomebrew/tree/master/Homebrew%20App%20Compiled/LagTestHomebrew

## Additional Requirements
Depending on what you intend to test you might need audio converters/extenders. Here are a few that I've used:
* https://www.amazon.com/gp/product/B00004Z5CP
* https://www.amazon.com/Female-Audio-Extension-Cable-CNE63133/dp/B00B973X20

You'll also need to purchase a 9V battery.

## Instructions
This sections explains how to actually use the tester

### Calibration
To add... I think it works kinda like the process in the next section but you gotta hold the button down first? I need to remember how to do this lol. Without calibrating, however, results will be meaningless. A "safe" CRT should be used to calibrate.

### Executing a Test
1. Ensure that the room you are testing in is not flooded with sunlight. The light sensor works by detecting lux and the sun will saturate the voltage output because it produces a lot of it. Too much artificial lighting is also not good but you should be fine as long as there's not a really bright light right behind the monitor you're testing or something.
2. Place a 9V battery in holder. The polarity is marked on the plastic. Do not reverse the polarity as I have not tested what would happen if you do. Worst case scenario, you'd fry the board. More likely though is that nothing would happen.
3. Plug audio cable into the device. Ideally this audio source comes directly from the Wii. In the case of the Sewell adapter use the cable I provided at its output and plug into board. If using component cables you might need to get an RCA connection extender for the audio to reach the device while maintaining mobility. You can also use the audio output of the monitor if it has one but you run the risk that if the monitor adds latency to the audio signal the measurement will be wrong.
4. Turn on the device by flipping the on off switch to on.
5. Boot into the homebrew app.
6. A white bar should be flashing on and off on the monitor, this is the test area. Place the device such that the clear light sensor on the bottom of the board is over the test area. For a CRT, keep a fair distance from the screen (probably at least 5 inches), you don't need to be overly precise about being directly in front of the test area on a CRT as it shoots out a fair amount of light. For an LCD I would recommend placing the sensor about half an inch to one inch away from the screen.
7. Holding the tester in place in front of the test area, press the push button on the top of the board. Wait until either an error code, indicated by the value Err#, or a result in milliseconds appears on the display.
8. Optionally take a few measurements to ensure repeatability.
9. Press A on the Wii Mote to move to the next test area.
10. Repeat steps 6 to 9 for the two other test areas (middle, bottom). Taking an equal amount of measurements for each.
11. Average all results to get the average frame lag of the display. On a CRT this should be close to 8.33 ms.

### Error codes
* Err0 - Audio is not begin detected. Either the amplitude of the audio signal has been reduced (via turning down the volume of a monitor output for example), the homebrew application is not running, or the cable is not plugged in.
* Err1 - Audio cable was disconnected during test?
* Err2 - Audio signal is stuck high for some reason. Not sure what would cause this.
* Err3 - Video signal difference between black and white is not large enough. Is the sensor in front of the test area?
