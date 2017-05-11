
#include <iostream>
#include <GL/glut.h>
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <fstream>
using namespace std;

const char COMMENT_LINE_TYPE = '#';
const char FACE_LINE_TYPE = 'f'; 
const char VERTEX_LINE_TYPE = 'v';

typedef struct {
	double x, y;
} Point;

struct {
	Point minP;
	Point maxP;
} limits;

void updateLimits(Point p) {
	limits.maxP.x = max(p.x, limits.maxP.x);
	limits.maxP.y = max(p.y, limits.maxP.y);
	limits.minP.x = min(p.x, limits.minP.x);
	limits.minP.y = min(p.y, limits.minP.y);
}

bool isCommentOrGroup(char lineType) 
{
	return lineType == COMMENT_LINE_TYPE || lineType == 'g';
}

glm::vec3 readVec3(ifstream &f)
{
	double x, y, z;
	f >> x >> y >> z;
	return glm::vec3(x, y, z);
}

vector<glm::vec3> readVertices(ifstream &f)
{
	const glm::vec3 EMPTY_VEC(0,0,0);
	vector<glm::vec3> data;
	data.push_back(EMPTY_VEC);
	
	char lineType;
	streampos cached = f.tellg();

	while (f >> lineType) {
		if(isCommentOrGroup(lineType)) {
			string comment;
			getline(f, comment); 

			cerr << "Preskacem komentar: " << comment << endl; 
			continue;
		}

		if (lineType != VERTEX_LINE_TYPE) {
			break;
		}

		double x, y, z;
		f >> x >> y >> z;

		data.push_back(glm::vec3(x, y, z));
		updateLimits({ .x = x, .y = y });

		cached = f.tellg();
	}
	
	f.seekg(cached);
	return data;
}

vector<glm::mat3> readFaces(ifstream &f, vector<glm::vec3> &vertexData) 
{
	vector<glm::mat3> shapeData;

	char lineType;

	while(f >> lineType) {
		if(isCommentOrGroup(lineType)) {
			string comment;
			getline(f, comment); 

			cerr << "Preskacem: " << comment << endl; 
			continue;
		}

		if (lineType != FACE_LINE_TYPE) {
			cerr << "Neocekivan znak: " << lineType << endl;
			break;
		}

		int i1, i2, i3;
		f >> i1 >> i2 >> i3;

		shapeData.push_back(glm::mat3(vertexData[i1],
																  vertexData[i3],
																	vertexData[i2]));
	}
	
	return shapeData;
}
