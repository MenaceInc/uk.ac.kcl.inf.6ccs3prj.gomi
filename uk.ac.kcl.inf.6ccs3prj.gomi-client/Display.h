#ifndef DISPLAY_H
#define DISPLAY_H

#include <QTextStream>

static QTextStream textOutput(stdout);

class Display
{
public:
	Display();
	~Display();
	static void clear_screen();
#include "DisplayTemplateFunctions.hxx"
};
#endif