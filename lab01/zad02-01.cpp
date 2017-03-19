#include <iostream>
using namespace std;

#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL/glut.h>

typedef struct { int x, y; } Point;
typedef struct { int x1, y1, x2, y2; } Input;
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

void swap(int *fst, int *snd) 
{
    int x = *fst;
    *fst  = *snd;
    *snd = x;
}

const int DRAW_MODE_2 = 1;
const int DRAW_MODE_3 = -1;
void turnOnRegular(int x, int yc) { glVertex2i(x, yc); }
void turnOnSwapped(int x, int yc) { glVertex2i(yc, x); }

void draw(int x1, int y1,
           int x2, int y2,
           void(*vertexTurnOnFunc)(int, int),
           int draw_mode) 
{

    glBegin(GL_POINTS);

    int a = 2 * (y2 - y1);
    //int yc = y1;
    int yf = -draw_mode * (x2 - x1);
    int correction = -draw_mode * 2 * (x2 - x1);

    for (int x = x1, yc = y1; x <= x2; x++) {
        vertexTurnOnFunc(x, yc);
        yf = yf + a;

        if (draw_mode * yf >= 0) {
            yf = yf + correction;
            yc = yc + draw_mode;
        }
    }

    glEnd();
}

void bresenham2(int x1, int y1, 
                int x2, int y2) 
{
    void(*turnOnFn)(int, int) = turnOnRegular;

    if(y2 - y1 > x2 - x1) {
        turnOnFn = turnOnSwapped;
        swap(x2, y2);
        swap(x1, y1);
    }

    draw(x1, y1, x2, y2, turnOnFn, DRAW_MODE_2);
}

void bresenham3(int x1, int y1, 
                int x2, int y2) 
{
    void(*turnOnFn)(int, int) = turnOnRegular;

    if (-(y2 - y1) > x2 - x1) {
        turnOnFn = turnOnSwapped;
        swap(x2, y1);
        swap(x1, y2);
    }

    draw(x1, y1, x2, y2, turnOnFn, DRAW_MODE_3);
}


void callBresenham(const int x1, const int y1, 
                   const int x2, const int y2) 
{

    if (x1 <= x2) {
        if (y1 <= y2) {
            bresenham2(x1, y1, x2, y2);
        } else {
            bresenham3(x1, y1, x2, y2);
        }
    } else {
        if (y1 >= y2) {
            bresenham2(x2, y2, x1, y1);
        } else {
            bresenham3(x2, y2, x1, y1);
        }
    }
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
    callBresenham(input.x1, input.y1, input.x2, input.y2);

    glColor3f(0.0f, 0.0f, 1.0f);
    drawLine(input.x1, input.y1 + 20, input.x2, input.y2 + 20);

	glutSwapBuffers();
}


void getInput() 
{
    cin >> input.x1 >> input.y1;
    cin >> input.x2 >> input.y2;
}

bool isDrawing = false;
Point mouseCached;
void expectNewInputOnMouseClick(int button, int state, int x, int y)
{
	y = sub_height - y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (isDrawing) {
			input.x1 = mouseCached.x;
			input.y1 = mouseCached.y;
			input.x2 = x;
			input.y2 = y;
			cout << input.x1 << " " << input.y1 << endl;
			cout << input.x2 << " " << input.y2 << endl;
			isDrawing = false;
			glutPostRedisplay();
		} else {
			isDrawing = true;
			mouseCached.x = x;
			mouseCached.y = y;
		}

		//getInput();
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
