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
#include "parsing.cpp"
#include "transforms.cpp"

using namespace std;

vector<glm::mat3> shapeData;

glm::vec3 g_eye;
glm::vec3 g_view;

GLuint sub_width = 256, 
			 sub_height = 256;

void print(glm::mat4 m) {
	cout << glm::to_string(m) << endl;
}

void updateOnReshape(int w, int h) 
{
	sub_width = w;
	sub_height = h;

	glViewport(0, 0, sub_width, sub_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-w/200.0, w/200.0, -h/200.0, h/200.0);
}

void glVertex3v(glm::vec3 v, double H)
{
	glVertex3f(v.x * (H / v.z), v.y * (H / v.z), 0);
}

void glVertex4v(glm::vec4 v)
{
	glVertex3f(v.x / v.w, v.y / v.w, 0);
}

void draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(1.0);
	glColor3f(1.0f, 0.0f, 0.0f);

	glm::mat4 transformMatrix = getTransformMatrix(g_eye, g_view);

	float H = glm::distance(g_view, g_eye);
	glm::mat4 perspective = getPerspectiveMatrix(H);
	
	transformMatrix = transformMatrix * perspective;

	for (int i = 0; i < shapeData.size(); i++) {
		glBegin(GL_LINE_LOOP);
		glm::mat3 curr = shapeData[i];

		glm::vec4 v1 = apply(transformMatrix, curr[0]);
		glm::vec4 v2 = apply(transformMatrix, curr[1]);
		glm::vec4 v3 = apply(transformMatrix, curr[2]);

		glVertex4v(v1); 
		glVertex4v(v2); 
		glVertex4v(v3); 

		glEnd();
	}

	glutSwapBuffers();
}

int main(int argc, char ** argv) 
{
	if (argc != 3) {
		cerr << "Usage: program [objectFile] [eyeViewFile]" << endl;
		return -1;
	}

	limits.minP.x = limits.minP.y = 10000;
	limits.maxP.x = limits.maxP.y = -10000;

	ifstream eyeViewFile(argv[2], ios::out);
	g_view = readVec3(eyeViewFile);
	g_eye = readVec3(eyeViewFile);
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
