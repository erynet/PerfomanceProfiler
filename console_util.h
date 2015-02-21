#include "common.h"

class ConsoleUtil {
private:
	HANDLE hStdOutput;
	WORD wOldColorAttrs;
public:
	ConsoleUtil();
	~ConsoleUtil();

	void Reset();
	void Go(int, int);
	void GoR(int, int);
	void CC(bool);

	COORD b;
	COORD c;

	inline friend ConsoleUtil& operator<<(ConsoleUtil& cu, string& s){
		cout << s;
		return cu;
	}
	inline friend ConsoleUtil& operator<<(ConsoleUtil& cu, string* s){
		cout << (*s);
		return cu;
	}
	inline friend ConsoleUtil& operator<<(ConsoleUtil& cu, char* c){
		cout << string(c);
		return cu;
	}
};

ConsoleUtil::ConsoleUtil(){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdOutput, &csbi);
	wOldColorAttrs = csbi.wAttributes;
	Reset();
}

ConsoleUtil::~ConsoleUtil()
{
}

void ConsoleUtil::Reset(){
	COORD coord = { 0, 0 };
	DWORD cCharsWritten, dwConSize;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi)) { return; }
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	if (!FillConsoleOutputCharacter(hStdOutput, (TCHAR)' ', dwConSize, coord, &cCharsWritten)) { return; }
	if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi)) { return; }
	if (!FillConsoleOutputAttribute(hStdOutput, csbi.wAttributes, dwConSize, coord, &cCharsWritten)) { return; }
	SetConsoleCursorPosition(hStdOutput, coord);

	if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi)) { return; }
	b.X = csbi.dwCursorPosition.X; c.X = csbi.dwCursorPosition.X;
	b.Y = csbi.dwCursorPosition.Y; c.Y = csbi.dwCursorPosition.Y;
}

void ConsoleUtil::Go(int col, int row){
	COORD coord;
	coord.X = col; coord.Y = row;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (SetConsoleCursorPosition(hConsole, coord)){
		c.X = coord.X;
		c.Y = coord.Y;
	}
}

void ConsoleUtil::GoR(int dtx, int dty){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi)) { return; }
	c.X = csbi.dwCursorPosition.X + dtx;
	c.Y = csbi.dwCursorPosition.Y + dty;
	Go(csbi.dwCursorPosition.X + dtx, csbi.dwCursorPosition.Y + dty);
}

void ConsoleUtil::CC(bool b){
	if (b){
		SetConsoleTextAttribute(hStdOutput, FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY);
	}
	else{
		SetConsoleTextAttribute(hStdOutput, wOldColorAttrs);
	}
}