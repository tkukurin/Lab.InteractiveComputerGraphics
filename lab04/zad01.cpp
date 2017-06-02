#include <stdio.h>
#include <GL/glut.h>
#include <vector>
#include <map>
#include <algorithm>

#include "parsing.cpp"
#include "common.cpp"

GLuint g_width, g_height;
bool g_color_type = false;

glm::vec3 g_eye(5, 5, 5);
glm::vec3 g_lightSource(3, 3, 3);
glm::vec3 g_up(0, 1, 0);
glm::vec3 g_view(0, 0, 0);

vector<glm::mat3> g_shapeData;
vector<glm::vec4> g_planeEquations;
vector<glm::vec3> g_points;
map<int, vector<glm::vec3>> g_indexToNormals;

glm::vec3 triangleCenter(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) { return (v1 + v2 + v3) * (1.0f / 3.0f); }

glm::vec3 triangleNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) { return glm::cross(v3 - v1, v2 - v1); }

int indexof(glm::vec3 v, vector<glm::vec3> points) { return std::distance(points.begin(), find(points.begin(), points.end(), v)); }

map<int, vector<glm::vec3>> neighboringPolyNormals(vector<glm::mat3> &polygons, vector<glm::vec3> points) {
	map<int, vector<glm::vec3>> normals;

	auto merge = [&normals](int fst, glm::vec3 value) {
		if (!normals.count(fst)) normals[fst] = vector<glm::vec3>();
		normals[fst].push_back(value);
	};

	for (int i = 0; i < polygons.size(); i++) {
		glm::mat3 curr = polygons[i];
		glm::vec3 v1 = curr[0];
		glm::vec3 v2 = curr[1];
		glm::vec3 v3 = curr[2];
		glm::vec3 center = glm::normalize(triangleNormal(v1, v2, v3));
		int loc1 = indexof(v1, points);
		int loc2 = indexof(v2, points);
		int loc3 = indexof(v3, points);
		merge(loc1, center);
		merge(loc2, center);
		merge(loc3, center);
	}

	return normals;
}

void updatePerspective()
{
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(45.0, (float)g_width / g_height, 1.0, 20.0);
	gluLookAt (g_eye.x, g_eye.y, g_eye.z, 
					   g_view.x, g_view.y, g_view.z, 
						 g_up.x, g_up.y, g_up.z);
}

void myReshape (int w, int h)
{
	g_width = w;
	g_height = h;

	glViewport (0, 0, g_width, g_height); 
	updatePerspective();
}

bool isVisible(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 eye)
{
		glm::vec3 n = triangleNormal(v1, v2, v3);
    glm::vec3 c = triangleCenter(v1, v2, v3);
    glm::vec3 np = eye - c;

    return glm::dot(n, np) >= 0;
}

float getMaxDistance(glm::vec3 maxVec, glm::vec3 minVec) {
	glm::vec3 result = maxVec - minVec;
	return max(max(result.x, result.y), result.z);
}

double intensity(glm::vec3 normal, glm::vec3 light)
{
	double Ia = 150;
	double ka = 0.5;

	double Ii = 150;
	double ki = 0.5;

	glm::vec3 normalNormalized = glm::normalize(normal);
	glm::vec3 lightNormalized = glm::normalize(g_lightSource - normal);
	double LN = glm::dot(normalNormalized, lightNormalized);

	return Ia * ka + Ii * ki * max(LN, 0.0d);
}

void color(glm::vec3 vec, glm::vec3 color) {
	color = glm::vec3(1, 1, 1);
	double totalComponent = intensity(vec, g_lightSource); 
	
	glColor3ub(color.r * totalComponent, color.g * totalComponent, color.b * totalComponent);
}

glm::vec3 meanNormal(vector<glm::vec3> normals) {
	glm::vec3 total;

	for (auto v: normals)
		total += v;

	return total * (1.0f / normals.size());
}

void colorModelWire(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 colorComponents)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
}

void colorModelConstant(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 colorComponents)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLES);
	color(triangleCenter(v1, v2, v3), colorComponents);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
}

void colorModelGouraud(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 colorComponents)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLES);
	color(meanNormal(g_indexToNormals[indexof(v1, g_points)]), colorComponents);
	glVertex3f(v1.x, v1.y, v1.z);

	color(meanNormal(g_indexToNormals[indexof(v2, g_points)]), colorComponents);
	glVertex3f(v2.x, v2.y, v2.z);

	color(meanNormal(g_indexToNormals[indexof(v3, g_points)]), colorComponents);
	glVertex3f(v3.x, v3.y, v3.z);
}

typedef void (*ColorModel)(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
vector<ColorModel> colorModels;
int colorModelIndex = 0;

void drawCoordinateSystem()
{
	glColor3f(200, 0, 0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);
	glVertex3f(0, 0, 0); glVertex3f(0, 10, 0);
	glVertex3f(0, 0, 0); glVertex3f(0, 0, 10);
	glEnd();
}

void drawLight()
{
	glColor3f(200, 0, 0);
	glPointSize(15.0);
	glBegin(GL_POINTS);
	glVertex3f(g_lightSource.x, g_lightSource.y, g_lightSource.z);
	glEnd();
}	

void myObject ()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glm::vec3 means = (limits.maxP + limits.minP) * 0.5f;
	glTranslatef(-means.x, -means.y, -means.z);

	float maxDiff = getMaxDistance(limits.maxP, limits.minP);
	glScalef(2.0 / maxDiff, 2.0 / maxDiff, 2.0 / maxDiff);

	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawCoordinateSystem();
	drawLight();
	
	glPointSize(1.0);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (int i = 0; i < g_shapeData.size(); i++) {
		glm::mat3 curr = g_shapeData[i];
		glm::vec3 v1 = curr[0];
		glm::vec3 v2 = curr[1];
		glm::vec3 v3 = curr[2];

		if (!isVisible(v1, v2, v3, g_eye)) {
			continue;
		}

		const glm::vec3 colorComponents(125, 125, 125);
		colorModels[colorModelIndex](v1, v2, v3, colorComponents);

		glEnd();
	}

}

void myDisplay(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		         
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myObject ();
	glutSwapBuffers();      
}

void myMouse(int button, int state, int x, int y)
{
	bool isLeft = button == GLUT_LEFT_BUTTON;
	bool isScroll = button == 3 || button == 4;

	if (isScroll && state == GLUT_DOWN) {
		glm::vec3 vec_move = g_eye / glm::length(g_eye);
	 	g_eye = g_eye + (button == 3 ? -1.0f : 1.0f) * vec_move;
	} 

	updatePerspective();
	glutPostRedisplay();
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
	switch (theKey) 
	{
		case 'l': g_eye.x = g_eye.x+0.1;
        break;

		case 'k': g_eye.x =g_eye.x-0.1;
        break;

		case 'r': g_eye.x=0.0;
        break;

		case 'c': 
				colorModelIndex = (colorModelIndex + 1) % colorModels.size();
				break;
	}

	updatePerspective();
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	assert_or_die(argc == 2, "Usage: program [object.obj]");

	colorModels.push_back(&colorModelWire);
	colorModels.push_back(&colorModelConstant);
	colorModels.push_back(&colorModelGouraud);

	limits.minP.x = limits.minP.y = limits.minP.z = 10000;
	limits.maxP.x = limits.maxP.y = limits.maxP.z = -10000;

	ifstream objFile(argv[1], ios::out);
	g_points = readVertices(objFile);
	g_shapeData = readFaces(objFile, g_points);
	g_indexToNormals = neighboringPolyNormals(g_shapeData, g_points);
	objFile.close();

	glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInit(&argc, argv);
   
	glutCreateWindow ("Tijelo");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	printf("Tipka: l - pomicanje ocista po x os +\n");
	printf("Tipka: k - pomicanje ocista po x os -\n");
	printf("Tipka: r - pocetno stanje\n");
	printf("esc: izlaz iz programa\n");

	glutMainLoop();
	return 0;
}
