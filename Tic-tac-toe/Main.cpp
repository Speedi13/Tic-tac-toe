#include <Windows.h>
#include <stdio.h>
#include "Tic-tac-toe.h"

int main( )
{
	hStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdInHandle = GetStdHandle(STD_INPUT_HANDLE);

/*	//--------- To change the console windows size ---------
	SMALL_RECT CmdWindowSize = {0, 0, 80-1, 22-1};
	SetConsoleWindowInfo(hStdOutHandle, TRUE, &CmdWindowSize);

	COORD CmdBufferSize = {80, 22};
    SetConsoleScreenBufferSize(hStdOutHandle, CmdBufferSize);
*/
	ResetGameField( consoleBuffer );

	while (true)
	{
		DWORD dwNumberOfEvents = 0;
		GetNumberOfConsoleInputEvents(hStdInHandle, &dwNumberOfEvents);
		if ( dwNumberOfEvents == NULL )
			continue;

		INPUT_RECORD* InputEventBuffer = (INPUT_RECORD*)malloc( dwNumberOfEvents * sizeof(INPUT_RECORD) );
		DWORD dwEventsRead = 0;
		ReadConsoleInputA(hStdInHandle, InputEventBuffer, dwNumberOfEvents, &dwEventsRead);

		for ( DWORD i = 0; i < dwEventsRead; i++ ) 
		{
			if (InputEventBuffer[i].EventType != MOUSE_EVENT) 
				continue;

			if ( !(InputEventBuffer[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) )
				continue;

			SHORT ClickX = InputEventBuffer[i].Event.MouseEvent.dwMousePosition.X;
			SHORT ClickY = InputEventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
			HandleClickAt( consoleBuffer, ClickX, ClickY );
		}
		free( InputEventBuffer );
	}

	return 0;
}