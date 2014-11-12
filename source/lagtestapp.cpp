#include <wiiuse/wpad.h>
#include <ogc/pad.h>
#include <wiisprite.h>

#include "asndlib.h"

using namespace wsp;
 
#define audioSignalLength 132

char audioSignal[audioSignalLength] = 
{
	64,0,64,0,63,255,64,2,63,253,64,3,63,254,64,1,63,255,64,1,63,255,64,1,64,0,63,255,64,2,63,254,64,1,64,0,63,255,64,2,63,254,
	64,1,64,0,63,255,64,2,63,254,64,1,63,255,64,2,63,254,64,2,63,255,63,255,64,2,191,254,192,1,192,1,191,254,192,1,192,1,191,253,
	192,5,191,250,192,5,191,254,192,0,192,1,191,254,192,2,191,254,192,3,191,253,192,1,192,1,191,253,192,4,191,253,192,2,191,255,
	192,0,192,1,191,254,192,3,191,253,192,3,191,253
};

Quad topTestBox, middleTestBox, bottomTestBox;
 
int main(int argc, char **argv)
{
	// Create the game window and initalise the VIDEO subsystem
	GameWindow gwd;
	gwd.InitVideo();
 
	gwd.SetBackground((GXColor){ 0x00, 0x00, 0x00, 0xFF });
 
	LayerManager manager(3);
 
	// Initialize test boxes
	topTestBox.SetPosition(32, 12);
	topTestBox.SetWidth(576);
	topTestBox.SetHeight(12);
	topTestBox.SetFillColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
	
	middleTestBox.SetPosition(32, 234);
	middleTestBox.SetWidth(576);
	middleTestBox.SetHeight(12);
	middleTestBox.SetFillColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
	
	bottomTestBox.SetPosition(32, 456);
	bottomTestBox.SetWidth(576);
	bottomTestBox.SetHeight(12);
	bottomTestBox.SetFillColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
	
	// Initialise Wiimote
	WPAD_Init();
	PAD_Init();
 
	int frameCounter = 0;
	int displayMode = 0;
	
	for(;;)
	{
		if (frameCounter >= 60) frameCounter -= 60;
		
		if (frameCounter == 28)
		{
			//Play sound
			//Library is re-initialized every time to prevent desync issue that was noticed otherwise
			ASND_Init();
			ASND_Pause(0);
			ASND_SetVoice(0, VOICE_MONO_16BIT_BE, 8000, 0, audioSignal, audioSignalLength, 255, 255, NULL);
		}
		else if (frameCounter == 31)
		{
			//Add test boxes to manager to that they will be displayer
			if (displayMode == 3 || displayMode == 0) manager.Append(&topTestBox);
			if (displayMode == 3 || displayMode == 1) manager.Append(&middleTestBox);
			if (displayMode == 3 || displayMode == 2) manager.Append(&bottomTestBox);
		}
		else if (frameCounter == 36)
		{
			//Clear manager so that test boxes disappear
			manager.RemoveAll();
			
			//Stop sound library
			ASND_End();
		}
		
		//Exit if home pressed
		WPAD_ScanPads();
		PAD_ScanPads();
		
		u16 buttonsDown = WPAD_ButtonsDown(WPAD_CHAN_0);
		u16 gcButtonsDown = PAD_ButtonsDown(PAD_CHAN0);
		if (buttonsDown & WPAD_BUTTON_HOME || gcButtonsDown & PAD_BUTTON_START) break;
		else if (buttonsDown & WPAD_BUTTON_A || gcButtonsDown & PAD_BUTTON_A) displayMode = (displayMode + 1) % 4;
		
		//Count the frames
		frameCounter++;
		
		manager.Draw(0, 0); 
		gwd.Flush();
	}
	
	manager.Draw(0, 0); 
	gwd.Flush();
		
	return 0;
}