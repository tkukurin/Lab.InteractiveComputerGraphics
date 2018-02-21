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
vector<glm::vec3> vertexData;
vector<glm::mat3> shapeData;
vector<glm::vec3> g_controlPolygon;
vector<glm::vec3> bezierPoints;

glm::vec3 g_eye;
glm::vec3 g_view;

vector<glm::vec3> bezier(vector<glm::vec3> controlPolygon, int divs)
{
	vector<double> factors;
	int n = controlPolygon.size() - 1;

	for(int i = 1, a = 1; i <= n + 1; i++) {
		factors.push_back(a);
		a = (a * (n - i + 1)) / i;
	}

	double delta = 1.0 / divs;
	double t = 0;
	vector<glm::vec3> points;
	for(int i = 0; i <= divs; i++) {
		glm::vec3 curr(0, 0, 0);

		for(int j = 0; j <= n; j++) {
			double b = factors[j] * pow(1 - t, n - j) * pow(t, j);
			curr.x += b * controlPolygon[j].x;
			curr.y += b * controlPolygon[j].y;
			curr.z += b * controlPolygon[j].z;
		}

		points.push_back(curr);
		t += delta;
	}
	
	return points;
}

bool isVisible(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 eye)
{
		glm::vec3 n = glm::cross(v3 - v1, v2 - v1);
    glm::vec3 c = (v1 + v2 + v3) * (1.0f/3.0f);
    glm::vec3 np = eye - c;

    return glm::dot(n, np) >= 0;
}

void draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(1.0);
	glColor3f(1.0f, 0.0f, 0.0f);

	double H = glm::distance(g_eye, g_view);
	glm::mat4 transformMatrix = getTransformMatrix(g_eye, g_view) * getPerspectiveMatrix(H);

	for (int i = 0; i < shapeData.size(); i++) {
		glBegin(GL_LINE_LOOP);
		glm::mat3 curr = shapeData[i];

		glm::vec4 v1 = apply(transformMatrix, curr[0]);
		glm::vec4 v2 = apply(transformMatrix, curr[1]);
		glm::vec4 v3 = apply(transformMatrix, curr[2]);

		if (!isVisible(v1, v2, v3, g_eye)) {
			continue;
		}

		glVertex4v(v1);
		glVertex4v(v2);
		glVertex4v(v3);
		glEnd();
	}
	
	glColor3f(0.0f, 1.0f, 1.0f);
	glPointSize(5.0f);
  glBegin(GL_POINTS);
  for (auto p : g_controlPolygon) {
    glm::vec4 point = apply(transformMatrix, p);
		glVertex4v(point);
  }
  glEnd();

  glColor3f(1.0f, 0.0f, 1.0f);
	glLineWidth(2.0f);

  glBegin(GL_LINE_STRIP);
  for (auto p : bezierPoints) {
    glm::vec4 point = apply(transformMatrix, p);
		glVertex4v(point);
  }

  glEnd();
	glLineWidth(1.0f);

	glutSwapBuffers();
}

int currentViewPosition = 0;

void advanceFrame(int amount)
{
	currentViewPosition += amount;
	
	if (currentViewPosition >= bezierPoints.size())
		currentViewPosition = 1;

	g_view = bezierPoints[currentViewPosition];
	draw();
}

void moveView(unsigned char key, int x, int y)
{
	if (currentViewPosition) {
		advanceFrame(10);
		return;
	}

	g_controlPolygon = vector<glm::vec3>(vertexData);
	bezierPoints = bezier(g_controlPolygon, 100);

	advanceFrame(1);
}

int main(int argc, char ** argv) 
{
	if (argc < 3) {
		cerr << "[program] [objfile] [eyeView] [polygon]" << endl;
		return -1;
	}

	if (argc == 4) {
		ifstream polyFile(argv[3], ios::out);
		g_controlPolygon = readPolygonFile(polyFile);
		bezierPoints = bezier(g_controlPolygon, 100);
		polyFile.close();
	}

	ifstream eyeViewFile(argv[2], ios::out);
	g_view = readVec3(eyeViewFile);
	g_eye = readVec3(eyeViewFile);
	eyeViewFile.close();

	ifstream objFile(argv[1], ios::out);
	vertexData = readVertices(objFile);
	shapeData = readFaces(objFile, vertexData);
	objFile.close();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(sub_width, sub_height);
	glutInit(&argc, argv);
	glutCreateWindow("Zadatak 6");
	glutReshapeFunc(updateOnReshape);
	glutDisplayFunc(draw);
	glutKeyboardFunc(moveView);

	glutSwapBuffers();
	glutMainLoop();

	return 0;
}
