#include "board.h"

int main(){
    Board board;
    board.setStartingPosition();
    board.movePiece(8,16);
    board.print();
    return 0;
}
