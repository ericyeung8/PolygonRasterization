#ifndef _DATASTRUC_H_
#define _DATASTRUC_H_

#include <queue>

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

class Vertex
{
public:
	float x, y, z;
	Vertex(float newX, float newY, float newZ)
	{
		x = newX;
		y = newY;
		z = newZ;
	}
	Vertex(void)
	{
		x = y = z = 0;
	}
};

class Surface
{
public:
	int a, b, c;
	Surface(int newA, int newB, int newC)
	{
		a = newA;
		b = newB;
		c = newC;
	}
	Surface(void)
	{
		a = b = c = 0;
	}
};

class PlanePara
{
public:
	float a,b,c,d;
	PlanePara(void)
	{
		a = b = c = d = 0;
	}
};

struct edge {
	float maxy;
	float currentx;
	float dx;
	Vertex currentF;
	Vertex df;
	edge *next;
};

struct color {
	float r, g, b;		// Color (R,G,B values)
};

struct PolygonStruc {
	std::queue<Vertex> q;
};

#endif