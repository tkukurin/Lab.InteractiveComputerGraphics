#include <iostream>
#include <GL/glut.h>
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;
const char COMMENT_LINE_TYPE = '#';
const char FACE_LINE_TYPE = 'f'; 
const char VERTEX_LINE_TYPE = 'v';

vector<glm::mat3> shapeData;
vector<glm::vec4> planeEquations;
GLuint sub_width = 256, 
			 sub_height = 256;

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

vector<glm::vec4> getPlaneEquations(vector<glm::mat3> &shapeData)
{
	vector<glm::vec4> planeEquations;

	for (int i = 0; i < shapeData.size(); i++) {
		glm::mat3 curr = shapeData[i];
		glm::vec3 v1 = curr[0];
		glm::vec3 v2 = curr[1];
		glm::vec3 v3 = curr[2];

		double A = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
		double B = -(v2.x - v1.x) * (v3.z - v1.z) + (v2.z - v1.z) * (v3.x - v1.x);
		double C = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);
		double D = -v1.x * A - v1.y * B - v1.z * C;

		planeEquations.push_back(glm::vec4(A, B, C, D));
	}

	return planeEquations;
}

bool isWithinLoadedObject(glm::vec3 point)
{
	glm::vec4 pointH(point, 1);
	for (int i = 0; i < planeEquations.size(); i++) {
		int dp = glm::dot(pointH, planeEquations[i]);
		if (dp > 0) {
			return false;
		}
	}

	return true;
}

void updateOnReshape(int w, int h) 
{
	sub_width = w;
	sub_height = h;

	glViewport(0, 0, sub_width, sub_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double dx = -limits.minP.x + limits.maxP.x;
	double dy = -limits.minP.y + limits.maxP.y;

	double sx = w / dx;
	double sy = h / dy;

	double originDistX = -dx/2 - limits.minP.x;
	double originDistY = -dy/2 - limits.minP.y;

	double scalingFactor = min(sx, sy) / 2;

	gluOrtho2D(-w/2, w/2, -h/2, h/2);
	glScalef(scalingFactor, scalingFactor, 1);
	glTranslatef(originDistX, originDistY, 0);

	glMatrixMode(GL_MODELVIEW);
}

void draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(1.0);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (int i = 0; i < shapeData.size(); i++) {
		glBegin(GL_LINE_LOOP);
		glm::mat3 curr = shapeData[i];
		glm::vec3 v1 = curr[0];
		glm::vec3 v2 = curr[1];
		glm::vec3 v3 = curr[2];
		glVertex3f(v1.x, v1.y, 0); // v1.z); // (z=0 radi upute u zadatku)
		glVertex3f(v2.x, v2.y, 0); // v2.z);
		glVertex3f(v3.x, v3.y, 0); // v3.z);
		glEnd();
	}

	glutSwapBuffers();
}

void getPointCoordinates(unsigned char key, int x, int y)
{
	switch(key) {
		case 'i':
			double x,y,z;
			cout << "Unesite koordinate tocke: ";
			cin >> x >> y >> z;

			bool isWithin = isWithinLoadedObject(glm::vec3(x, y, z));
			cout << (isWithin ? "Unutar" : "Izvan") << endl;

			break;
	}
}

int main(int argc, char ** argv) 
{
	if (argc != 2) {
		cerr << "Gimmie name of objfile pls" << endl;
		return -1;
	}

	limits.minP.x = limits.minP.y = 10000;
	limits.maxP.x = limits.maxP.y = -10000;

	ifstream objFile(argv[1], ios::out);
	vector<glm::vec3> vertexData = readVertices(objFile);
	shapeData = readFaces(objFile, vertexData);
	objFile.close();

	planeEquations = getPlaneEquations(shapeData);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(sub_width, sub_height);
	glutInit(&argc, argv);
	glutCreateWindow("Zadatak 4");
	glutReshapeFunc(updateOnReshape);
	glutDisplayFunc(draw);
	glutKeyboardFunc(getPointCoordinates);
	glutMainLoop();

	return 0;
}
