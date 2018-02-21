#include <iostream>
#include <vector>
#include <GL/glut.h>
#include "structs.cpp"

using namespace std;

GLuint window; 
GLuint sub_width = 256, 
			 sub_height = 256;

bool fillPoly = false;
int nPoints = 0;
int verticesIndex = 0;
vector<Point> vertices;
vector<Edge> edgeVectors;

struct {
	Point minP;
	Point maxP;
} limits;

bool isInPolygon(int x, int y) 
{
	for (int i = 0; i < edgeVectors.size(); i++) {
		Edge e = edgeVectors[i];

		if (e.a * x + e.b * y + e.c > 0) {
			return false;
		}
	}

	return true;
}

void outputVertexState(int x, int y) 
{
	cout << (isInPolygon(x, y) ? "unutar" 
													   : "izvan") << endl;
}

void updateLimits(Point p) {
	limits.maxP.x = max(p.x, limits.maxP.x);
	limits.maxP.y = max(p.y, limits.maxP.y);
	limits.minP.x = min(p.x, limits.minP.x);
	limits.minP.y = min(p.y, limits.minP.y);
}

void updateEdgeVectors(int index) 
{
	if (index < 0) return;

	bool reachedLastVertex = index == nPoints - 2;
	if (reachedLastVertex) updateEdgeVectors(index + 1);
	
	Edge *curr = &edgeVectors[index];
	Point *i = &vertices[index];
	Point *iPlusOne = &vertices[(index + 1) % edgeVectors.size()];

	curr->a = i->y - iPlusOne->y;
	curr->b = -i->x + iPlusOne->x;
	curr->c = i->x * iPlusOne->y - iPlusOne->x * i->y;
}

void addPolygonPoint(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		y = sub_height - y;

		if (verticesIndex == vertices.size()) {
			outputVertexState(x, y);
			return;
		}

		Point p = { .x = x, .y = y };
		vertices[verticesIndex++] = p;
		updateLimits(p);
		updateEdgeVectors(verticesIndex - 2);

		glutPostRedisplay();
	}
}

void paintPolygon() 
{
	glBegin(GL_LINES);

	for(int y = limits.minP.y; y < limits.maxP.y; y++) {
		int L = limits.minP.x;
		int D = limits.maxP.x;

		for (int i = 0, i0 = nPoints - 1; i < vertices.size(); i0=i++) {
			bool isHorizontal = edgeVectors[i0].a != 0;
			if (isHorizontal) {
				double x = (-edgeVectors[i0].b * y - edgeVectors[i0].c) / (double)edgeVectors[i0].a;

				if (vertices[i0].y < vertices[i].y) 
					L = max(L, (int)x);
				else if (vertices[i0].y >= vertices[i].y) 
					D = min(D, (int)x);
			}
		}

		if (L < D) {
			glVertex2i(L, y);
			glVertex2i(D, y);
		}
	}

	glEnd();
}

int getNumPoints() {
	cin >> nPoints;
	return nPoints;
}

void resetOrColorKeyboardEvent(unsigned char key, int x, int y)
{
	switch(key) {
		case 'r':
			verticesIndex = 0;
			fillPoly = false;
			glutPostRedisplay();
			break;
		case 'o':
			fillPoly = !fillPoly;
			glutPostRedisplay();
			break;
	}
}

void updateOnReshape(int w, int h) {
	sub_width = w;
	sub_height = h;

	glViewport(0, 0, sub_width, sub_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, sub_width - 1, 0, sub_height - 1);
	glMatrixMode(GL_MODELVIEW);
}

void draw() {
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(1.0);

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < verticesIndex; i++) {
		Point mp = vertices[i];
    glVertex2i(mp.x, mp.y);
	}

	glEnd();

	if (fillPoly) {
		cout << "Filling poly" << endl;
		paintPolygon();
	}

	glutSwapBuffers();
}

int main(int argc, char **argv) 
{
	int n = getNumPoints();
	vertices = vector<Point>(n);
	edgeVectors = vector<Edge>(n);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(sub_width,sub_height);
	glutInit(&argc, argv);

	window = glutCreateWindow("Zadatak 3");

	glutMouseFunc(addPolygonPoint);
	glutKeyboardFunc(resetOrColorKeyboardEvent);
	glutReshapeFunc(updateOnReshape);
	glutDisplayFunc(draw);

	glutMainLoop();
	return 0;
}
