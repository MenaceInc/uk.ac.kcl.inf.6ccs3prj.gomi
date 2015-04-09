#include <QTextStream>
#include "Display.h"

Display::Display(){
	clear_screen();
}

Display::~Display(){

}

void Display::clear_screen(){
#ifdef _WIN32
	std::system("CLS");
#else
	// Assume POSIX
	std::system("clear");
#endif
}