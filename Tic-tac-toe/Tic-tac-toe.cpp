#include <Windows.h>
#include <stdio.h>
#include "Tic-tac-toe.h"

HANDLE hStdOutHandle, hStdInHandle;
CHAR_INFO consoleBuffer[GameFieldSize_X * GameFieldSize_Y];

int GetClickedField( SHORT X, SHORT Y)
{
	for (BYTE i = 0; i < 9; i++)
	{
		if ( g_FieldEntrys[i].dwStartColumn <= Y && g_FieldEntrys[i].dwEndColumn >= Y  )
		{
			if ( g_FieldEntrys[i].dwStartRow <= X && g_FieldEntrys[i].dwEndRow >= X )
				return i;
		}
	}
	return -1;
}

int GetWinnerTeamId()
{
	for (int i = 0; i < 8; i++)
	{	
		if ( GameFieldTeams[WinningCombinations[i][0]] != 0 &&
			 GameFieldTeams[WinningCombinations[i][0]] == GameFieldTeams[WinningCombinations[i][1]] && 
			                                              GameFieldTeams[WinningCombinations[i][1]] == GameFieldTeams[WinningCombinations[i][2]] )
		return GameFieldTeams[WinningCombinations[i][0]]-1;
	}
	return -1;
}

bool AreAllFieldsFilled()
{
	for (int i = 0; i < 9; i++)
	{
		if ( GameFieldTeams[i] == 0 )
			return false;
	}
	return true;
}

/// <summary>Sets the team's symbol to the field by id</summary>
void SetSymbol(CHAR_INFO* consoleBuffer, BYTE TeamId, int iFieldNumber )
{
	FieldPart* pField = &g_FieldEntrys[iFieldNumber];
	if ( pField == NULL )
		return;

	char* pSelectedSmybole = NULL;
	WORD SelectedColor = NULL;

	if ( TeamId < TeamID_TeamOne || TeamId > TeamID_TeamTwo )
		return;

	pSelectedSmybole = g_Teams[TeamId].SymbolChars;
	SelectedColor = g_Teams[TeamId].SymbolColor;

	int iSymbolLocation = 0;
	for (int iY = 0; iY < 3; iY++)
	{
		for (int iX = 0; iX < 3; iX++)
		{
			DWORD dwBufferOffset = (pField->dwStartRow+iX) + (pField->dwStartColumn+iY) * GameFieldSize_Y;
			consoleBuffer[ dwBufferOffset ].Char.AsciiChar = pSelectedSmybole[iSymbolLocation];
			consoleBuffer[ dwBufferOffset ].Attributes = SelectedColor;
			iSymbolLocation++;
		}
	}
}

COORD GameFieldSize = {GameFieldSize_Y,GameFieldSize_Y};
COORD characterPos = {0,0};
SMALL_RECT writeArea = {0,0,GameFieldSize.X-1,GameFieldSize.Y-1}; 

/// <summary>resets the game field</summary>
void ResetGameField(CHAR_INFO* consoleBuffer)
{
	CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = {0};
	DWORD written;
	GetConsoleScreenBufferInfo(hStdOutHandle, &ScreenBufferInfo);
	FillConsoleOutputCharacterA( hStdOutHandle, ' ', (ScreenBufferInfo.dwSize.X * ScreenBufferInfo.dwSize.Y), characterPos, &written );

	for (int i = 0; i < GameFieldSize.X * GameFieldSize.Y; i++) 
	{
		consoleBuffer[i].Char.AsciiChar = GameField[i];
		consoleBuffer[i].Attributes =  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	}
	WriteConsoleOutputA(hStdOutHandle, consoleBuffer, GameFieldSize, characterPos, &writeArea);

	ZeroMemory( GameFieldTeams, 9 );

	COORD CursorPosition = {0,GameFieldSize.Y+1};
	SetConsoleCursorPosition( hStdOutHandle, CursorPosition );

	//Read the console events now so that events after the game finished don't get processed
	DWORD dwNumberOfEvents;
	GetNumberOfConsoleInputEvents(hStdInHandle, &dwNumberOfEvents);

	INPUT_RECORD* InputEventBuffer = (INPUT_RECORD*)malloc( dwNumberOfEvents * sizeof(INPUT_RECORD) );
	DWORD dwEventsRead = 0;
	ReadConsoleInputA(hStdInHandle, InputEventBuffer, dwNumberOfEvents, &dwEventsRead);
	free( InputEventBuffer );
}

/// <summary>has to be called on a left click event</summary>
void HandleClickAt(CHAR_INFO* consoleBuffer, SHORT ClickX, SHORT ClickY )
{
	static BYTE dwCurrentTeamId = TeamID_TeamOne;

	int dwFieldClickedNumber = GetClickedField( ClickX, ClickY );
	if ( dwFieldClickedNumber == -1 || GameFieldTeams[dwFieldClickedNumber] != 0 )
		return;
	
	SetSymbol( consoleBuffer, dwCurrentTeamId, dwFieldClickedNumber );
	WriteConsoleOutputA(hStdOutHandle, consoleBuffer, GameFieldSize, characterPos, &writeArea);

	GameFieldTeams[dwFieldClickedNumber] = dwCurrentTeamId+1;
	
	int WinnerTeamId = GetWinnerTeamId();
	bool bAreAllFieldsFilled = AreAllFieldsFilled();
	
	if ( WinnerTeamId != -1 || bAreAllFieldsFilled == true )
	{
		if ( WinnerTeamId != -1 )
			printf("Team %s WON!\n",g_Teams[WinnerTeamId].SymbolTeamName);
		else
			printf("No one won, its a draw!\n",g_Teams[WinnerTeamId].SymbolTeamName);

		/*
		//-------- Play some sounds --------
		Beep( 500, 100 );
		Beep( 600, 100 );
		Beep( 700, 100 );
		Beep( 800, 200 );
		*/

		CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo = {0};
		GetConsoleScreenBufferInfo( hStdOutHandle, &ConsoleScreenBufferInfo );
		for (int h = 5; h > 0; h--)
		{
			printf("New game in %i seconds",h);
			Sleep( 1000 );
			
			SetConsoleCursorPosition( hStdOutHandle, ConsoleScreenBufferInfo.dwCursorPosition );
		}
		ResetGameField(consoleBuffer);
	}
	dwCurrentTeamId++;
	if ( dwCurrentTeamId >= TeamID_Max )
		 dwCurrentTeamId = 0;
}
