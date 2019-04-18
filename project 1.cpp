#include <time.h>
#include <iostream>
#include <deque>
#include <string>
using namespace std;

const int TOTAL_PLAYERS = 2;
const int HAND_SIZE = 10; // Must be less than 28 divided by TOTAL_PLAYERS

struct domino_piece{
	int left, right;
};

class CRandom{
	public:
		CRandom(){}  // constructor
		~CRandom(){} // destructor

		int getRandomPublic(int rangeLow, int rangeHigh){
			int rand_scaled=getRandomPrivate(rangeLow, rangeHigh);
			return rand_scaled;
		}
	private:
		// Generates uniform distribution between rangeLow and rangeHigh
		int getRandomPrivate(int rangeLow, int rangeHigh) {
		    double r = rand()/(1.0 + RAND_MAX);
		    int range = rangeHigh - rangeLow + 1;
		    int rand_scaled = (r * range) + rangeLow;
		    return rand_scaled;
		}
};

class CDomino{
	public:
		CDomino(){}  // constructor
		~CDomino(){} // destructor

		deque<domino_piece> dominoes;

		void API(void){
			init();
		}
		// Returns domino object and removes it from deque
		domino_piece getPiece(int pieceID){
			domino_piece copy = dominoes.at(pieceID);
			dominoes.erase(dominoes.begin() + pieceID);
			return(copy);
		}
	private:
		void init(void){
			domino_piece mypiece;
			for(int right=0;right<7;right++){
				for(int left=right; left<7;left++){
					mypiece.right=right;
					mypiece.left=left;
					dominoes.push_back(mypiece);
				}
			}
		}
};

class CPlayer : public CRandom{
	public:
		CPlayer(){}  // constructor
		~CPlayer(){} // destructor

		deque<domino_piece> playerHand;
		class CDomino *dominoOBJ=NULL;

		void API(CDomino *receive_dominoPointerOBJ){
			dominoOBJ=receive_dominoPointerOBJ;
		}
		// Returns domino object and removes it from deque
		domino_piece getPiece(int pieceID) {
			domino_piece copy = playerHand.at(pieceID);
			playerHand.erase(playerHand.begin() + pieceID);
			return(copy);
		}
		void takeFromPile(int pieceNo){
			domino_piece takenPiece;
			takenPiece = dominoOBJ->getPiece(pieceNo);
			playerHand.push_back(takenPiece);
		}
		// Removes numPieces number of domino pieces from domino deque and puts them into playerHand
		void drawPieces(int numPieces) {
			for (int i = 0; i < numPieces; i++) {
				int pieceNo = getRandomPublic(0, dominoOBJ->dominoes.size() - 1);
				takeFromPile(pieceNo);
			}
		}
		// Finds the first domino in playerHand that has a value matching either the head or tail and returns 
		// its position in playerHand. If no such domino exists, draw dominoes from domino deque until a match  
		// is found or domino deque is empty. If no such domino is found, returns -1.
		int findPlayable(int head, int tail) {
			for (unsigned int pieceID = 0; pieceID < playerHand.size(); pieceID++) {
				int left = playerHand[pieceID].left;
				int right = playerHand[pieceID].right;
				if (left == head || left == tail || right == head || right == tail) return pieceID;
			}
			while (dominoOBJ->dominoes.size() > 0) {
				drawPieces(1);
				int left = playerHand[playerHand.size() - 1].left;
				int right = playerHand[playerHand.size() - 1].right;
				if (left == head || left == tail || right == head || right == tail) return playerHand.size() - 1;
			}
			return -1;
		}
};

class CTable{
	public:
		CTable(){}  // constructor
		~CTable(){} // destructor

		int head = -1, tail = -1;
		string board;
		class CPlayer *playerOBJ = NULL;

		void API(CPlayer *receive_playersOBJ) {
			playerOBJ = receive_playersOBJ;
		}
		void showPlayerHands() {
			domino_piece showpiece;
			for(int playerID = 0; playerID < TOTAL_PLAYERS; playerID++){
				cout << "Player " << playerID + 1 << " has " <<
						playerOBJ[playerID].playerHand.size() << " pieces.\n";
				for(unsigned int pieceID = 0; pieceID < playerOBJ[playerID].playerHand.size(); pieceID++){
					showpiece = playerOBJ[playerID].playerHand.at(pieceID);
					cout << "[" << showpiece.left << "|" << showpiece.right << "]" << endl;
				}
			}
		}
		void playFirstPiece(domino_piece firstPiece, int playerID) {
			cout << "\nPlayer " << playerID + 1 << " gets to go first\n";
			head = firstPiece.left;
			tail = firstPiece.right;
			board.append("[");
			board.append(to_string(head));
			board.append("|");
			board.append(to_string(tail));
			board.append("]");
		}
		// Call findPlayable from CPlayer with the current head and tail values
		int findPlayable(int playerID) {
			return playerOBJ[playerID].findPlayable(head, tail);
		}
		// Update board and head or tail as required
		void playPiece(domino_piece nextPiece, int playerTurn) {
			cout << "\nPlayer " << playerTurn + 1 << " placed a piece\n";
			string l = to_string(nextPiece.left);
			string r =
				to_string(nextPiece.right);
			if (nextPiece.left == head) {
				head = nextPiece.right;
				board.insert(0, "]");
				board.insert(0, l);
				board.insert(0, "|");
				board.insert(0, r);
				board.insert(0, "[");
			} else if (nextPiece.left == tail) {
				tail = nextPiece.right;
				board.append("[");
				board.append(l);
				board.append("|");
				board.append(r);
				board.append("]");
			} else if (nextPiece.right == head) {
				head = nextPiece.left;
				board.insert(0, "]");
				board.insert(0, r);
				board.insert(0, "|");
				board.insert(0, l);
				board.insert(0, "[");
			} else { //nextPiece.right == tail 
				tail = nextPiece.left;
				board.append("[");
				board.append(r);
				board.append("|");
				board.append(l);
				board.append("]");
			}
		}
		void showBoard() {
			for (unsigned int i = 0; i < board.size(); i++) {
				cout << board.at(i);
			}
			cout << "\n\n";
		}
		// Display pieces left in each player's hands and the winner, if one exists.
		void showWinner(int playerTurn) {
			int winnerID = -1;
			if (playerOBJ[playerTurn].playerHand.size() == 0) winnerID = playerTurn;
			showPlayerHands();
			if (winnerID == -1) cout << "Game is a draw.\n";
			else cout << "Player " << winnerID + 1 << " wins!\n";
		}
};

int main(void){

	// Seed random generator
	srand(time(NULL));
	
	// initialize objects
    CDomino dominoOBJ,*dominoPointer;
    dominoOBJ.API();
    dominoPointer=&dominoOBJ;

    CPlayer *playerOBJ;
    playerOBJ = new CPlayer[TOTAL_PLAYERS];
	for (int i = 0; i < TOTAL_PLAYERS; i++) {
		playerOBJ[i].API(dominoPointer);
		playerOBJ[i].drawPieces(HAND_SIZE);
	}

    CTable tableOBJ;
    tableOBJ.API(playerOBJ);
	tableOBJ.showPlayerHands();

	// initialize game
	int playersCantPlay = 0;
	int playerTurn = playerOBJ[0].getRandomPublic(0, TOTAL_PLAYERS - 1);
	int nextPieceID = playerOBJ[playerTurn].getRandomPublic(0, playerOBJ[playerTurn].playerHand.size() - 1);
	domino_piece nextPiece = playerOBJ[playerTurn].getPiece(nextPieceID);
	bool gameRunning = true, noMoreDominoes = false;

	tableOBJ.playFirstPiece(nextPiece, playerTurn);
	tableOBJ.showBoard();
	system("pause");
	while (gameRunning) {
		if (noMoreDominoes) {
			playersCantPlay++;
			if (playersCantPlay >= TOTAL_PLAYERS) break; // no players can play, game is a draw
		}
		playerTurn++;
		if (playerTurn > TOTAL_PLAYERS - 1) playerTurn = 0;
		nextPieceID = tableOBJ.findPlayable(playerTurn);
		if (nextPieceID == -1) { // player doesn't have playable piece
			cout << "Player " << playerTurn + 1 << " can't play a piece!\n";
			noMoreDominoes = true;
			continue;
		}
		nextPiece = playerOBJ[playerTurn].getPiece(nextPieceID);
		tableOBJ.playPiece(nextPiece, playerTurn);
		tableOBJ.showBoard();
		if (noMoreDominoes) playersCantPlay--;
		gameRunning = (playerOBJ[playerTurn].playerHand.size() > 0);
		system("pause");
	}
	tableOBJ.showWinner(playerTurn);
	system("pause");
    delete []playerOBJ;
    return EXIT_SUCCESS;
}