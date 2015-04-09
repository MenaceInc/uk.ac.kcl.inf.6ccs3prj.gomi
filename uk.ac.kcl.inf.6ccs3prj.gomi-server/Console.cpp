#include "Console.h"

QString ConsoleBuffer;
QTextStream textConsole(stdin);

Console::Console(QObject *parent) : QObject(parent){

}

Console::Console(){

}

Console::~Console(){

}


void Console::prompt(){

	Display::printToScreen("\n");
	Display::printToScreen(">>> ");
	input = textConsole.readLine();
	emit newCommand(input);
}