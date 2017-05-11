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
vector<glm::vec3> controlPolygon;
vector<glm::vec3> bezierPoints;

glm::vec3 g_eye;
glm::vec3 g_view;

GLuint sub_width = 256, 
			 sub_height = 256;

void print(glm::mat4 m) {
	cout << glm::to_string(m) << endl;
}

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


void updateOnReshape(int w, int h) 
{
	sub_width = w;
	sub_height = h;

	glViewport(0, 0, sub_width, sub_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-w/200.0, w/200.0, -h/200.0, h/200.0);
}


void glVertex4v(glm::vec4 v)
{
	glVertex3f(v.x / v.w, v.y / v.w, 0);
}

bool isVisible(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
		glm::vec3 n = glm::cross(v3 - v1, v2 - v1);
    glm::vec3 c = (v1 + v2 + v3) * (1.0f/3.0f);
    glm::vec3 np = g_eye - c;

    return glm::dot(n, np) >= 0;
}

void draw() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(1.0);
	glColor3f(1.0f, 0.0f, 0.0f);

	glm::mat4 m = getTransformMatrix(g_eye, g_view);
	double H = glm::distance(g_eye, g_view);
	glm::mat4 perspective = getPerspectiveMatrix(H);
	m = m * perspective;

	for (int i = 0; i < shapeData.size(); i++) {
		glBegin(GL_LINE_LOOP);
		glm::mat3 curr = shapeData[i];

		if (!isVisible(curr[0], curr[1], curr[2])) {
			continue;
		}

		glm::vec4 v1 = apply(m, curr[0]);
		glm::vec4 v2 = apply(m, curr[1]);
		glm::vec4 v3 = apply(m, curr[2]);

		glVertex4v(v1);
		glVertex4v(v2);
		glVertex4v(v3);
		glEnd();
	}
	
	glColor3f(0.0f, 1.0f, 1.0f);
  glBegin(GL_POINTS);
  for (auto p : controlPolygon) {
    glm::vec4 point = apply(m, p);
    glVertex3f(point.x, point.y, point.z);
  }
  glEnd();

  glColor3f(1.0f, 0.0f, 1.0f);
  glBegin(GL_LINE_STRIP);
	glLineWidth(4);
  for (auto p : bezierPoints) {
    glm::vec4 point = apply(m, p);
    glVertex3f(point.x, point.y, point.z);
  }
	glLineWidth(1);
  glEnd();

	glutSwapBuffers();
}

int currentViewPosition = 0;
void moveView(unsigned char key, int x, int y)
{
	if (currentViewPosition) {
		currentViewPosition += 10;
		
		if (currentViewPosition < bezierPoints.size())
			g_view = bezierPoints[currentViewPosition];
		else
			currentViewPosition = 1;

		draw();
		return;
	}

	controlPolygon = vector<glm::vec3>();

	glm::mat4 m = getTransformMatrix(g_eye, g_view);
	double H = glm::distance(g_eye, g_view);
	glm::mat4 perspective = getPerspectiveMatrix(H);
	m = m * perspective;

	for (auto curr :shapeData) {
		if (!isVisible(curr[0], curr[1], curr[2])) {
			continue;
		}

		glm::vec4 v1 = apply(m, curr[0]);
		glm::vec4 v2 = apply(m, curr[1]);
		glm::vec4 v3 = apply(m, curr[2]);
		
		controlPolygon.push_back(v1);
		controlPolygon.push_back(v2);
		controlPolygon.push_back(v3);
	}

	bezierPoints = bezier(controlPolygon, 100);
	currentViewPosition++;
	draw();
}

int main(int argc, char ** argv) 
{
	if (argc < 3) {
		cerr << "[program] [objfile] [eyeView] [polygon]" << endl;
		return -1;
	}

	limits.minP.x = limits.minP.y = 10000;
	limits.maxP.x = limits.maxP.y = -10000;

	ifstream eyeViewFile(argv[2], ios::out);
	g_view = readVec3(eyeViewFile);
	g_eye = readVec3(eyeViewFile);
	eyeViewFile.close();

	if (argc == 4) {
		ifstream polyFile(argv[3], ios::out);
		int n; polyFile >> n;
		while (n--) {
			controlPolygon.push_back(readVec3(polyFile));
		}
		bezierPoints = bezier(controlPolygon, 100);
		polyFile.close();
	}

	ifstream objFile(argv[1], ios::out);
	vector<glm::vec3> vertexData = readVertices(objFile);
	shapeData = readFaces(objFile, vertexData);
	objFile.close();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(sub_width, sub_height);
	glutInit(&argc, argv);
	glutCreateWindow("Zadatak 6");
	glutReshapeFunc(updateOnReshape);
	glutDisplayFunc(draw);
	glutKeyboardFunc(moveView);
	glutMainLoop();

	return 0;
}
