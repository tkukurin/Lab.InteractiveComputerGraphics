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
#include "common_gl_methods.cpp"

using namespace std;

vector<glm::mat3> shapeData;
glm::vec3 g_eye;
glm::vec3 g_view;

void draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 0.0f);

	double H = glm::distance(g_eye, g_view);
	glm::mat4 transformMatrix = getTransformMatrix(g_eye, g_view) * getPerspectiveMatrix(H);

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

	glutSwapBuffers();
	glutMainLoop();

	return 0;
}
