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

#include <QtNetwork>

#include "connection.h"
#include "server.h"

Server::Server(QObject *parent)
	: QTcpServer(parent){

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

		listenServerPort = (quint16)0;
}

quint16 Server::getServerPort(){
	return listenServerPort;
}

void Server::incomingConnection(qintptr socketDescriptor){
	Connection *connection = new Connection(this);
	connection->setSocketDescriptor(socketDescriptor);
	emit newConnection(connection);
}

void Server::setServerPort(int port){
	listenServerPort = (quint16)port;
}

void Server::startListening(){
	
}

void Server::stopListening(){
	
}

QByteArray Server::userName() const{
	return username;
}