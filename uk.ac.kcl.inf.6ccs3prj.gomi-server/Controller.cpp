#include "Controller.h"

Controller::Controller(QObject *parent)	: QObject(parent){
	gameSession = new GoModel(this);
	networkSession = new Network();
	uptime.start();

	connect(networkSession, SIGNAL(newParticipant(QString)),
		this, SLOT(newParticipant(QString)));
	connect(networkSession, SIGNAL(participantLeft(QString)),
		this, SLOT(participantLeft(QString)));

	connect(networkSession, SIGNAL(newParticipant(QString)),
		gameSession, SLOT(newParticipant(QString)));
	connect(networkSession, SIGNAL(participantLeft(QString)),
		gameSession, SLOT(participantLeft(QString)));
	connect(gameSession, SIGNAL(gameFull(QString)),
		networkSession, SLOT(gameFull(QString)));

	connect(networkSession, SIGNAL(newMessage(QString,QString)),
		this, SLOT(newIncomingCommand(QString,QString)));

	listCommands.append("boardsize");
	listCommands.append("clear_board");
	listCommands.append("getboard");
	listCommands.append("final_score");
	listCommands.append("final_status_list");
	listCommands.append("forfeit");
	listCommands.append("getboard");
	listCommands.append("known_command");
	listCommands.append("pass");
	listCommands.append("play");
	listCommands.append("protocol_version");
	listCommands.append("ready");
	listCommands.append("say");
	listCommands.append("showboard");
	listCommands.append("version");

}

Controller::~Controller(){

}


void Controller::newIncomingCommand(const QString &from, const QString &command){
	if(gameSession->isPlayerInGame(from)){
		QString userID = gameSession->getPlayerID(from);
		QString userColour = gameSession->getPlayerColour(from);

		if(command.isEmpty()){
			networkSession->privMessage(from, "?" + userID + " nocommand");
			return;
		}


		Display::printToScreen("\n" + from + " says " + command + "\n");
		QStringList commandSplit = command.split(' ');



		if(commandSplit.at(1) == "boardsize" || commandSplit.at(1) == "newgame"){
			if(commandSplit.count() < 2){
				networkSession->privMessage(from, "?" + userID + " " + command + " is not a valid command");
			} else {

				gameSession->updateGridSize(commandSplit.at(2).toInt());
				networkSession->sendMessage(command);
				Display::printToScreen("\n=" + command +  "\n");
			}
			return;

		}

		if(commandSplit.at(1) == "clear_board"){
			gameSession->updateGridSize(gameSession->getGridSize());
		}

		if(commandSplit.at(1) == "final_score"){
			if(gameSession->isGameOver()){
				networkSession->privMessage(from, "=" + userID + " final_score " + gameSession->getFinalScore());
				return;
			} else {
				networkSession->privMessage(from, "?" + userID + " The game is not over yet");
				return;
			}
		}

		if(commandSplit.at(1) == "final_status_list"){
			QString result = gameSession->finalStatusList(commandSplit.at(2));
			networkSession->privMessage(from, result);
		}

		if(commandSplit.at(1) == "forfeit"){
			gameSession->forfeit(from);
			networkSession->sendMessage("=" + userID + " forfeit");
		}

		if(commandSplit.at(1) == "getboard"){
			QString board = gameSession->getPrintableGridRaw();
			networkSession->privMessage(from, "=" + userID + " getboard " + QString::number(gameSession->getGridSize()) + " \n" + board);
			return;
		}
		if(commandSplit.at(1) == "getlegalmoves"){
			QString moves = gameSession->generateLegalMoves(userColour.at(0).toLower().toLatin1());
			networkSession->privMessage(from, "=" + userID + " getlegalmoves " + moves);
			return;
		}
		if(commandSplit.at(1) == "known_command"){
			if(listCommands.contains(commandSplit.at(2))){
				networkSession->privMessage(from, "=" + userID + " " +command);
			} else {

				networkSession->privMessage(from, "?" + userID + " " + command);
			}
		}




		if(commandSplit.at(1) == "myid"){
			networkSession->privMessage(from, "=" + userID + " myid " + userID + " " + userColour);
			return;
		}

		if(commandSplit.at(1) == "play"){
			Display::printToScreen(command + "\n");
			Display::printToScreen(commandSplit.at(2) + "\n");
			commandSplit.removeAt(0);
			commandSplit.removeAt(0);

			QString result = gameSession->makeMove(from, commandSplit.join(" "));
			Display::printToScreen(result + "\n");
			if(result.at(0) == "="){
				networkSession->sendMessage(result);
				return;
			} else {
				networkSession->privMessage(from, result);
				return;
			}

		}

		if(commandSplit.at(1) == "protocol_version"){
			networkSession->privMessage(from, "=" + userID + " protocol_version " + gameSession->getVersionProtocol());
			return;
		}



		if(commandSplit.at(1) == "say"){
			commandSplit.removeAt(0);
			commandSplit.removeAt(0);
			QString chatMessage = commandSplit.join(' ');
			Display::printToScreen(chatMessage + "\n");
			networkSession->sendMessage("=" + userID + " say " + chatMessage);
			Display::printToScreen("\n=" + userID + " say " + chatMessage +  "\n");
			return;
		}

		if(commandSplit.at(1) == "version"){
			networkSession->privMessage(from, "=" + userID + " version " + gameSession->getVersionEngine());
			return;
		}

	}
}

void Controller::newParticipant(QString in){
	Display::printToScreen("\nNew player: " + in + " has joined the server\n");
}

void Controller::processCommand(QString command){

	Display::printToScreen(QString::number(gameSession->getNumberOfPlayers()) + "\n");
	if(command.isEmpty()){
		command.append("?");
	}

	QStringList input = command.toLower().split(" ");
	input.removeAll("");

	if(input.at(0) == "?"){
		Display::printToScreen("You are running " + gameSession->getNameEngine() + " "
			+ gameSession->getVersionEngine() + "\n"
			+ "To get a full list of commands available, type listcommands");
	}

	if(input.at(0).toLower() == "boardsize" || input.at(0).toLower() == "newgame"){


		if(input.count() < 2){
			Display::printToScreen("Starting new game with size 19\n");
			gameSession->updateGridSize(19);
		} else {
			if(input.at(1).toInt() >  1 && input.at(1).toInt() < 26){
				Display::printToScreen("Starting new game with size " + input.at(1) + "\n");
				gameSession->updateGridSize(input.at(1).toInt());
			} else {

				Display::printToScreen(input.at(1) + " is not a valid size option\n");
			}
		}
	} 




	if(input.at(0) == "clear"){
		Display::clear_screen();
	}

	if(input.at(0) == "listusers"){
		Display::printToScreen("\n");
		QStringList list = networkSession->listUsers();
		for (int i = 0; i < list.count(); i++){
			Display::printToScreen(list.at(i) + "\n");
		}
	}

	if(input.at(0) == "listcommands"){

	}

	if(input.at(0) == "myip"){
		Display::printToScreen(networkSession->serverAddress());
	}

	if(input.at(0) == "quit"){
		Display::printToScreen("\nShutting down...\n\n\n");
		QCoreApplication::exit(0);
		return;
	}

	if(input.at(0) ==  "say"){
		QString message;
		input.mid(1);
		message.append(command.toLower().mid(4));
		networkSession->sendMessage(message);
		Display::printToScreen(message);
	}

	if(input.at(0) == "showboard"){
		Display::printToScreen(gameSession->getPrintableGrid());
	}

	if(input.at(0) == "start"){
		if(!networkSession->isListening()){
			networkSession->startListening();
		} 
	}

	if(input.at(0) == "status"){
		if(networkSession->isListening()){
			Display::printToScreen("Listening for connections at: " + networkSession->serverAddress() + ":"
				+ QString(networkSession->serverPort()) + "\n");
		} else {
			Display::printToScreen("Not listening for connections...\n");
		}
	}



	if(input.at(0) == "time"){
		Display::printToScreen("The local time on the server is : " + uptime.currentTime().toString());
	}

	if(input.at(0) == "uptime"){
		int secondsElapsed = (uptime.elapsed() / 1000);
		int minutesElapsed = (secondsElapsed / 60);
		int hoursElapsed = (minutesElapsed / 60);
		Display::printToScreen("Uptime: " + QString::number(hoursElapsed) + "h "
			+ QString::number(minutesElapsed) + "m " + QString::number(secondsElapsed) + "s");
	}

	emit doneProcessingCommand();
}

void Controller::participantLeft(QString in){
	Display::printToScreen("\nPlayer: " + in + " has left the server\n>>>");
}

void Controller::gameOver(QString winner){
	networkSession->sendMessage("=final_score " + winner);
}