#ifndef SCANCONVERT_H
#define SCANCONVERT_H

#include "DataStruc.h"

PlanePara CalculatePlane(Vertex a, Vertex b, Vertex c);
void PolygonFill(std::vector <Vertex> &VertexNormal, PolygonStruc *pg, float Depthbuff[][WINDOW_WIDTH][1], Vertex PlaneNormal, int mode);

#endif