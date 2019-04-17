#include <time.h>
#include <iostream>
#include <deque>
using namespace std;

const int TOTAL_PLAYERS = 2;
const int HAND_SIZE = 10;

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
		domino_piece getPiece(int pieceID){
			domino_piece mypiece=dominoes.at(pieceID);
			dominoes.erase(dominoes.begin() + pieceID);
			return(mypiece);
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
		void takePiece(int pieceNo){
			domino_piece takenPiece;
			takenPiece=dominoOBJ->getPiece(pieceNo);
			playerHand.push_back(takenPiece);
		}
		void drawHand() {
			for (int playerID = 0; playerID < TOTAL_PLAYERS; playerID++) {
				for (int i = 0; i < HAND_SIZE; i++) {
					int pieceNO = getRandomPublic(0, dominoOBJ[0].dominoes.size());
					takePiece(pieceNO);
				}
			}
		}
};

class CTable{
	public:
		CTable(){}  // constructor
		~CTable(){} // destructor

		class CPlayer *playerOBJ = NULL;

		void API(CPlayer *receive_playersOBJ) {
			playerOBJ = receive_playersOBJ;
		}
		void showPlayerHand(void){
			domino_piece showpiece;
			for(int playerID = 0; playerID < TOTAL_PLAYERS; playerID++){
				cout << "Player " << playerID + 1 << " has " <<
						playerOBJ[playerID].playerHand.size() << " pieces.\n";
				for(unsigned int pieceNo = 0; pieceNo < playerOBJ[playerID].playerHand.size(); pieceNo++){
					showpiece = playerOBJ[playerID].playerHand.at(pieceNo);
					cout << "[" << showpiece.left << "|" << showpiece.right << "]" << endl;
				}
			}
		}
};

int main(void){

	srand(time(NULL));
	
    CDomino dominoOBJ,*dominoPointer;
    dominoOBJ.API();
    dominoPointer=&dominoOBJ;

    CPlayer *playerOBJ;
    playerOBJ = new CPlayer[TOTAL_PLAYERS];
	for (int i = 0; i < TOTAL_PLAYERS; i++) {
		playerOBJ[i].API(dominoPointer);
	}

    CTable tableObj;
    tableObj.API(playerOBJ);

	int playerTurn = playerOBJ[0].getRandomPublic(0, TOTAL_PLAYERS - 1);
	int firstPiece = playerOBJ[playerTurn].getRandomPublic(0, playerOBJ[playerTurn].playerHand.size());

	system("pause");
    delete []playerOBJ;
    return EXIT_SUCCESS;
}