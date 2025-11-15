#ifndef UI_H
#define UI_H

void clearScreen();
void pressAnyKey();
void printHeader();
int getMenuChoice();
void getFilenames(char* inputFile, char* outputFile, int mode);
void showProgress(int current, int total, const char* operation);
void compressWithProgress(const char* inputFile, const char* outputFile);
void showMainMenu();

#endif
