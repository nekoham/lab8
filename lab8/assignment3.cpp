#include <stdio.h>
#include <windows.h>
#include <time.h>
#define scount 80
#define screen_x 80
#define screen_y 25
HANDLE wHnd;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
COORD star[scount];
COORD ship;
HANDLE rHnd;
DWORD fdwMode;
int ship_color = 7;
int hp = 10;


int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}

int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

void clear_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 7;
		}
	}
}
void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos,
		&windowSize);
}
void init_star()
{
	for (int i = 0; i < scount; i++)
	{
		star[i].X = rand() % screen_x;
		star[i].Y = rand() % screen_y;
	}
}

void star_fall()
{
	int i;
	for (i = 0; i < scount; i++) {
		if (star[i].Y >= screen_y - 1) {
			star[i] = { (SHORT(rand() % screen_x)),SHORT(1) };
		}
		else {
			star[i] = { star[i].X, SHORT(star[i].Y + 1) };
		}
	}
}

void fill_star_to_buffer()
{
	for (int i = 0; i < scount; i++)
	{
		consoleBuffer[star[i].X + screen_x * star[i].Y].Char.AsciiChar = '*';
	}
}

void fill_ship_to_buffer(int x, int y, int color)
{
	consoleBuffer[x + screen_x * y].Char.AsciiChar = '<';
	consoleBuffer[x + 1 + screen_x * y].Char.AsciiChar = '-';
	consoleBuffer[x + 2 + screen_x * y].Char.AsciiChar = '0';
	consoleBuffer[x + 3 + screen_x * y].Char.AsciiChar = '-';
	consoleBuffer[x + 4 + screen_x * y].Char.AsciiChar = '>';

	consoleBuffer[x + screen_x * y].Attributes = color;
	consoleBuffer[x + 1 + screen_x * y].Attributes = color; 
	consoleBuffer[x + 2 + screen_x * y].Attributes = color;
	consoleBuffer[x + 3 + screen_x * y].Attributes = color;
	consoleBuffer[x + 4 + screen_x * y].Attributes = color;
}

int main()
{
	srand(time(NULL));
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setConsole(screen_x, screen_y);
	setMode();
	init_star();
	while (play && hp > 0)
	{
		//input stuff
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT &&
					eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c') {
						ship_color = rand() % 256;
					}
				}
				if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						FROM_LEFT_1ST_BUTTON_PRESSED) {
						ship_color = rand() % 256;
					}
					if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						ship.X = posx - 2;
						ship.Y = posy;
					}
				}
			}
			delete[] eventBuffer;
		}
		//collision detecting
		for (int i = 0; i < scount; i++)
		{
			if ((star[i].X == ship.X || star[i].X == ship.X + 1 || star[i].X == ship.X + 2 || star[i].X == ship.X + 3 || star[i].X == ship.X + 4) 
				&& star[i].Y == ship.Y)
			{
				hp -= 1;
				star[i] = { (SHORT(rand() % screen_x)),SHORT(1) };
			}
		}

		//function calling
		star_fall();
		clear_buffer();
		fill_star_to_buffer();
		fill_ship_to_buffer(ship.X, ship.Y, ship_color);
		fill_buffer_to_console();
		Sleep(100);
	}
	return 0;
}


