#include "GoModel.h"


static const QString nameEngine = "GoMI";
static const QString versionEngine = "v0.1";
static const QString versionProtocol = "2";

//	Public functions

GoModel::GoModel(QObject *parent)
	: QObject(parent) {	
		blackTurn = true;
		gridSize = 19;
		buildGrid();

		gameTime.start();
		handicap = 0;
		komi = 7.5;
		numberOfPlayers = 0;
		previousMovePass = false;
		gameStarted = false;
		gameEnded = false;
		finalScore = "0";
}

GoModel::~GoModel(){
	deleteGrid();

}

QString GoModel::finalStatusList(QString status){
	return "whatever";
}


void GoModel::forfeit(const QString &from){
	updateGridSize(gridSize);
}


QString GoModel::getFinalScore(){
	return finalScore;
}

char* GoModel::getGrid(){
	return grid;
}

int GoModel::getGridSize(){
	return gridSize;
}

int GoModel::getHandicap(){
	return handicap;
}

float GoModel::getKomi(){
	return komi;
}

QString GoModel::getNameEngine(){
	return nameEngine;
}


int GoModel::getNumberOfPlayers(){
	return numberOfPlayers;
}


QString GoModel::getPlayerColour(const QString &from){
	if(player1UserID.contains(from)){
		return "b";
	}
	if(player2UserID.contains(from)){
		return "w";
	}
	return "none";
}

QString GoModel::getPlayerID(const QString &from){
	if(player1UserID.contains(from)){
		return "Player_1";
	}
	if(player2UserID.contains(from)){
		return "Player_2";
	}
	return "none";
}

QString GoModel::getPrintableGrid(){
	QString output("   ");

	for(int i = 0; i <= gridSize; i++){
		if(i == 0){
			char xAxisLabel = 'a';
			for(int j = 0; j < gridSize; j++){
				if(j == 8){
					xAxisLabel++;
				}

				output.append(xAxisLabel);
				output.append(" ");
				xAxisLabel++;
			}
			output.append("\n");
		} else {
			output.append(QString::number(i));
			if(i < 10){
				output.append("  ");
			} else {
				output.append(" ");
			}
			for(int k = 0; k < gridSize; k++){
				output.append(QString(grid[((i-1)*gridSize)+k]));
				if(k != gridSize-1){
					output.append(",");
				}
			}
			output.append("\n");
		}
	}
	return output;
}

QString GoModel::getPrintableGridRaw(){
	QString output;

	for(int i = 0; i < gridSize; i++){
		for(int j = 0; j < gridSize; j++){
			output.append(QString(grid[((i)*gridSize)+j]));
			if(j != gridSize-1){
				output.append(",");
			}
		}
		output.append("\n");
	}
	return output;
}

QString GoModel::getVersionEngine(){
	return versionEngine;
}

QString GoModel::getVersionProtocol(){
	return versionProtocol;
}

bool GoModel::isBlackTurn(){
	return blackTurn;
}

bool GoModel::isGameOver(){
	return gameEnded;
}

bool GoModel::isPlayerInGame(QString name){
	if(player1UserID.contains(name) || player2UserID.contains(name)){
		return true;
	}
	return false;
}

bool GoModel::isValidMove(QString proposedMove){
	QStringList vertex = proposedMove.split(" ");
	QRegularExpression regex("^[a-hj-z](2[0-5]|1?\\d)$");
	char move = vertex.at(0).at(0).toLatin1();

	if(!regex.match(vertex.at(1)).hasMatch()){
		return false;
	}



	int x = vertex.at(1).at(0).toLatin1();
	int y = (vertex.at(1).mid(1).toInt()) - 1;
	if(x >= 105){
		--x;
	}
	x = x - 97;

	char target = grid[(y*gridSize)+x];
	if(target != 'e'){
		return false;
	}

	int numberOfLib = checkLiberties(move, y, x, false);
	Display::printToScreen(QString::number(numberOfLib) + "\n");
	for(int i = 0; i < toBeCaptured.count();i++){
		Display::printToScreen(toBeCaptured.at(i) + "\n");
	}


	QString original = QString::number(y) + ":" + QString::number(x);

	Display::printToScreen("Original" + original + "\n");
	toBeCaptured.removeAll(original);
	QString check1 = QString::number(y-1) + ":" + QString::number(x);
	QString check2 = QString::number(y+1) + ":" + QString::number(x);
	QString check3 = QString::number(y) + ":" + QString::number(x-1);
	QString check4 = QString::number(y) + ":" + QString::number(x+1);

	for(int i = 0; i < toBeCaptured.count();i++){
		Display::printToScreen(toBeCaptured.at(i) + "\n");
	}

	Display::printToScreen(check1 + "\n");

	Display::printToScreen(check2 + "\n");

	Display::printToScreen(check3 + "\n");

	Display::printToScreen(check4 + "\n");

	if(toBeCaptured.contains(check1) && numberOfLib == 1 || toBeCaptured.contains(check2) && numberOfLib == 1 ||\
		toBeCaptured.contains(check3) && numberOfLib == 1 || toBeCaptured.contains(check4) && numberOfLib == 1){
			toBeCaptured.clear();
			return false;
	}

	if(numberOfLib < 2){
		return false;
	}

	return true;
}

QString GoModel::makeMove(const QString &from, QString playerMove){
	QString result;
	QString id = getPlayerID(from);
	QStringList moveList = playerMove.split(" ");


	char move;
	if(moveList.at(0).at(0).toLatin1() == 'b'){
		move = 'b';
	} else {
		move = 'w';
	}

	if((isBlackTurn() && move == 'w')||(!isBlackTurn() && move == 'b')){
		result = "?" + id + " is not " + id + "'s turn";
		return result;
	}

	if(playerMove == "pass" && previousMovePass){
		emit gameOver();
		result = "=" + id + " play gameover " + QString::number(uptime.elapsed());
		swapPlayerTurn();
		return result;
	}

	if(playerMove == "pass" && !previousMovePass){
		previousMovePass = true;
		result = "=" + id + " play pass";
		swapPlayerTurn();
		return result;
	}

	if(!isValidMove(playerMove)){
		result = "?" + id + " \"" + playerMove + "\" is not a valid move";
		return result;
	}


	QString temp = moveList.at(1);
	int x = temp.at(0).toLatin1();
	int y = (temp.mid(1).toInt()) - 1;
	if(x >= 105){
		--x;
	}
	x = x - 97;

	if(x < 0 || y < 0 || x >= getGridSize() || y >= getGridSize()){
		result = "?" + id + " \"" + playerMove + "\" is not a valid move";
		return result;
	}
	grid[(y*gridSize)+x] = move;

	for(int i = 0; i < toBeCaptured.count(); i++){
		int dely = toBeCaptured.at(i).split(":").at(0).toInt();
		int delx = toBeCaptured.at(i).split(":").at(1).toInt();
		grid[(dely*gridSize)+delx] = 'e';
		if(move == 'b'){
			capturedWhite++;
		} else {
			capturedBlack++;
		}
	}
	toBeCaptured.clear();
	previousMovePass = false;
	swapPlayerTurn();
	result = "=" + id + " " + playerMove;
	return result;
}

void GoModel::newParticipant(QString from){

	if(numberOfPlayers < 2 && player1UserID.isEmpty()){

		Display::printToScreen("Player 1 : " + from + " added\n");
		player1UserID = from;
		numberOfPlayers++;
		uptime.start();
	} else if(numberOfPlayers < 2 && player2UserID.isEmpty()){


		Display::printToScreen("Player 2 : " + from + " added\n");
		player2UserID = from;
		numberOfPlayers++;
		uptime.start();
	} else {
		emit gameFull(from);
	}

}

void GoModel::participantLeft(QString from){
	if(player1UserID.contains(from)){

		Display::printToScreen("Player1 " + from + " left\n");
		player1UserID = "";
		numberOfPlayers--;
		updateGridSize(gridSize);
		return;
	}

	if(player2UserID.contains(from)){

		Display::printToScreen("Player2 " + from + " left\n");
		player2UserID = "";
		numberOfPlayers--;
		updateGridSize(gridSize);
	}
}

void GoModel::updateGridSize(int size){
	gridSize = size;
	blackTurn = true;
	gameTime.start();
	handicap = 0;
	komi = 7.5;
	previousMovePass = false;
	gameStarted = false;
	gameEnded = false;
	finalScore = "0";
	buildGrid();
}

void GoModel::updateHandicap(int newHandicap){
	komi = newHandicap;
}

void GoModel::updateKomi(float newKomi){
	komi = newKomi;
}

//	Private functions

void GoModel::buildGrid(){
	grid = new char[gridSize*gridSize];

	for(int i = 0; i < gridSize*gridSize; i++){
		grid[i] = 'e';

	} 
}

int GoModel::checkLiberties(char move, int y, int x, bool recursive){
	QStringList toCheckList;
	toCheckList.append(QString::number(y) + ":" + QString::number(x));
	QStringList checkedList;
	int liberties = 0;
	int checkX = x;
	int checkY = y;
	char moveToCheck = move;
	while(!toCheckList.isEmpty()){
		checkY = toCheckList.at(0).split(":").at(0).toInt();
		checkX = toCheckList.at(0).split(":").at(1).toInt();


		int toCheck[] = {checkY,checkX-1,checkY,checkX+1,checkY-1,checkX,checkY+1,checkX};


		for (int i = 0; (i*2) < 8; i++){
			int tempY = toCheck[i*2];
			int tempX = toCheck[(i*2)+1];

			if(tempX < 0 || tempY < 0 || tempX >= getGridSize() || tempY >= getGridSize()){
				checkedList.append(QString::number(tempY) + ":" + QString::number(tempX));
			} else {
				if(!checkedList.contains(QString::number(tempY) + ":" + QString::number(tempX))){

					char temp = grid[(tempY*gridSize)+tempX];

					if(temp == moveToCheck){
						toCheckList.append(QString::number(tempY) + ":" + QString::number(tempX));
					} else {
						if(temp == 'e'){
							liberties++;
						} else {
							if (moveToCheck == 'b' && !recursive){
								int enemyLib = checkLiberties('w',tempY,tempX,true);
								if(enemyLib < 2){
									liberties++;
								}
							}
							if (moveToCheck == 'w' && !recursive){
								int enemyLib = checkLiberties('b',tempY,tempX,true);
								if(enemyLib < 2){
									liberties++;
								}
							}
						}

					}
					checkedList.append(QString::number(tempY) + ":" + QString::number(tempX));
				}
			}
		}
		toCheckList.removeAt(0);
	}
	if(liberties < 2 && recursive){
		checkedList.append(QString::number(y) + ":" + QString::number(x));
		for(int i = 0; i < checkedList.count(); i++){
			int dely = checkedList.at(i).split(":").at(0).toInt();
			int delx = checkedList.at(i).split(":").at(1).toInt();

			char deltarget = grid[(dely*gridSize)+delx];

			if(moveToCheck ==  deltarget){
				toBeCaptured.append(QString::number(dely) + ":" + QString::number(delx));
			}
		}
	}
	return liberties;
}

void GoModel::deleteGrid(){
	delete [] grid;
}

void GoModel::test(){
	grid[(2*gridSize)+0] = 'b';
	grid[(1*gridSize)+1] = 'b';
	grid[(3*gridSize)+1] = 'b';
	grid[(0*gridSize)+2] = 'b';
	grid[(4*gridSize)+2] = 'b';
	grid[(1*gridSize)+3] = 'b';
	grid[(3*gridSize)+3] = 'b';
	grid[(1*gridSize)+4] = 'b';
	grid[(3*gridSize)+4] = 'b';
	grid[(2*gridSize)+5] = 'b';

	grid[(1*gridSize)+2] = 'w';
	grid[(2*gridSize)+1] = 'w';
	grid[(2*gridSize)+3] = 'w';
	grid[(3*gridSize)+2] = 'w';
	grid[(2*gridSize)+4] = 'w';
}

void GoModel::test2(){
	grid[(0*gridSize)+2] = 'b';
	grid[(1*gridSize)+1] = 'b';
	grid[(2*gridSize)+2] = 'b';
	grid[(1*gridSize)+3] = 'b';

	grid[(0*gridSize)+1] = 'w';
	grid[(1*gridSize)+0] = 'w';
	grid[(2*gridSize)+1] = 'w';
}

void GoModel::swapPlayerTurn(){
	if(blackTurn){
		blackTurn = false;
	} else {
		blackTurn = true;
	}
}

QString GoModel::generateLegalMoves(char colour){
	QString result;
	QString move;

	for(int x = 0; x < gridSize; x++){
		for(int y = 0; y < gridSize; y++){
			if(y < 8){
				move.append(((char)y+97) + QString::number(x+1) + ",");
			} else {
				move.append(((char)y+98) + QString::number(x+1) + ",");
			}
			Display::printToScreen(move + "\n");
		}
	}
	QStringList temp = move.split(",");
	temp.removeAll("");
	
	for(int i = 0; i < temp.count(); i++){
		Display::printToScreen(QString::number(i) + " ");
		Display::printToScreen(temp.at(i) + " ");
		if(isValidMove(QString(colour) + " " + temp.at(i))){
			result.append(temp.at(i) + ",");
		}
	}
		return result;
}


void GoModel::gameOver(){

}