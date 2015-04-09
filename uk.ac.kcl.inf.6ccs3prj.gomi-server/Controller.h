#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QCoreApplication>
#include <QStringList>
#include <QTcpSocket>
#include <QTime>
#include "GoModel.h"
#include "Display.h"
#include "Network.h"

class Controller : public QObject
{
	Q_OBJECT

public:
	Controller(QObject *parent);
	~Controller();
	
private:
	GoModel *gameSession;
	Network *networkSession;
	QString targetHost;
	QString targetPort;
	
	QStringList listCommands;
	QTime uptime;


	
public slots:
	void newIncomingCommand(const QString &from, const QString &message);
	void newParticipant(QString);
	void participantLeft(QString);
	void processCommand(QString);
	void gameOver(QString winner);


signals:
	void doneProcessingCommand();
	void shutdown();
};

#endif