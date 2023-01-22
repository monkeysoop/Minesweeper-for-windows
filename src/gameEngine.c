#include <stdio.h>
#include <time.h> // for random numbers
#include <stdlib.h>
#include "gameEngine.h"




void generateMines(int* pmi, int WIDTH, int HEIGHT, int db) {
    int db0 = db;
    while (db0) {
        int randX = rand()%(WIDTH - 2) + 1;
        int randY = rand()%(HEIGHT - 2) + 1;
        if (!*(pmi + randY * WIDTH + randX)) {
            *(pmi + randY * WIDTH + randX) = 1;
            db0--;
        }
    }
}

int isMine(const int* pmi, int h, int w, int WIDTH) {
    return *(pmi + h * WIDTH + w);
}

int numOfNeighbours(const int* pcH, int h, int w, int WIDTH) {
    return *(pcH + h * WIDTH + w);
}

int notBorder(int h, int w, int WIDTH, int HEIGHT) {
    if (h > 0 && h < (HEIGHT-1) && w > 0 && w < (WIDTH-1)) {
        return 1;
    } else {
        return 0;
    }

}

int notFlagg(const int* pma, int h, int w, int WIDTH) {
    if (*(pma + h * WIDTH + w) > -1) {
        return 1;
    } else {
        return 0;
    }
}

int notExposed(const int* pma, int h, int w, int WIDTH) {
    if (*(pma + h * WIDTH + w) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void addToNeigbours(int* pcH, int h, int w, int WIDTH, int value) {
    if (*(pcH + (h+1) * WIDTH + (w+1)) > 0) {
        *(pcH + (h+1) * WIDTH + (w+1)) += value;
    }
    if (*(pcH + (h+1) * WIDTH + (w)) > 0) {
        *(pcH + (h+1) * WIDTH + (w)) += value;
    }
    if (*(pcH + (h+1) * WIDTH + (w-1)) > 0) {
        *(pcH + (h+1) * WIDTH + (w-1)) += value;
    }
    if (*(pcH + (h) * WIDTH + (w+1)) > 0) {
        *(pcH + (h) * WIDTH + (w+1)) += value;
    }
    if (*(pcH + (h) * WIDTH + (w-1)) > 0) {
        *(pcH + (h) * WIDTH + (w-1)) += value;
    }
    if (*(pcH + (h-1) * WIDTH + (w+1)) > 0) {
        *(pcH + (h-1) * WIDTH + (w+1)) += value;
    }
    if (*(pcH + (h-1) * WIDTH + (w)) > 0) {
        *(pcH + (h-1) * WIDTH + (w)) += value;
    }
    if (*(pcH + (h-1) * WIDTH + (w-1)) > 0) {
        *(pcH + (h-1) * WIDTH + (w-1)) += value;
    }
}

int countNeigbourMines(const int* pmi, int h, int w, int WIDTH, int HEIGHT) {
    int sum = 0;
    if (notBorder((h+1), (w+1), WIDTH, HEIGHT) && *(pmi + (h+1) * WIDTH + (w+1))) sum++;
    if (notBorder((h+1), (w), WIDTH, HEIGHT) && *(pmi + (h+1) * WIDTH + (w)))     sum++;
    if (notBorder((h+1), (w-1), WIDTH, HEIGHT) && *(pmi + (h+1) * WIDTH + (w-1))) sum++;
    if (notBorder((h), (w+1), WIDTH, HEIGHT) && *(pmi + (h) * WIDTH + (w+1)))     sum++;
    if (notBorder((h), (w), WIDTH, HEIGHT) && *(pmi + (h) * WIDTH + (w)))         sum++;
    if (notBorder((h), (w-1), WIDTH, HEIGHT) && *(pmi + (h) * WIDTH + (w-1)))     sum++;
    if (notBorder((h-1), (w+1), WIDTH, HEIGHT) && *(pmi + (h-1) * WIDTH + (w+1))) sum++;
    if (notBorder((h-1), (w), WIDTH, HEIGHT) && *(pmi + (h-1) * WIDTH + (w)))     sum++;
    if (notBorder((h-1), (w-1), WIDTH, HEIGHT) && *(pmi + (h-1) * WIDTH + (w-1))) sum++;
    return sum;

}

void countMines(const int* pmi, int* pcD, int WIDTH, int HEIGHT) {
    for (int i = 1; i < (HEIGHT-1); i++) {
        for (int j = 1; j < (WIDTH-1); j++) {
            if (!*(pmi + i * WIDTH + j)) {
                *(pcD + i * WIDTH + j) = countNeigbourMines(pmi, i, j, WIDTH, HEIGHT); 
            }
        }
    }
}

int getStartingPos(const int* pmi, const int* pcD, int* ph, int* pw, int WIDTH, int HEIGHT) {
    for (int i = 2; i < (HEIGHT-2); i++) {
        for (int j = 2; j < (WIDTH-2); j++) {
            if (!*(pmi + i * WIDTH + j) && !*(pcD + i * WIDTH + j)) {
                *ph = i;
                *pw = j;
                return 0;
            }
        }
    }
    return 1;
}

void exposeNeigbours(int* emptySpaces, int* pma0, int h, int w, int WIDTH, int HEIGHT) {
    if (notBorder((h+1), (w+1), WIDTH, HEIGHT) && notExposed(pma0, (h+1), (w+1), WIDTH)) {
        *(pma0 + (h+1) * WIDTH + (w+1)) = 1;
        *(emptySpaces) -= 1;
    }
    if (notBorder((h+1), (w), WIDTH, HEIGHT) && notExposed(pma0, (h+1), (w), WIDTH)) {
        *(pma0 + (h+1) * WIDTH + (w)) = 1;
        *(emptySpaces) -= 1;
    }
    if (notBorder((h+1), (w-1), WIDTH, HEIGHT) && notExposed(pma0, (h+1), (w-1), WIDTH)) {
        *(pma0 + (h+1) * WIDTH + (w-1)) = 1;
        *(emptySpaces) -= 1;
    }
    if (notBorder((h), (w+1), WIDTH, HEIGHT) && notExposed(pma0, (h), (w+1), WIDTH)) {
        *(pma0 + (h) * WIDTH + (w+1)) = 1;
        *(emptySpaces) -= 1;
    }
    if (notBorder((h), (w), WIDTH, HEIGHT) && notExposed(pma0, (h), (w), WIDTH)) {
        *(pma0 + (h) * WIDTH + (w)) = 1;
        *(emptySpaces) -= 1;
    }
    if (notBorder((h), (w-1), WIDTH, HEIGHT) && notExposed(pma0, (h), (w-1), WIDTH)) {
        *(pma0 + (h) * WIDTH + (w-1)) = 1;
        *(emptySpaces) -= 1;
    }
    if (notBorder((h-1), (w+1), WIDTH, HEIGHT) && notExposed(pma0, (h-1), (w+1), WIDTH)) {
        *(pma0 + (h-1) * WIDTH + (w+1)) = 1;
        *(emptySpaces) -= 1;
    }
    if (notBorder((h-1), (w), WIDTH, HEIGHT) && notExposed(pma0, (h-1), (w), WIDTH)) {
        *(pma0 + (h-1) * WIDTH + (w)) = 1;
        *(emptySpaces) -= 1;
    }
    if (notBorder((h-1), (w-1), WIDTH, HEIGHT) && notExposed(pma0, (h-1), (w-1), WIDTH)) {
        *(pma0 + (h-1) * WIDTH + (w-1)) = 1;
        *(emptySpaces) -= 1;
    }
}

int expandable(const int* pcH, const int* pma, int h, int w, int WIDTH, int HEIGHT) {
    if (notFlagg(pma, h, w, WIDTH) && numOfNeighbours(pcH, h, w, WIDTH) == 0 && notBorder(h, w, WIDTH, HEIGHT)) {
        return 1;
    }
    return 0;
}

int expand(int* emptySpaces, const int* pmi, int* pcH, int* pma, int h, int w, int WIDTH, int HEIGHT) {
    if (isMine(pmi, h, w, WIDTH)) {
        return 1;
    } else {
        exposeNeigbours(emptySpaces, pma, h, w, WIDTH, HEIGHT);
        *(pcH + h * WIDTH + w) = -1;

        if (expandable(pcH, pma, (h-1), (w-1), WIDTH, HEIGHT)) {
            if (expand(emptySpaces, pmi, pcH, pma, (h-1), (w-1), WIDTH, HEIGHT)) {
                return 1;
            }
        }
        if (expandable(pcH, pma, (h-1), (w), WIDTH, HEIGHT)) {
            if (expand(emptySpaces, pmi, pcH, pma, (h-1), (w), WIDTH, HEIGHT)) {
                return 1;
            }
        }
        if (expandable(pcH, pma, (h-1), (w+1), WIDTH, HEIGHT)) {
            if (expand(emptySpaces, pmi, pcH, pma, (h-1), (w+1), WIDTH, HEIGHT)) {
                return 1;
            }
        }
        if (expandable(pcH, pma, (h), (w-1), WIDTH, HEIGHT)) {
            if (expand(emptySpaces, pmi, pcH, pma, (h), (w-1), WIDTH, HEIGHT)) {
                return 1;
            }
        }
        if (expandable(pcH, pma, (h), (w+1), WIDTH, HEIGHT)) {
            if (expand(emptySpaces, pmi, pcH, pma, (h), (w+1), WIDTH, HEIGHT)) {
                return 1;
            }
        }
        if (expandable(pcH, pma, (h+1), (w-1), WIDTH, HEIGHT)) {
            if (expand(emptySpaces, pmi, pcH, pma, (h+1), (w-1), WIDTH, HEIGHT)) {
                return 1;
            }
        }
        if (expandable(pcH, pma, (h+1), (w), WIDTH, HEIGHT)) {
            if (expand(emptySpaces, pmi, pcH, pma, (h+1), (w), WIDTH, HEIGHT)) {
                return 1;
            }
        }
        if (expandable(pcH, pma, (h+1), (w+1), WIDTH, HEIGHT)) {
            if (expand(emptySpaces, pmi, pcH, pma, (h+1), (w+1), WIDTH, HEIGHT)) {
                return 1;
            }
        }
    }
}

void matrixCopy(const int* pfrom, int* pto, int WIDTH, int HEIGHT) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            *(pto + i * WIDTH + j) = *(pfrom + i * WIDTH + j);
        }
    }

}

void setBorder(int* pcD, int WIDTH, int HEIGHT) {
    for (int i = 0; i < WIDTH; i++) {
        *(pcD + i) = -1;
        *(pcD + (HEIGHT-1) * WIDTH + i) = -1;
    }
    for (int i = 1; i < (HEIGHT-1); i++) {
        *(pcD + i * WIDTH) = -1;
        *(pcD + (i+1) * WIDTH - 1) = -1;
    }
}
