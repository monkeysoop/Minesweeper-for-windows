#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

void generateMines(int* pmi, int WIDTH, int HEIGHT, int db);
int isMine(const int* pmi, int h, int w, int WIDTH);
int numOfNeighbours(const int* pcH, int h, int w, int WIDTH);
int notBorder(int h, int w, int WIDTH, int HEIGHT);
int notFlagg(const int* pma, int h, int w, int WIDTH);
int notExposed(const int* pma, int h, int w, int WIDTH);
void addToNeigbours(int* pcH, int h, int w, int WIDTH, int value);
int countNeigbourMines(const int* pmi, int h, int w, int WIDTH, int HEIGHT);
void countMines(const int* pmi, int* pcD, int WIDTH, int HEIGHT);
int getStartingPos(const int* pmi, const int* pcD, int* ph, int* pw, int WIDTH, int HEIGHT);
void exposeNeigbours(int* pma0, int h, int w, int WIDTH, int HEIGHT);
int expandable(const int* pcH, const int* pma, int h, int w, int WIDTH, int HEIGHT);
int expand(const int* pmi, int* pcH, int* pma, int h, int w, int WIDTH, int HEIGHT);
void matrixCopy(const int* pfrom, int* pto, int WIDTH, int HEIGHT);
void setBorder(int* pcD, int WIDTH, int HEIGHT);

#endif
