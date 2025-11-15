

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <sys/stat.h>

long getFileSize(const char* filename);
int fileExists(const char* filename);
void getFileInfo(const char* filename);
void compareFiles(const char* file1, const char* file2);

#endif
