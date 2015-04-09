#ifndef GOMODEL_H
#define GOMODEL_H

#include <QString>
#include <QHostAddress>
#include <QMultiHash>
#include <QObject>
#include <QStringList>
#include <QRegularExpression>
#include <QTime>

#include "Display.h"

class GoModel : public QObject
{
	Q_OBJECT

public:	
	GoModel(QObject *parent);
	~GoModel();
	
	int checkLiberties(char move,int y, int x, bool recursive);
	QString finalStatusList(QString status);
	void forfeit(const QString &from);
	
	QString generateLegalMoves(char move);

	char* getGrid();
	int getGridSize();
	int getHandicap();
	float getKomi();
	QString getFinalScore();
	QString getNameBlack();
	QString getNameWhite();
	QString getNameEngine();
	int getNumberOfPlayers();
	QString getPlayerColour(const QString &from);
	QString getPlayerID(const QString &from);
	QString getPrintableGrid();
	QString getPrintableGridRaw();
	QString getVersionEngine();
	QString getVersionProtocol();
	bool isBlackTurn();
	bool isGameOver();
	bool isPlayerInGame(QString player);
	bool isValidMove(QString proposedMove);
	QString makeMove(const QString &from, QString playerMove);
	void test();
	void test2();
	void updateGridSize(int size);
	void updateHandicap(int newHandicap);
	void updateKomi(float newKomi);
	QTime uptime;

private:
	bool blackTurn;
	int capturedBlack;
	int capturedWhite;
	QString finalScore;
	bool gameEnded;
	bool gameStarted;
	QTime gameTime;
	char* grid;
	int gridSize;
	int handicap;
	float komi;
	bool previousMovePass;
	QString player1UserID;
    QString player2UserID;
	int numberOfPlayers;
	QStringList toBeCaptured;

	void buildGrid();
	void deleteGrid();
	void gameOver();
	void swapPlayerTurn();

public slots:
	void newParticipant(QString);
	void participantLeft(QString);

signals:
	void gameFull(QString);
	void gameOver(QString);

};
#endif