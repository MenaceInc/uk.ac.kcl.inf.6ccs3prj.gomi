#include "Controller.h"

Controller::Controller(QObject *parent, Network *networkSession)	: QObject(parent){
	time.start();
	this->networkSession = networkSession;
	clientversion = "GoMI client v0.1";

	connect(networkSession, SIGNAL(newMessage(QString,QString)),
		this, SLOT(serverResponse(QString,QString)));
}

Controller::~Controller(){

}

void Controller::networkError(QTcpSocket::SocketError socketError){
	Display::printToScreen(socketError);
}

void Controller::serverResponse(const QString &from, const QString &command){
	Display::printToScreen("\n" + from + " says " + command + "\n");	
}

void Controller::processCommand(QString command){
	if(command.isEmpty()){
		command.append("?");
	}

	QStringList input = command.toLower().split(" ");
	input.removeAll("");

	if(input.at(0) == "?"){
		Display::printToScreen("You are running " + clientversion + "\n"
			+ "To get a full list of commands available, type listcommands");
	}

	if(input.at(0) == "connect"){
		if(input.count() < 2){
			targetHost.append("localhost");
		} else {
			targetHost.append(input.at(1));
		}
		networkSession->connectToServer(targetHost);
	}

	if(input.at(0) == "clear"){
		Display::clear_screen();
	}

	if(input.at(0) == "disconnect"){
		if(networkSession->disconnectFromServer()){
			Display::printToScreen("\nDisconnected from " + targetHost);
		} else {
			Display::printToScreen(clientversion + " is not connected to a server");
		}
	}

	if(input.at(0) == "listusers"){
		//sendMessage
	}

	if(input.at(0) == "list_commands"){

	}

	if(input.at(0) == "myip"){
		Display::printToScreen(networkSession->myIP());
	}

	if(input.at(0) == "play"){
		//sendMessage([id] + "play " + [move])
	}

	if(input.at(0) == "quit"){
		Display::printToScreen("\nShutting down...\n\n\n");
		QCoreApplication::exit(0);
		return;
	}

	if(input.at(0) ==  "say"){
		//sendMessage([id] + "say " + [message])
		QString message;
		input.mid(1);
		message.append(command.toLower().mid(4));
		networkSession->sendMessage(message);
	}

	if(input.at(0) == "showgrid"){
		//Display::printToScreen(gameSession->getPrintableGrid());
	}

	if(input.at(0) == "status"){
		Display::printToScreen("User: " + networkSession->getUsername() + "\n");
		/*if(networkSession->isListening()){
		Display::printToScreen("Listening for connections at: " + networkSession->serverAddress() + ":"
		+ QString(networkSession->serverPort()) + "\n");
		} else {
		Display::printToScreen("Not listening for connections...\n");
		}*/
	}

	if(input.at(0) == "test"){
		Display::printToScreen((char)7);
		Display::printToScreen(networkSession->nickName());
	}

	if(input.at(0) == "time"){
		Display::printToScreen("The local time is : " + time.currentTime().toString() + "\n");
		int secondsElapsed = (time.elapsed() / 1000);
		int minutesElapsed = (secondsElapsed / 60);
		int hoursElapsed = (minutesElapsed / 60);
		Display::printToScreen("Time elapsed: " + QString::number(hoursElapsed) + "h "
			+ QString::number(minutesElapsed) + "m " + QString::number(secondsElapsed) + "s\n");
	}

	if(input.at(0) == "uptime"){

	}

	emit doneProcessingCommand();
}