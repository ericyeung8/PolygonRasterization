#include "stdafx.h"
#include <fstream>
#include <vector>
#include "DataStruc.h"

void Loadobj(std::vector <Vertex> &newv, std::vector <Surface> &newf, char *filename)
{
	char buffer[256];

	std::ifstream in(filename);
	
	/*Check the file whether is good or not*/
	if (!in.good())
	{
		printf("Please input the correct obj file's path!\n");
		printf("Remind: Please use double slashes instead of single slash\n");
		exit(EXIT_FAILURE);
	}
	/*Read the file by line*/
	while (!in.getline(buffer, sizeof(buffer)).eof())
	{
		float x, y, z;
		int a, b, c;

		buffer[255] = 0;

		if (buffer[0] == 'v')
		{
			if (sscanf_s(buffer, "v %f %f %f", &x, &y, &z) == 3)
			{
				newv.push_back(Vertex(x, y, z));
			}
			else
			{
				printf("Obj file format error!\n");
				exit(EXIT_FAILURE);
			}
		}
		else if (buffer[0] == 'f')
		{
			if (sscanf_s(buffer, "f %d %d %d", &a, &b, &c) == 3)
				newf.push_back(Surface(a, b, c));
			else
			{
				printf("Obj file format error!\n");
				exit(EXIT_FAILURE);
			}
		}
		else if (buffer[0] == '#')
			continue;
	}
}