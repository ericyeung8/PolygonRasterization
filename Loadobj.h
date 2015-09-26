#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <fstream>
#include <vector>
#include "DataStruc.h"

int MyCheckPath(char *filename);
void Loadobj(std::vector <Vertex> &newv, std::vector <Surface> &newf, char *filename);

#endif