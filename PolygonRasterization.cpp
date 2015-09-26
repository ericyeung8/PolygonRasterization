// PolygonRasterization.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include "PolygonRasterization.h"
#include "Loadobj.h"
#include "ScanConvert.h"
#include <gl/glut.h>
#include <gl/gl.h>

std::vector <Vertex> v;
std::vector <Surface> f;
std::vector <Vertex> pNormal;
std::vector <Vertex> vNormal;
float framebuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
float Depthbuffer[WINDOW_HEIGHT][WINDOW_WIDTH][1];
int flag = 0;

Vertex Normalize(Vertex a)
{
	Vertex rvalue;
	float scale;

	scale = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	rvalue.x = a.x / scale;
	rvalue.y = a.y / scale;
	rvalue.z = a.z / scale;

	return rvalue;
}

Vertex CrossProduct(Vertex a, Vertex b)
{
	Vertex rvalue;

	rvalue.x = a.y*b.z - a.z*b.y;
	rvalue.y = a.z*b.x - a.x*b.z;
	rvalue.z = a.x*b.y - a.y*b.x;

	rvalue = Normalize(rvalue);

	return rvalue;
}

Vertex Minus(Vertex a, Vertex b)
{
	Vertex rvalue;

	rvalue.x = a.x - b.x;
	rvalue.y = a.y - b.y;
	rvalue.z = a.z - b.z;

	return rvalue;
}

Vertex Plus(Vertex a, Vertex b)
{
	Vertex rvalue;

	rvalue.x = a.x + b.x;
	rvalue.y = a.y + b.y;
	rvalue.z = a.z + b.z;

	return rvalue;
}

float DotProduct(Vertex a, Vertex b)
{
	float rvalue;
	
	a = Normalize(a);
	b = Normalize(b);
	rvalue = a.x*b.x + a.y*b.y + a.z*b.z;

	return rvalue;
}

Vertex Scale(float a, Vertex b)
{
	Vertex rvalue;

	rvalue.x = a*b.x;
	rvalue.y = a*b.y;
	rvalue.z = a*b.z;

	return rvalue;
}

Vertex Lighting(Vertex n) 
{
	float ka[3] = { 0.1, 0, 0 };
	float kd[3] = { 0.7, 0, 0 };
	float ks[3] = { 0.5, 0.5, 0.5 };
	float A[3] = { 0.5, 0.5, 0.5 };
	float C[3] = { 1, 1, 1 };
	Vertex L(1, 1, -1);
	Vertex E(0, 0, -1);
	Vertex R;
	int exp = 5;
	float temp;
	float Diffuse;
	float Specular;
	Vertex rvalue;

	L = Normalize(L);
	n = Normalize(n);

	//Calculate the vector R
	temp = 2.0 * DotProduct(L, n);
	R = Minus(Scale(temp, n), L);

	Diffuse = DotProduct(L, n);
	Specular = pow((double)DotProduct(R, E), (double)exp);

	if (Diffuse < 0)
	{
		Diffuse = 0;
		Specular = 0;
	}

	if (Specular < 0)
		Specular = 0;

	//Calculate the Lighting value
	rvalue.x = ka[0] * A[0] + C[0] * (kd[0] * Diffuse + ks[0] * Specular);
	rvalue.y = ka[1] * A[1] + C[1] * (kd[1] * Diffuse + ks[1] * Specular);
	rvalue.z = ka[2] * A[2] + C[2] * (kd[2] * Diffuse + ks[2] * Specular);

	return rvalue;
}

void CalculateNormal(std::vector <Vertex> &VertexNormal)
{
	int i;

	//Calculate the sum of the normal connected the same vertex
	for (i = 0; i <= (int)f.size() - 1; i++)
	{
		Vertex v1, v2, v3;
		Vertex PlaneNormal;
		Vertex temp;

		v1 = v[f[i].a - 1];
		v2 = v[f[i].b - 1];
		v3 = v[f[i].c - 1];

		PlaneNormal = CrossProduct(Minus(v3, v1), Minus(v2, v1));

		VertexNormal[f[i].a - 1] = Plus(VertexNormal[f[i].a - 1], PlaneNormal);
		VertexNormal[f[i].b - 1] = Plus(VertexNormal[f[i].b - 1], PlaneNormal);
		VertexNormal[f[i].c - 1] = Plus(VertexNormal[f[i].c - 1], PlaneNormal);
	}

	//Normalize the vertex normal
	for (i = 0; i <= (int)v.size() - 1; i++)
		VertexNormal[i] = Normalize(VertexNormal[i]);

}

void FlatNormal(std::vector <Vertex> &Normal)
{
	int i;

	//Calculate each plane's normal
	for (i = 0; i <= (int)f.size() - 1; i++)
	{
		Vertex v1, v2, v3;
		Vertex PlaneNormal;

		v1 = v[f[i].a - 1];
		v2 = v[f[i].b - 1];
		v3 = v[f[i].c - 1];

		PlaneNormal = CrossProduct(Minus(v3, v1), Minus(v2, v1));
		Normal[i] = PlaneNormal;
	}
}

void clearFramebuffer()
{
	int i, j;

	for (i = 0; i<WINDOW_HEIGHT; i++) {
		for (j = 0; j<WINDOW_WIDTH; j++) {
			framebuffer[i][j][0] = 0.0;
			framebuffer[i][j][1] = 0.0;
			framebuffer[i][j][2] = 0.0;
		}
	}
}

void clearDepthbuffer()
{
	int i, j;

	for (i = 0; i<WINDOW_HEIGHT; i++) {
		for (j = 0; j<WINDOW_WIDTH; j++) {
			Depthbuffer[i][j][0] = 10;
		}
	}
}

void setFramebuffer(int x, int y, float R, float G, float B)
{
	// changes the origin from the lower-left corner to the upper-left corner
	y = WINDOW_HEIGHT - 1 - y;
	if (R <= 1.0)
	if (R >= 0.0)
		framebuffer[y][x][0] = R;
	else
		framebuffer[y][x][0] = 0.0;
	else
		framebuffer[y][x][0] = 1.0;
	if (G <= 1.0)
	if (G >= 0.0)
		framebuffer[y][x][1] = G;
	else
		framebuffer[y][x][1] = 0.0;
	else
		framebuffer[y][x][1] = 1.0;
	if (B <= 1.0)
	if (B >= 0.0)
		framebuffer[y][x][2] = B;
	else
		framebuffer[y][x][2] = 0.0;
	else
		framebuffer[y][x][2] = 1.0;
}

void drawit(void)
{
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, framebuffer);
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	/*clear the screen*/
	glClear(GL_COLOR_BUFFER_BIT);

	/*clear the buffer*/
	clearDepthbuffer();
	clearFramebuffer();

	/*Allocate vector's size*/
	pNormal.resize(f.size());
	vNormal.resize(v.size());

	/*Calculate the plane normal and vertex normal*/
	FlatNormal(pNormal);
	CalculateNormal(vNormal);
}

void display(void)
{	
	int i;

	if (flag != 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		clearDepthbuffer();
		clearFramebuffer();
		
		for (i = 0; i <= (int)f.size() - 1; i++)
		{
			struct PolygonStruc *pout;
			std::vector <Vertex> normal;

			pout = new PolygonStruc;

			/*Store the triangle*/
			pout->q.push(v[f[i].a - 1]);
			pout->q.push(v[f[i].b - 1]);
			pout->q.push(v[f[i].c - 1]);

			/*Store the three vertex normal of the triangle*/
			normal.push_back(vNormal[f[i].a - 1]);
			normal.push_back(vNormal[f[i].b - 1]);
			normal.push_back(vNormal[f[i].c - 1]);

			/*Call the function to draw the triangle*/
			PolygonFill(normal, pout, Depthbuffer, pNormal[i], flag);
		}
	}

	drawit();
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		flag = 1;
		break;
	case '2':
		flag = 2;
		break;
	case '3':
		flag = 3;
		break;
	}

	glutPostRedisplay();
}

int main(int argc, char*argv[])
{
	if (argc != 2)
	{
		printf("Please input the obj file name!\n");
		exit(1);
	}
	else printf("%s", argv[1]);

	Loadobj(v, f, argv[1]);

	//Loadobj(v, f, "E:\\cow.obj");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Jiajun Yang - Assignment 4");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}