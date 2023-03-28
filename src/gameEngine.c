#include <stdio.h>
#include <time.h> // for random numbers
#include <stdlib.h>
#include "new_gameEngine.h"





int isMine(int** pmi, int h, int w) {
    return pmi[h][w];
}

int numOfNeighbours(int** pcH, int h, int w) {
    return pcH[h][w];
}

int notBorder(int h, int w, int WIDTH, int HEIGHT) {
    if (h > 0 && h < (HEIGHT-1) && w > 0 && w < (WIDTH-1)) {
        return 1;
    }
    return 0;
}

int notFlagg(int** pma, int h, int w) {
    if (pma[h][w] > -1) {
        return 1;
    }
    return 0;
}

int notExposed(int** pma, int h, int w) {
    if (pma[h][w] == 0) {
        return 1;
    }
    return 0;
}

void matrixCopy(int** pfrom, int** pto, int WIDTH, int HEIGHT) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            pto[i][j] = pfrom[i][j];
        }
    }
}

void generateMines(int** pmi, int WIDTH, int HEIGHT, int db) {
    int db0 = db;
    while (db0) {
        int randX = rand()%(WIDTH - 2) + 1;
        int randY = rand()%(HEIGHT - 2) + 1;
        if (!pmi[randY][randX]) {
            pmi[randY][randX] = 1;
            db0--;
        }
    }
}

int countNeigbourMines(int** pmi, int h, int w, int WIDTH, int HEIGHT) {
    int sum = 0;
    for (int th = 1; th >= -1; th--) {
        for (int tw = 1; tw >= -1; tw--) {
            if (th != 0 || tw != 0) {

                if (notBorder((h + th), (w + tw), WIDTH, HEIGHT) && 
                    isMine(pmi, (h + th), (w + tw))) {
                    sum++;
                }
            }
        }
    }
    return sum;
}

void countMines(int** pmi, int** pcD, int WIDTH, int HEIGHT) {
    for (int i = 1; i < (HEIGHT-1); i++) {
        for (int j = 1; j < (WIDTH-1); j++) {
            if (!isMine(pmi, i, j)) {
                pcD[i][j] = countNeigbourMines(pmi, i, j, WIDTH, HEIGHT); 
            }
        }
    }
}

int getStartingPos(int** pmi, int** pcD, int* ph, int* pw, int WIDTH, int HEIGHT) {
    for (int i = 2; i < (HEIGHT-2); i++) {
        for (int j = 2; j < (WIDTH-2); j++) {
            if (!pmi[i][j] && !pcD[i][j]) {
                *ph = i;
                *pw = j;
                return 0;
            }
        }
    }
    return 1;
}

void addToNeigbours(int** pmi, int** pcH, int h, int w, int value) {
    for (int th = 1; th >= -1; th--) {
        for (int tw = 1; tw >= -1; tw--) {
            if (th != 0 || tw != 0) {
                if (!isMine(pmi, (h + th), (w + tw))) {
                    pcH[(h + th)][(w + tw)] += value;
                }
            }
        }
    }
}

void exposeNeigbours(int* emptySpaces, int** pma, int h, int w, int WIDTH, int HEIGHT) {
    for (int th = 1; th >= -1; th--) {
        for (int tw = 1; tw >= -1; tw--) {
            if (th != 0 || tw != 0) {
                if (notBorder((h + th), (w + tw), WIDTH, HEIGHT) && 
                    notExposed(pma, (h + th), (w + tw))) {
                    pma[(h + th)][(w + tw)] = 1;
                    *(emptySpaces) -= 1;
                }
            }
        }
    }
}

int expandable(int** pcH, int** pma, int h, int w, int WIDTH, int HEIGHT) {
    if (notFlagg(pma, h, w) && numOfNeighbours(pcH, h, w) == 0 && notBorder(h, w, WIDTH, HEIGHT)) {
        return 1;
    }
    return 0;
}

int expand(int* emptySpaces, int** pmi, int** pcH, int** pma, int h, int w, int WIDTH, int HEIGHT) {
    if (isMine(pmi, h, w)) {
        return 1;
    } else {
        exposeNeigbours(emptySpaces, pma, h, w, WIDTH, HEIGHT);
        pcH[h][w] = -1;    // this is for preventing the recursion backtracking

        for (int th = 1; th >= -1; th--) {
            for (int tw = 1; tw >= -1; tw--) {
                if (th != 0 || tw != 0) {
                    if (expandable(pcH, pma, (h + th), (w + tw), WIDTH, HEIGHT) &&
                        expand(emptySpaces, pmi, pcH, pma,(h + th), (w + tw), WIDTH, HEIGHT)) {
                        return 1;
                    }
                }
            }
        }

        
    }
}
