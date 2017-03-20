#include <GL/glut.h>

void swap(int *fst, int *snd) 
{
    int x = *fst;
    *fst  = *snd;
    *snd = x;
}

const int DRAW_MODE_UP = 1;
const int DRAW_MODE_DOWN = -1;
void turnOnRegular(int x, int yc) { glVertex2i(x, yc); }
void turnOnSwapped(int x, int yc) { glVertex2i(yc, x); }

void draw(int x1, int y1,
           int x2, int y2,
           void(*vertexTurnOnFunc)(int, int),
           int draw_mode) 
{

    glBegin(GL_POINTS);

    int a = 2 * (y2 - y1);
    int yf = -draw_mode * (x2 - x1); // -draw_mode
    int correction = -draw_mode * 2 * (x2 - x1); // -draw_mode

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

void bresenhamUpwards(int x1, int y1, 
											int x2, int y2) 
{
    void(*turnOnFn)(int, int) = turnOnRegular;

    if(y2 - y1 > x2 - x1) {
        turnOnFn = turnOnSwapped;
        swap(x2, y2);
        swap(x1, y1);
    }

    draw(x1, y1, x2, y2, turnOnFn, DRAW_MODE_UP);
}

void bresenhamDownwards(int x1, int y1, 
												int x2, int y2) 
{
    void(*turnOnFn)(int, int) = turnOnRegular;

    if (-(y2 - y1) > x2 - x1) {
        turnOnFn = turnOnSwapped;
        swap(x2, y1);
        swap(x1, y2);
    }

    draw(x1, y1, x2, y2, turnOnFn, DRAW_MODE_DOWN);
}


void callBresenham(const int x1, const int y1, 
                   const int x2, const int y2) 
{

    if (x1 <= x2) {
        if (y1 <= y2) {
            bresenhamUpwards(x1, y1, x2, y2);
        } else {
            bresenhamDownwards(x1, y1, x2, y2);
        }
    } else {
        if (y1 >= y2) {
            bresenhamUpwards(x2, y2, x1, y1);
        } else {
            bresenhamDownwards(x2, y2, x1, y1);
        }
    }
}

