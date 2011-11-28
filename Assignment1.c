// CSC444H1F Solution Code for Assignment 1 (with no input validation).
//  By Prof. Penny, October 29, 2006
//  ~70 lines of code, ~150 minutes to code, 2 defects
//    Defect 1: used a tiles[1][1] declaration and then was surprised when stride was 1, not N!

#include "stdafx.h" // standard pre-compiled headers

int N;                  // board dimension
int BoardBytes;         // Bytes to allocate for each board structure (below)

// Need to keep track of all boards in the current depth-first search
// in order to both
//  1. remember the sequence of moves
//  2. ensure we are not looping
const int MaxDepth = 30;    // Would take over 1 day to solve depth 30
struct board {
    int blankRow;   // 0..N-1
    int blankCol;   // 0..N-1
    int move;       // Index into "moveDirections" for next move for this board
    int tiles[1];   // indexed as tiles[row*N+col] - will allocate sufficient storage for NxN
} *boards[MaxDepth+1];


// Static array of possible move directions for the blank square
const int NumMoveDirections = 4;
struct MoveDirection {
    int row;            // -1,0,+1
    int col;            // -1,0,+1
    const char *name;   // user-readable name of this move
} moveDirections[NumMoveDirections] =
{ {-1,0,"up"}, {+1,0,"down"}, {0,-1,"left"}, {0,+1,"right"} };


// Apply the move in direction "md" to the board "b" if possible and return true.
// If not possible (because at the edge of the board), return false.
bool move(struct board* b, int md) {
    int blankRow = b->blankRow + moveDirections[md].row;
    if( blankRow < 0 || blankRow >= N ) return false;
    int blankCol = b->blankCol + moveDirections[md].col;
    if( blankCol < 0 || blankCol >= N ) return false;

    b->tiles[b->blankRow*N+b->blankCol] = b->tiles[blankRow*N+blankCol];
    b->blankCol = blankCol;
    b->blankRow = blankRow;
    b->tiles[blankRow*N+blankCol] = 0; // nice for printing boards, not otherwise required
    return true;
}


// Return if the board "b" is in the correct final position or not.
bool isSolved(struct board *b) {
    for(int i=0; i < N*N-1; i++) if( b->tiles[i] != i+1 ) return false;
    return true;
}


// Return if board "b1" is the same configuration as board "b2" or not.
bool isSameBoard(struct board *b1, struct board *b2) {
    for(int i=0; i < N*N; i++) if( b1->tiles[i] != b2->tiles[i] ) return false;
    return true;
}


// Attempt to solve the board at "board[curlevel]".
// Do not recurse deeper than "maxlevel" in total.
// Return if successful at solving the board within maxlevel moves.
bool solve(int maxlevel, int curlevel) {
    // alias for current board
    struct board *curb = boards[curlevel];

    // Return solved if this board represents a solution
    if( isSolved(curb) ) return true;

    // Return with no solution if at maximum depth
    if(curlevel == maxlevel) return false;

    // Return with no solution if seen this board before on this descent (a move loop)
    for(int n=curlevel-1; n>=0; n--) if( isSameBoard(curb,boards[n]) ) return false;

    // Iterate through all possible moves
    struct board *nextb = boards[curlevel+1];
    for(curb->move=0;  curb->move < NumMoveDirections; curb->move++) {
        // make a copy of the current board into the next
        memcpy(nextb,curb,BoardBytes);

        // if we can move in this direction and the solve the board, it is solved
        if( move(nextb,curb->move) && solve(maxlevel,curlevel+1) ) return true;
    }

    // Could not solve after all legal moves - no solution from the current board.
    return false;
}


int _tmain(int argc, _TCHAR* argv[])
{
    // Read size of board into "N".
    scanf("%d",&N);

    // Allocate board "stack" storage.
    BoardBytes = sizeof(struct board) + sizeof(int)*(N*N-1);
    for(int n=0; n<=MaxDepth; n++) boards[n] = (struct board*)malloc(BoardBytes);

    // Input the board into "boards[0]".
    for(int row=0; row<N; row++)
        for(int col=0; col<N; col++) {
            scanf("%d",&(boards[0]->tiles[row*N+col]));
            if( boards[0]->tiles[row*N+col] == 0 ) {
                // this is the blank space
                boards[0]->blankCol = col;
                boards[0]->blankRow = row;/////////
            }
        }

    // Depeen the search one move at a time
    for(int maxlevel = 0; maxlevel <= MaxDepth; maxlevel++) {
        // Can we solve within "maxlevel" moves?
        if( solve(maxlevel,0) ) {
            // If yes, print out the move sequence that solves it
            printf("Solved in %d moves\n", maxlevel);
            for(int n=0; n<maxlevel; n++) printf("%s\n",moveDirections[boards[n]->move].name);
            return 0;
        }
    }

    // Could not find a solution
    printf("No solution found in %d moves.\n", MaxDepth);
    return 1;
}
