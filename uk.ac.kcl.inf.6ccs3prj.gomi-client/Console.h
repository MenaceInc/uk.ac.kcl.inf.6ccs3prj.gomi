#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QTextStream>
#include <QString>
#include "Display.h"

class Console : public QObject
{
	Q_OBJECT

public:
	Console();
	Console(QObject *parent);
    ~Console();

private:
	QString input;

signals:
	void newCommand(QString command);

public slots:
	void prompt();
};

#endif