#include "stdafx.h"
#include "PolygonRasterization.h"
#include "DataStruc.h"

PlanePara CalculatePlane(Vertex a, Vertex b, Vertex c)
{
	PlanePara rvalue;
	Vertex normal;

	/*Calculate the plane function: ax+by+cd+z=0*/
	normal = CrossProduct(Minus(c, a), Minus(b, a));
	rvalue.a = normal.x;
	rvalue.b = normal.y;
	rvalue.c = normal.z;
	rvalue.d = -1.0*(normal.x*a.x + normal.y*a.y + normal.z*a.z);

	return rvalue;
}

void PolygonFill(std::vector <Vertex> &VertexNormal, PolygonStruc *pg, float Depthbuff[][WINDOW_WIDTH][1], Vertex PlaneNormal, int mode)
{
	unsigned int ymax = 0;
	unsigned int ymin = WINDOW_HEIGHT;
	unsigned int i = 0, j = 0;
	struct edge *ET[WINDOW_HEIGHT];
	struct edge *AEL;
	std::queue <Vertex> fcolor;
	std::vector <Vertex> point;
	Vertex rgb;
	Vertex temp;
	Vertex temp1, temp2;
	Vertex ftemp1, ftemp2;
	PlanePara para;

	AEL = new edge;
	AEL->next = NULL;

	/*Initialize the active edge table*/
	for (i = 0; i <= WINDOW_HEIGHT - 1; i++)
	{
		ET[i] = new edge;
		ET[i]->next = NULL;
	}

	/*Calculate the ymax and ymin of the polygon*/
	for (i = 1; i <= pg->q.size(); i++)
	{
		temp = pg->q.front();
		point.push_back(temp);
		pg->q.pop();

		temp.x = (WINDOW_WIDTH - 1) / 2 * (temp.x + 1);
		temp.y = (WINDOW_HEIGHT - 1) / 2 * (1 - temp.y);

		if (ymax <= temp.y)
			ymax = temp.y;
		if (ymin >= temp.y)
			ymin = temp.y;
		pg->q.push(temp);
	}

	/*Call the function to calculate the plane function*/
	para = CalculatePlane(point[0], point[1], point[2]);

	/*Initialize the color value depending on the mode*/
	if (mode == 1)
	{
		/*Flat*/
		for (i = 0; i <= (int)pg->q.size() - 1; i++)
		{
			fcolor.push(Vertex(0, 0, 0));
		}
	}
	else if (mode == 2)
	{
		/*Gouraud*/
		for (i = 0; i <= (int)pg->q.size() - 1; i++)
		{
			fcolor.push(Lighting(VertexNormal[i]));
		}
	}
	else if (mode == 3)
	{
		/*Phong*/
		/*Because of the order, we switch the element here*/
		Vertex swapcontent;
		swapcontent = VertexNormal[0];
		VertexNormal[0] = VertexNormal[2];
		VertexNormal[2] = swapcontent;

		for (i = 0; i <= (int)pg->q.size() - 1; i++)
		{
			fcolor.push(VertexNormal.back());
			VertexNormal.pop_back();
		}
	}

	/*Insert the edge to the active edge table*/
	for (i = ymin; i <= ymax; i++)
	{
		for (j = 1; j <= pg->q.size(); j++)
		{
			temp1 = pg->q.front();
			pg->q.pop();
			ftemp1 = fcolor.front();
			fcolor.pop();

			if ((int)temp1.y == i)
			{
				temp2 = pg->q.front();
				ftemp2 = fcolor.front();

				if ((int)temp1.y > (int)temp2.y)
				{
					edge *p = new edge;
					edge *q = new edge;

					q = ET[(int)temp2.y];
					while (q->next != NULL)	//find the last element
						q = q->next;

					p->maxy = temp1.y;
					p->currentx = temp2.x;
					p->dx = (temp1.x - temp2.x) / (temp1.y - temp2.y);
					p->currentF = ftemp2;
					p->df = Scale(1.0 / (temp1.y - temp2.y), Minus(ftemp1, ftemp2));
					p->next = NULL;
					q->next = p;
				}
				else if ((int)temp1.y < (int)temp2.y)
				{
					edge *p = new edge;
					edge *q = new edge;

					q = ET[(int)temp1.y];
					while (q->next != NULL)
						q = q->next;

					p->maxy = temp2.y;
					p->currentx = temp1.x;
					p->dx = (temp1.x - temp2.x) / (temp1.y - temp2.y);
					p->currentF = ftemp1;
					p->df = Scale(1.0 / (temp1.y - temp2.y), Minus(ftemp1, ftemp2));
					p->next = NULL;
					q->next = p;
				}
			}
			pg->q.push(temp1);	//push the first point into queue
			fcolor.push(ftemp1);
		}
	}

	/*Draw the polygon*/
	for (i = ymin; i <= ymax; i++)
	{
		edge *p = new edge;
		edge *q = new edge;

		p = AEL->next;

		while (p) //calculate the new x position
		{
			p->currentx = p->currentx + p->dx;
			p->currentF = Plus(p->currentF, p->df);
			p = p->next;
		}

		/*sort the linked list, to be continued*/
		edge *r = new edge;
		p = AEL;
		while (p->next && p->next->next)	//at least 2 elements
		{
			q = p->next;
			r = q->next;
			while (r)
			{
				edge *s = new edge;
				if (q->currentx > r->currentx)
				{
					p->next = r;
					q->next = r->next;
					r->next = q;
					s = r;
					r = q->next;
					q = s;
				}
				else	r = r->next;
			}
			p = q;
		}

		r = AEL;
		p = ET[i]->next;
		q = AEL->next;
		while (p)	//+edge
		{
			if (q != NULL)
			{
				if (p->currentx <= (int)q->currentx)	//may have error due to the dx
				{
					edge *s = new edge;
					s->currentx = p->currentx;
					s->dx = p->dx;
					s->maxy = p->maxy;
					s->currentF = p->currentF;
					s->df = p->df;
					s->next = q;
					r->next = s;
					p = p->next;

					r = AEL;
					q = AEL->next;
				}
				else
				{
					r = r->next;
					q = q->next;
				}
			}
			else
			{
				edge *s = new edge;
				s->currentx = p->currentx;
				s->dx = p->dx;
				s->maxy = p->maxy;
				s->currentF = p->currentF;
				s->df = p->df;
				s->next = NULL;
				r->next = s;
				p = p->next;
				r = AEL;
				q = AEL->next;
			}

		}

		p = AEL;
		q = AEL->next;
		while (q)	//delete edge
		{
			if ((int)q->maxy == i)
			{
				p->next = q->next;
				delete q;
				q = p->next;
			}
			else
			{
				q = q->next;
				p = p->next;
			}
		}

		p = AEL->next;
		while (p && p->next)	//setframebuffer
		{
			q = p->next;
			for (j = ceil(p->currentx); j <= (unsigned int)floor(q->currentx); j++)
			{
				float depth;
				Vertex cIncr;
				float x, y;

				/*Retransform the coodinate to the original one*/
				x = 2.0 * j / (WINDOW_WIDTH - 1) - 1;
				y = 1 - 2.0 *i / (WINDOW_HEIGHT - 1);

				/*Calculate each pixel's depth*/
				depth = -(para.a*x + para.b*y + para.d) / para.c;

				/*In case p->currentx = q->currentx*/
				if (ceil(p->currentx) == floor(q->currentx))
					cIncr = Vertex(0, 0, 0);
				else
					cIncr = Scale(1.0 / (p->currentx - q->currentx), Minus(p->currentF, q->currentF));

				/*Z-buffering algorithm*/
				if (depth < Depthbuff[i][j][0])
				{
					if (mode == 1)
						rgb = Lighting(PlaneNormal);
					else if (mode == 2)
						rgb = Plus(p->currentF, Scale((j - p->currentx), cIncr));
					else if (mode == 3)
					{
						rgb = Plus(p->currentF, Scale((j - p->currentx), cIncr));
						rgb = Lighting(rgb);
					}

					/*Update the depthbuffering*/
					Depthbuff[i][j][0] = depth;
					setFramebuffer((int)j, (int)i, rgb.x, rgb.y, rgb.z);
				}
			}
			p = q->next;
		}
	}
}