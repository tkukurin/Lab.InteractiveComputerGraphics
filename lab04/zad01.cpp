#include <stdio.h>
#include <GL/glut.h>
#include <vector>
#include <map>
#include <algorithm>
#include <random>

#include "parsing.cpp"
#include "common.cpp"

GLuint g_width, g_height;
bool g_color_type = false;

void render() {

	int w = g_width;
	int h = g_height;
	double scale = 4.0;
	double scale2 = scale;

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int limit = 25;
	glPointSize(1);
	glColor3f(0.0f, 0.7f, 0.3f);
	glBegin(GL_POINTS);
	double x0, y0;
	for(int i = 0; i < 20000; i++) {
		x0 = 0;
		y0 = 0;
		for(int j = 0; j < limit; j++) {
			double x, y;
			int p = rand() % 100;
			x = 0.5 * x0;
			y = 0.5 * y0;

			if (p >= 33 && p < 66) {
				x += 1.28;
			} else if (p > 66) {
				x += 0.64;
				y += 0.8;
			}
			x0 = x; y0 = y;
		}

		glVertex2i( (int)(x0 * (w/scale)), (int)(y0 * (h / scale)));
	}

	glEnd();
	glutSwapBuffers();
}

void resize(int width, int height) {
  g_width = width;
  g_height = height;

  glViewport(0, 0, g_width, g_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, g_width, 0, g_height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

  glutPostRedisplay();
}

int main(int argc, char** argv)
{

	glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE);
	glutInit(&argc, argv);
   
	glutCreateWindow ("Tijelo");
	glutReshapeFunc(resize);
	glutDisplayFunc(render);
	glutMainLoop();
	return 0;
}
