
#include <QThread>
#include <QCoreApplication>
#include "Console.h"
#include "Controller.h"
#include "GoModel.h"
#include "Network.h"

int main(int argc, char *argv[]){
	QCoreApplication GoMI(argc, argv);

	Controller control(&GoMI);
	control.processCommand("status");

	QThread *consoleThread = new QThread();
	Console *consoleSession = new Console();
	consoleSession->moveToThread(consoleThread);
	consoleThread->start();
	QMetaObject::invokeMethod(consoleSession, "prompt", Qt::QueuedConnection);

	QObject::connect(consoleSession, &Console::newCommand,
		&control, &Controller::processCommand);
	QObject::connect(&control, &Controller::doneProcessingCommand,
		consoleSession, &Console::prompt);

	return GoMI.exec();
}
