#include <stdio.h>
#include <windows.h>
#include <shellapi.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <shellapi.h>
#include "readBMP.h"
#include "gameEngine.h"



#define FPS 60   // This is just a limit
#define PIXELS_PER_TILE 15   // Should be bigger than the width and the height of all source images
#define SOURCE_IMAGES 22   // number of them



void* bitmapMemory;



struct Image {
    int* data;
    int w;
    int h;
};



LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_KEYDOWN: {
            switch (wparam) {
                case 'Q': DestroyWindow(window); break;
            }
        } break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default: {
            return DefWindowProc(window, msg, wparam, lparam);
        }
    }
    return 0;
}


void printUsage() {
    wprintf(L"\nUsade:\n\t.\\a.exe\n\t\t=> Width: 16\n\t\t=> Height: 30\n\t\t=> Bomb db: 99\n\n");
    wprintf(L"Or:\n\t.\\a.exe [Width, min 4] [Height, min 4]\n\t\t=> Width: User input\n\t\t=> Height: User input\n\t\t=> Bombs: ~ 20%%\n\n");
    wprintf(L"Or:\n\t.\\a.exe [Width, min 4] [Height, min 4] [Bomb percentile, max 90%%]\n\t\t=> Width: User input\n\t\t=> Height: User input\n\t\t=> Bombs: User input in %%\n\n");
        
}



void DrawImage(int X, int Y, const struct Image* image, int PIXELS_W) {
    uint32_t* pixel = (uint32_t*)bitmapMemory;
    int w0 = image->w; 
    int h0 = image->h;
    if (w0 > PIXELS_PER_TILE) {
        w0 = PIXELS_PER_TILE;
    }
    if (h0 > PIXELS_PER_TILE) {
        h0 = PIXELS_PER_TILE;
    }
    for (int row = Y; row < (Y + h0); row++) {
        for (int colum = X; colum < (X + w0); colum++) {
            pixel += row * PIXELS_W + colum;
            *pixel = *(image->data + (row - Y) * w0 + (colum - X));
            pixel -= row * PIXELS_W + colum;
        }
    }
}



void DrawMapLost(const struct Image* images, const int* pmi, const int* pcD, const int* pma, int H, int W, int WIDTH, int HEIGHT, int PIXELS_W) {
    for (int i = 1; i < (HEIGHT-1); i++) {
        for (int j = 1; j < (WIDTH-1); j++) {
            int mi = *(pmi + i * WIDTH + j);
            int pc = *(pcD + i * WIDTH + j);
            int ma = *(pma + i * WIDTH + j);
            int y = (i - 1) * PIXELS_PER_TILE;
            int x = (j - 1) * PIXELS_PER_TILE;
            if (i == H && j == W) {
                if (mi == 1) {
                    DrawImage(x, y, (images + 21), PIXELS_W);;   // exploded bomb
                } else if (pc > 0) {
                    DrawImage(x, y, (images + pc + 8), PIXELS_W);   // exploded numbers
                }
            } else if (mi == 1) {
                if (ma == -1) {
                    DrawImage(x, y, (images + 17), PIXELS_W);;   // flagg
                } else {
                    DrawImage(x, y, (images + 20), PIXELS_W);;   // bomb
                }
            } else if (ma == -2) {
                DrawImage(x, y, (images + 18), PIXELS_W);;   // incorrect flagg
            } else {
                DrawImage(x, y, (images + pc), PIXELS_W);   // numbers
            }
        }
    }
}



void DrawMap(const struct Image* images, const int* pmi, const int* pcD, const int* pma, int WIDTH, int HEIGHT, int PIXELS_W) {
    for (int i = 1; i < (HEIGHT-1); i++) {
        for (int j = 1; j < (WIDTH-1); j++) {
            int mi = *(pmi + i * WIDTH + j);
            int pc = *(pcD + i * WIDTH + j);
            int ma = *(pma + i * WIDTH + j);
            int y = (i - 1) * PIXELS_PER_TILE;
            int x = (j - 1) * PIXELS_PER_TILE;
            if (ma == -2 || ma == -1) {
                DrawImage(x, y, (images + 17), PIXELS_W);   // flagg
            } else if (ma == 1) {
                DrawImage(x, y, (images + pc), PIXELS_W);   // numbers + empty
            } else {
                DrawImage(x, y, (images + 19), PIXELS_W);   // unknown
            }   
        }
    }
}



void LeftClick(const struct Image* images, int* alive, int* counter, int* emptySpaces, const int* pmi, int* pcD, int* pcH, int* pma, int H, int W, int WIDTH, int HEIGHT, int PIXELS_W) {
    if (notFlagg(pma, H, W, WIDTH)) {
        if (isMine(pmi, H, W, WIDTH)) {
            wprintf(L"You Died!\n");
            *alive = 0;
            *counter = FPS * 5;   // 5 seconds before closing
            DrawMapLost(images, pmi, pcD, pma, H, W, WIDTH, HEIGHT, PIXELS_W);
        } else {
            if (numOfNeighbours(pcH, H, W, WIDTH) > 0) {
                *(pma + H * WIDTH + W) = 1;
                *(emptySpaces) -= 1;
                DrawMap(images, pmi, pcD, pma, WIDTH, HEIGHT, PIXELS_W);
            } else {
                if (expand(emptySpaces, pmi, pcH, pma, H, W, WIDTH, HEIGHT)) {
                    wprintf(L"You Died!\n");
                    *alive = 0;
                    *counter = FPS * 5;   // 5 seconds before closing
                    DrawMapLost(images, pmi, pcD, pma, H, W, WIDTH, HEIGHT, PIXELS_W);
                } else {
                    DrawMap(images, pmi, pcD, pma, WIDTH, HEIGHT, PIXELS_W);
                }
            }
        }
    }
}



void RightClick(const struct Image* images, int* emptySpaces, int* flagCount, const int* pmi, const int* pcD, int* pcH, int* pma, int H, int W, int WIDTH, int HEIGHT, int PIXELS_W) {
    if (isMine(pmi, H, W, WIDTH)) {
        if (notFlagg(pma, H, W, WIDTH)) {
            if (notExposed(pma, H, W, WIDTH) && *(flagCount)) {
                *(flagCount) -= 1;
                *(pma + H * WIDTH + W) = -1;
                *(emptySpaces) -= 1;
                addToNeigbours(pcH, H, W, WIDTH, -1);
                DrawMap(images, pmi, pcD, pma, WIDTH, HEIGHT, PIXELS_W);
            } else {
                wprintf(L"Illegal position or out of flags!\n");
            }
        } else {
            *(flagCount) += 1;
            *(pma + H * WIDTH + W) = 0;
            *(emptySpaces) += 1;
            addToNeigbours(pcH, H, W, WIDTH, 1);
            DrawMap(images, pmi, pcD, pma, WIDTH, HEIGHT, PIXELS_W);
        }
    } else {
        if (notFlagg(pma, H, W, WIDTH)) {
            if (notExposed(pma, H, W, WIDTH) && *(flagCount)) {
                *(flagCount) -= 1;
                *(pma + H * WIDTH + W) = -2;
                *(emptySpaces) -= 1;
                addToNeigbours(pcH, H, W, WIDTH, -1);
                DrawMap(images, pmi, pcD, pma, WIDTH, HEIGHT, PIXELS_W);
            } else {
                wprintf(L"Illegal position or out of flags!\n");
            }
        } else {
            *(flagCount) += 1;
            *(pma + H * WIDTH + W) = 0;
            *(emptySpaces) += 1;
            addToNeigbours(pcH, H, W, WIDTH, 1);
            DrawMap(images, pmi, pcD, pma, WIDTH, HEIGHT, PIXELS_W);
        }
    }
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow) {
    float targetMsPerFrame = 1 / (float)FPS;
    int tilesW = 16;   // Visible colums
    int tilesH = 30;   // Visible rows
    int bombDb = 99;



    WNDCLASS wc = {0};
    const char className[] = "MyWindowClass";
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if(!RegisterClass(&wc)) {
        MessageBox(0, "RegisterClass failed!", 0, 0);
        return (int)GetLastError();
    }



    // Reading parameters
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc == 2) {
        printUsage();
        return 1;
    } else if (argc == 3) {
        tilesW = _wtoi(argv[1]);
        tilesH = _wtoi(argv[2]);
        if (tilesW < 4 || tilesH < 4) {
            printUsage();
            return 1;
        }
        bombDb = tilesW * tilesH / 5; // ~20% bomb
    } else if (argc >= 4) {
        tilesW = _wtoi(argv[1]);
        tilesH = _wtoi(argv[2]);
        bombDb = _wtoi(argv[3]);
        if (tilesW < 4 || tilesH < 4 || bombDb <= 0 || bombDb > 90) {
            printUsage();
            return 1;
        }
        bombDb = bombDb * tilesW * tilesH / 100;
    }
    const int WIDTH = tilesW + 2;   // +2 is for adding a broder, which isnt displayed
    const int HEIGHT = tilesH + 2;
    const int PIXELS_W = tilesW * PIXELS_PER_TILE;   // size in pixels
    const int PIXELS_H = tilesH * PIXELS_PER_TILE;
    const int db = bombDb;



    // Calculates the actual size needed for the window and its position to be in the center
    RECT windowRect;
    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = PIXELS_W;
    windowRect.bottom = PIXELS_H;
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    int windowX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (windowWidth / 2);
    int windowY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (windowHeight / 2);



    // Creates the window
    HWND window = CreateWindow(
        className, "Minesweeper",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        windowX, windowY,
        windowWidth, windowHeight,
        0, 0, hInstance, 0
    );
    if (!window) {
        MessageBox(0, "Failed creating Window!", 0, 0);
        return (int)GetLastError;
    }


    // Allocates memory for the pixels(bitmap) and creates the header for it
    bitmapMemory = VirtualAlloc(
        0, 
        PIXELS_W * PIXELS_H * 4,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    BITMAPINFO bitmapInfo;
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = PIXELS_W;
    bitmapInfo.bmiHeader.biHeight = -PIXELS_H;   // Negative height makes top left as the coordinate system origin
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    HDC deviceContext = GetDC(window);



    // loads all images into an array of structs
    struct Image images[SOURCE_IMAGES];
    char fileNames[SOURCE_IMAGES][64] = {"./media/empty.bmp",
                                         "./media/one.bmp",
                                         "./media/two.bmp",
                                         "./media/three.bmp",
                                         "./media/four.bmp",
                                         "./media/five.bmp",  
                                         "./media/six.bmp",
                                         "./media/seven.bmp",
                                         "./media/eight.bmp",
                                         "./media/explodedone.bmp",
                                         "./media/explodedtwo.bmp",
                                         "./media/explodedthree.bmp",
                                         "./media/explodedfour.bmp",
                                         "./media/explodedfive.bmp",  
                                         "./media/explodedsix.bmp",
                                         "./media/explodedseven.bmp",
                                         "./media/explodedeight.bmp",
                                         "./media/flagg.bmp",
                                         "./media/incorrectflag.bmp",
                                         "./media/unknown.bmp",
                                         "./media/bomb.bmp",
                                         "./media/explodedbomb.bmp"};
    for (int i = 0; i < SOURCE_IMAGES; i++) {
        int* imageBitmapData = LoadBitmapFile(fileNames[i], &images[i].w, &images[i].h);
        if (imageBitmapData == NULL) {
            wprintf(L"Failed to load bmp image: %s\n", fileNames[i]);
            return 1;
        } else {
            images[i].data = imageBitmapData;
        }
    }
    


    // starts the counting, (its needed for limiting FPS)
    LARGE_INTEGER startCounter, endCounter, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startCounter);



    // seeding the random number generator
    srand(time(NULL));



    // creating arrays for the game
    int* pmi = malloc(WIDTH * HEIGHT * sizeof(int));
    // 0 - no mine
    // 1 - mine
    
    int* pcD = malloc(WIDTH * HEIGHT * sizeof(int));
    memset(pcD, 0, WIDTH * HEIGHT * sizeof(int));
    // 0-8 - number of surrounding mines (0 if its a mine)
    // (-1) - border
    // constant
    
    int* pcH = malloc(WIDTH * HEIGHT * sizeof(int));
    memset(pcH, 0, WIDTH * HEIGHT * sizeof(int));
    // 0-8 - number of surrounding mines (0 if its a mine)
    // can change
    
    int* pma = malloc(WIDTH * HEIGHT * sizeof(int));
    memset(pma, 0, WIDTH * HEIGHT * sizeof(int));
    // 0 - hidden
    // 1 - exposed
    // (-1) - flagg
    // (-2) - incorrect flagg



    // Creates a map and gets a starting position
    int emptySpaces = tilesW * tilesH;
    int H;
    int W;
    int failed = 0;
    do {
        memset(pmi, 0, WIDTH * HEIGHT * sizeof(int));
        generateMines(pmi, WIDTH, HEIGHT, db);
        countMines(pmi, pcD, WIDTH, HEIGHT);
    } while (getStartingPos(pmi, pcD, &H, &W, WIDTH, HEIGHT) && ++failed < 100);
    if (failed == 100) {
        wprintf(L"Failed to generate mines or a starting position after 100 tries\n");
        return 1;
    }
    matrixCopy(pcD, pcH, WIDTH, HEIGHT);   //copies countDefault to countHidden
    setBorder(pcD, WIDTH, HEIGHT);   //creates a border
    expand(&emptySpaces, pmi, pcH, pma, H, W, WIDTH, HEIGHT);   //expand from starting position
    DrawMap(images, pmi, pcD, pma, WIDTH, HEIGHT, PIXELS_W);



    // game loop
    wprintf(L"There are %d mines to be found!\n", db);
    POINT pos;
    int flagCount = db;
    int counter = 1;
    int alive = 1;
    while (counter > 0) {
        QueryPerformanceCounter(&startCounter);
        if (emptySpaces == 0) {
            wprintf(L" ***  You won!  ***\n");
            counter = 0;
        } else if (alive) {
            counter++;
        } else {
            counter--;
        }

        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                counter = 0;
            } else if (msg.message == WM_LBUTTONDOWN && alive) {
                GetCursorPos(&pos);
                ScreenToClient(window, &pos);
                H = pos.y / PIXELS_PER_TILE + 1;
                W = pos.x / PIXELS_PER_TILE + 1;
                LeftClick(images, &alive, &counter, &emptySpaces, pmi, pcD, pcH, pma, H, W, WIDTH, HEIGHT, PIXELS_W);
            } else if (msg.message == WM_RBUTTONDOWN && alive) {
                GetCursorPos(&pos);
                ScreenToClient(window, &pos);
                H = pos.y / PIXELS_PER_TILE + 1;
                W = pos.x / PIXELS_PER_TILE + 1;
                RightClick(images, &emptySpaces, &flagCount, pmi, pcD, pcH, pma, H, W, WIDTH, HEIGHT, PIXELS_W);
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        StretchDIBits(deviceContext, 
                      0, 0,
                      PIXELS_W, PIXELS_H,
                      0, 0,
                      PIXELS_W, PIXELS_H,
                      bitmapMemory, &bitmapInfo,
                      DIB_RGB_COLORS, SRCCOPY);
        
        // creating delay to limit FPS
        QueryPerformanceCounter(&endCounter);
        float timeToSleep = targetMsPerFrame - (endCounter.QuadPart - startCounter.QuadPart)/(float)frequency.QuadPart;
        if (timeToSleep > 0) {
            Sleep((DWORD)(1000 * timeToSleep));
        }
    }



    free(pmi);
    free(pcD);
    free(pcH);
    free(pma);



    return 0;
}
