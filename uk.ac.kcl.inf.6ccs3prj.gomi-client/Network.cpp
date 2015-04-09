/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "Network.h"

Network::Network(){

	QStringList envVariables;
		envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
			<< "HOSTNAME.*" << "DOMAINNAME.*";

		QStringList environment = QProcess::systemEnvironment();
		foreach(QString string, envVariables){
			int index = environment.indexOf(QRegExp(string));
			if(index != -1){
				QStringList stringList = environment.at(index).split('=');
				if(stringList.size() == 2){
					username = stringList.at(1).toUtf8();
					break;
				}
			}
		}

		if(username.isEmpty()){
			username = "unknown";
		}
}

QString Network::connectionPort(){
	return QString::number(connection->peerPort());
}

bool Network::disconnectFromServer(){
	if(connection){
	connection->disconnectFromHost();
	connection = 0;
	return true;
	}
	return false;
}
void Network::connectToServer(QString address){
	QStringList splitAddress = address.split(":");
	hostAddress = splitAddress.at(0);
	if(splitAddress.count() < 2){
		hostPort = (quint16)6969;
	} else {
		hostPort = (quint16)address.split(":").at(1).toInt();
	}
	connection = new Connection(this);
	connection->setGreetingMessage(username);
	connection->connectToHost(hostAddress, hostPort);
	connect(connection, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(connectionError(QAbstractSocket::SocketError)));
	connect(connection, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(connection, SIGNAL(readyForUse()), this, SLOT(readyForUse()));

}

QString Network::myIP(){
	QString ipAddresses = QHostInfo::localHostName();
	
	if(ipAddresses == "0.0.0.0"){
		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
		QStringList addressesToSort;
		for(int i = 0; i < ipAddressesList.count(); i++){
			if(ipAddressesList[i].protocol() == QAbstractSocket::IPv4Protocol){
				addressesToSort.append(ipAddressesList.value(i).toString());
			}
		}
		addressesToSort.sort();
		return addressesToSort.at(1);
	}
	return ipAddresses;
}

QString Network::nickName() const{
	return QString(username) + '@' + QHostInfo::localHostName()
		+ ':' + QString::number(connection->localPort());
}

QString Network::getUsername() const{
	return QString(username);
}

void Network::sendMessage(const QString &message){
	if(message.isEmpty()){
		return;
	}
		connection->sendMessage(message);
}

// Slots

void Network::connectionError(QAbstractSocket::SocketError){
	connection->deleteLater();
}

void Network::disconnected(){
	connection->deleteLater();
}

void Network::readyForUse(){
	connect(connection, SIGNAL(newMessage(QString,QString)),
		this, SIGNAL(newMessage(QString,QString)));
}
