#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QCoreApplication>
#include <QStringList>
#include <QTcpSocket>
#include <QTime>
#include "Display.h"
#include "Network.h"

class Controller : public QObject
{
	Q_OBJECT

public:
	Controller(QObject *parent, Network *networkSession);
	~Controller();
	
private:
	Network* networkSession;
	QString targetHost;
	QString clientversion;
	QTime time;
	
public slots:
	void networkError(QTcpSocket::SocketError socketError);
	void serverResponse(const QString &from, const QString &message);
	void processCommand(QString);


signals:
	void doneProcessingCommand();
	void shutdown();
};

#endif