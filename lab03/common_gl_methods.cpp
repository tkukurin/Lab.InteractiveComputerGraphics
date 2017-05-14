#include <GL/glut.h>
#include <glm/vec3.hpp>

GLuint sub_width = 256, 
			 sub_height = 256;

void updateOnReshape(int w, int h) 
{
	sub_width = w;
	sub_height = h;

	glViewport(0, 0, sub_width, sub_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	const double normalization = 200.0;
	gluOrtho2D(-w / normalization, w / normalization, 
						 -h / normalization, h / normalization);
}

void glVertex4v(glm::vec4 v)
{
	glVertex3f(v.x / v.w, v.y / v.w, v.z / v.w);
}


