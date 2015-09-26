#ifndef POLYGONRASTERIZATION_H
#define POLYGONRASTERIZATION_H

#include "DataStruc.h"

Vertex Normalize(Vertex a);
Vertex CrossProduct(Vertex a, Vertex b);
Vertex Minus(Vertex a, Vertex b);
Vertex Plus(Vertex a, Vertex b);
float DotProduct(Vertex a, Vertex b);
Vertex Scale(float a, Vertex b);
Vertex Lighting(Vertex n);
void CalculateNormal(std::vector <Vertex> &VertexNormal);
void FlatNormal(std::vector <Vertex> &Normal);
void setFramebuffer(int x, int y, float R, float G, float B);
void clearFramebuffer();
void drawit(void);

#endif