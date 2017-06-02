#include <iostream>
#include <stdio.h>
#include <GL/glut.h>
#include <glm/vec3.hpp>
#include <vector>
#include <algorithm>
#include "common.cpp"
using namespace std;

GLuint g_width, g_height;

typedef struct { double re, im; } Complex;
Complex g_const;

typedef struct { double min, max; } Dimension;
Dimension g_u, g_v;

double g_epsilon;
int g_max_iterations;

int divergence_test_julia(double, double, int);
int divergence_test_mandelbrot(double, double, int);
int (*active_divergence_test)(double, double, int) = divergence_test_mandelbrot;

int divergence_test(Complex z, Complex c, int limit) {
  double _re = z.re, _im = z.im;
	double re = c.re, im = c.im;

  for (int i = 1; i <= limit; i++) {
    double next_re = _re*_re - _im*_im + re;
    double next_im = 2*_re*_im + im;
    _re = next_re;
    _im = next_im;

    if (_re * _re + _im * _im > g_epsilon * g_epsilon) 
			return i;
  }

  return -1;
}

int divergence_test_julia(double re, double im, int limit) 
{
	return divergence_test({.re=re, .im=im}, g_const, limit);	
}

int divergence_test_mandelbrot(double re, double im, int limit)
{
	return divergence_test({.re=0, .im=0}, {.re=re, .im=im}, limit);
}

void color_pixel(glm::vec3 color) { glColor3f(color.r, color.g, color.b); }

glm::vec3 determine_color(int n) 
{
	if (n == -1) return glm::vec3(0, 0, 0);
	return glm::vec3(1.0 * n / g_max_iterations, 
								   1.0 - n / g_max_iterations / 2.0, 
									 0.8 - n / g_max_iterations / 3.0);
}

double complex_normalize(Dimension complex_d, double value)
{
  return (double)(complex_d.max - complex_d.min) * value / g_width + complex_d.min;
}

void display() 
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPointSize(1);
  glBegin(GL_POINTS);

  for (int y = 0; y <= g_height; y++) {
    for (int x = 0; x <= g_width; x++) {
      double u = complex_normalize(g_u, x);
      double v = complex_normalize(g_v, y);
      int n = active_divergence_test(u, v, g_max_iterations);

			color_pixel(determine_color(n));
      glVertex2i(x, y);
    }
  }

	glEnd();
  glutSwapBuffers();
}

void toggle_julia_mandelbrot(unsigned char key, int x, int y) {
  if (key == 'n' || key == 'N') {
		active_divergence_test = active_divergence_test == divergence_test_julia ?
				divergence_test_mandelbrot : divergence_test_julia;

    glutPostRedisplay();
  }
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

void read_globals(FILE *f)
{
  fscanf(f, "%lf", &g_epsilon);
  fscanf(f, "%d", &g_max_iterations);
  fscanf(f, "%lf %lf %lf %lf", &g_u.min, &g_u.max, &g_v.min, &g_v.max);
  fscanf(f, "%lf %lf", &g_const.re, &g_const.im);

	fclose(f);
}

int main(int argc, char **argv) {
	assert_or_die(argc == 2, "Usage: program [input_file]");	
	read_globals(fopen(argv[1], "r"));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Mandelbrot");
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(toggle_julia_mandelbrot);
	glutSwapBuffers();
	glutMainLoop();

	return 0;
}

