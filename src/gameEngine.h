#ifndef NEW_GAME_ENGINE_H
#define NEW_GAME_ENGINE_H

int isMine(int** pmi, int h, int w);
int numOfNeighbours(int** pcH, int h, int w);
int notBorder(int h, int w, int WIDTH, int HEIGHT);
int notFlagg(int** pma, int h, int w);
int notExposed(int** pma, int h, int w);
void matrixCopy(int** pfrom, int** pto, int WIDTH, int HEIGHT);
void generateMines(int** pmi, int WIDTH, int HEIGHT, int db);
int countNeigbourMines(int** pmi, int h, int w, int WIDTH, int HEIGHT);
void countMines(int** pmi, int** pcD, int WIDTH, int HEIGHT);
int getStartingPos(int** pmi, int** pcD, int* ph, int* pw, int WIDTH, int HEIGHT);
void addToNeigbours(int** pmi, int** pcH, int h, int w, int value);
void exposeNeigbours(int* emptySpaces, int** pma, int h, int w, int WIDTH, int HEIGHT);
int expandable(int** pcH, int** pma, int h, int w, int WIDTH, int HEIGHT);
int expand(int* emptySpaces, int** pmi, int** pcH, int** pma, int h, int w, int WIDTH, int HEIGHT);

#endif
