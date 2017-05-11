#include <iostream>
#include <GL/glut.h>
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;
const char COMMENT_LINE_TYPE = '#';
const char FACE_LINE_TYPE = 'f'; 
const char VERTEX_LINE_TYPE = 'v';

vector<glm::mat3> shapeData;

glm::vec3 eye;
glm::vec3 view;

GLuint sub_width = 256, 
			 sub_height = 256;

typedef struct {
	double x, y;
} Point;

struct {
	Point minP;
	Point maxP;
} limits;

void print(glm::mat4 m) {
	cout << glm::to_string(m) << endl;
}

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

void updateOnReshape(int w, int h) 
{
	sub_width = w;
	sub_height = h;
	double r = (double)h / w;

	glViewport(0, 0, sub_width, sub_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(-1, 1, -r, r);
	gluOrtho2D(-w/2, w/2, -h/2, h/2);
}

glm::vec4 apply(glm::mat4 transformMatrix, glm::vec3 vec)
{
	return glm::vec4(vec.x, vec.y, vec.z, 1) * transformMatrix;
}

void glVertex3v(glm::vec3 v, double H)
{
	glVertex3f(v.x * (H / v.z), v.y * (H / v.z), 0);
	//glVertex3f(v.x / H, v.y / H, 0);
}

void draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(1.0);
	glColor3f(1.0f, 0.0f, 0.0f);

	glm::vec4 view2(view.x, view.y, view.z, 1);
	glm::vec4 eye2(eye.x, eye.y, eye.z, 1);

	glm::mat4 T1(1, 0, 0, 0,
							 0, 1, 0, 0,
							 0, 0, 1, 0,
							 -eye.x, -eye.y, -eye.z, 1);
	T1 = glm::transpose(T1);
	view2 = apply(T1, view2);

	double sAlpha = sqrt(view2.x*view2.x + view2.y*view2.y);
	double sinAlpha = view2.y / sAlpha;
	double cosAlpha = view2.x / sAlpha;

	glm::mat4 T2(cosAlpha, -sinAlpha, 0, 0,
							 sinAlpha, cosAlpha, 0, 0,
										 0, 0, 1, 0,
										 0, 0, 0, 1);
	T2 = glm::transpose(T2);
	view2 = apply(T2, view2);

	double sBeta = sqrt(view2.x*view2.x + view2.z*view2.z);
	double sinBeta = view2.x / sBeta;
	double cosBeta = view2.z / sBeta;
	glm::mat4 T3(cosBeta, 0, sinBeta, 0, 
										 0, 1, 0, 0,
										 -sinBeta, 0, cosBeta, 0,
									   0, 0, 0, 1);
	T3 = glm::transpose(T3);
	
	glm::mat4 T4(0, -1, 0, 0,
							 1, 0, 0, 0,
							 0, 0, 1, 0,
							 0, 0, 0, 1);
	T4 = glm::transpose(T4);

	glm::mat4 T5(-1, 0, 0, 0,
							 0, 1, 0, 0,
							 0, 0, 1, 0,
							 0, 0, 0, 1);
	T5 = glm::transpose(T5);

	glm::mat4 m = T1 * T2 * T3 * T4 * T5;

	float H = glm::distance(view, eye);
	glm::mat4 perspective(1, 0, 0, 0,
											  0, 1, 0, 0,
												0, 0, 0, 1/H,
												0, 0, 0, 0);
	perspective = glm::transpose(perspective);

	double scaleX = 1;//(limits.maxP.x - limits.minP.x);
	double scaleY = 1;//(limits.maxP.y - limits.minP.y);
	glm::mat4 scale = glm::transpose(glm::mat4(
				1/scaleX, 0, 0, 0,
				0, 1/scaleY, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1));
	//scale = glm::mat4();

	for (int i = 0; i < shapeData.size(); i++) {
		glBegin(GL_LINE_LOOP);
		glm::mat3 curr = shapeData[i];
		glm::vec3 v1 = curr[0];
		glm::vec3 v2 = curr[1];
		glm::vec3 v3 = curr[2];

		glm::vec3 n = glm::cross(v3 - v1, v2 - v1);
    glm::vec3 c = (v1 + v2 + v3) * (1.0f/3.0f);
    glm::vec3 np = eye - c;

		v1 = apply(m, curr[0]) * scale;
		v2 = apply(m, curr[1]) * scale;
		v3 = apply(m, curr[2]) * scale;

    if (glm::dot(n, np) < 0) {
			cout << "not displaying" << endl;
      continue;
    }

		glVertex3v(v1, H); glVertex3v(v2, H); glVertex3v(v3, H);
		glEnd();
	}

	glutSwapBuffers();
}

int main(int argc, char ** argv) 
{
if (argc != 3) {
	cerr << "Gimmie name of objfile pls" << endl;
		return -1;
	}

	limits.minP.x = limits.minP.y = 10000;
	limits.maxP.x = limits.maxP.y = -10000;

	ifstream eyeViewFile(argv[2], ios::out);
	view = readVec3(eyeViewFile);
	eye = readVec3(eyeViewFile);
	eyeViewFile.close();

	ifstream objFile(argv[1], ios::out);
	vector<glm::vec3> vertexData = readVertices(objFile);
	shapeData = readFaces(objFile, vertexData);
	objFile.close();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(sub_width, sub_height);
	glutInit(&argc, argv);
	glutCreateWindow("Zadatak 5");
	glutReshapeFunc(updateOnReshape);
	glutDisplayFunc(draw);
	glutMainLoop();

	return 0;
}
