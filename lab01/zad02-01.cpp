#include <iostream>
using namespace std;

#include "zad02-bresenham.cpp"
#include <GL/glut.h>

typedef struct { int x, y; } Point;
typedef struct { Point p1, p2; } Input;
Input input;

GLuint window; 
GLuint sub_width = 256, sub_height = 256;

void drawLine(int x1, int y1, int x2, int y2) 
{
    glBegin(GL_LINES);
    
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);

    glEnd();
}

void updateOnReshape(int width, int height)
{
	sub_width = width;
	sub_height = height;

	glViewport(0, 0, sub_width, sub_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, sub_width - 1, 0, sub_height - 1);
	glMatrixMode(GL_MODELVIEW);
}


void draw() 
{
	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(1.0);

	glColor3f(1.0f, 0.0f, 0.0f);
	callBresenham(input.p1.x, input.p1.y, input.p2.x, input.p2.y);

	glColor3f(0.0f, 0.0f, 1.0f);
	drawLine(input.p1.x, input.p1.y + 20, input.p2.x, input.p2.y + 20);

	glutSwapBuffers();
}


void getInput() 
{
	cin >> input.p1.x >> input.p1.y;
	cin >> input.p2.x >> input.p2.y;
}

bool isDrawing = false;
Point mouseCached;

void expectNewInputOnMouseClick(int button, int state, int x, int y)
{
	y = sub_height - y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (isDrawing) {
			input.p1.x = mouseCached.x;
			input.p1.y = mouseCached.y;
			input.p2.x = x;
			input.p2.y = y;

			cout << input.p1.x << " " << input.p1.y << endl;
			cout << input.p2.x << " " << input.p2.y << endl;

			isDrawing = false;
			glutPostRedisplay();
		} else {
			isDrawing = true;
			mouseCached.x = x;
			mouseCached.y = y;
		}
	}
}


int main(int argc, char **argv) 
{
	cout << "Unesite pocetne koordinate" << endl;
	getInput();
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(sub_width,sub_height);
	glutInitWindowPosition(100,100);
	glutInit(&argc, argv);
	window = glutCreateWindow("Zadatak 2 - Bresenhamov");

	glutMouseFunc(expectNewInputOnMouseClick);
	glutReshapeFunc(updateOnReshape);
	glutDisplayFunc(draw);

	glutMainLoop();
	return 0;
}
