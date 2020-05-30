#include <GL/glut.h>
#include <bevgrafmath2017.h>
#include <math.h>


GLsizei winWidth = 800, winHeight = 600;

int  n = 9;
int counter = 0;

vec2 points[10];

GLint dragged = -1;

void init() {
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glShadeModel(GL_FLAT);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0);
	glLineWidth(1.0);
}

double binomialCoefficient(int n, int k)
{
	if (k > n - k)
		k = n - k;

	double c = 1.0;

	for (int i = 0; i < k; i++)
	{
		c = c * (n - i);
		c = c / (i + 1);
	}
	return c;
}

void drawBezierWithBernstein() {

	//Draw control points
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < counter; i++) {
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();

	//Dont draw the curve until all the points are added
	if (counter < n + 1)
		return;

	//Draw lines connecting control points
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < n + 1; i++)
		glVertex2f(points[i].x, points[i].y);
	glEnd();

	glColor3f(1.0, 0.0, 1.0);

	glBegin(GL_LINE_STRIP);
	for (double t = 0.0; t < 1.0; t += 0.01) {
		vec2 curvePoint = { 0.0, 0.0 };
		for (int i = 0; i <= n; i++)
		{
			double biCoeff = binomialCoefficient(n, i);
			double oneMinusT = pow(1 - t, n - i);
			double powT = pow(t, i);
			curvePoint.x += biCoeff * oneMinusT * powT * points[i].x;
			curvePoint.y += biCoeff * oneMinusT * powT * points[i].y;
		}
		glVertex2f(curvePoint.x, curvePoint.y);
	}

	glVertex2f(points[n].x, points[n].y);
	glEnd();

}

void deboor() {


	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < counter; i++) {
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();

	if (counter < n + 1)
		return;


	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < n + 1; i++)
		glVertex2f(points[i].x, points[i].y);
	glEnd();

	//int u[14]; // n + p + 1 + 1
	//m = n + p + 1 + 1

	//degree of curve
	//int p = 3;
	int p = 5;

	//int u[14] = { 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7 }; //knot vector arrray  non-uniform
	int u[16] = { 0,0,0,0,0,0,1,2,3,4,7,7,7,7,7,7 }; // more curvy 
	




	//float u[] = {1,1,1,1,5,6,7,8,9,10,11,11,11,11}; 

	vec2* helperPoints = new vec2[n + 1];; 
	for (int i = 0; i < n + 1; i++) 
		helperPoints[i] = points[i];

	glColor3f(0.0, 0.0, 1.0);
	float a = 0;

	glBegin(GL_LINE_STRIP);
	for (int i = p; i <= n; i++) {

		for (float t = u[i]; t < u[i + 1]; t += 0.01) {

			for (int j = i - p; j <= i; j++) 
				helperPoints[j] = points[j];

			for (int r = 1; r <= p; r++) { 

				vec2* helperPointsnew = new vec2[n + 1];

				for (int j = i - p + r; j <= i; j++) {
					a = (t - u[j]) / (u[j + p - r + 1] - u[j]);
					vec2 newPoint =
					{ (1 - a) * helperPoints[j - 1].x + a * helperPoints[j].x,
					(1 - a) * helperPoints[j - 1].y + a * helperPoints[j].y
					};
					helperPointsnew[j] = newPoint;
				}
				helperPoints = helperPointsnew;
			}

			glVertex2f(helperPoints[i].x, helperPoints[i].y);

		}
	}
	glVertex2f(points[n].x, points[n].y);
	glEnd();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 0.0, 0.0);

	drawBezierWithBernstein();

	deboor();


	glutSwapBuffers();
}

GLint getActivePoint1(vec2 p[], GLint size, GLint sens, GLint x, GLint y) {
	GLint i, s = sens * sens;
	vec2 P = { (float)x, (float)y };

	for (i = 0; i < size; i++)
		if (dist(p[i], P) < s)
			return i;
	return -1;
}

void processMouse(GLint button, GLint action, GLint xMouse, GLint yMouse) {
	GLint i;


	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN && counter < n + 1) {
		vec2 mouse = vec2(xMouse, winHeight - yMouse);
		points[counter] = mouse;
		counter++;
		glutPostRedisplay();
	}

	if (counter == n + 1 && (i = getActivePoint1(points, n + 1, 5, xMouse, winHeight - yMouse)) != -1)
		dragged = i;
	if (button == GLUT_LEFT_BUTTON && action == GLUT_UP)
		dragged = -1;


}

void processMouseActiveMotion(GLint xMouse, GLint yMouse) {
	GLint i;

	if (dragged >= 0) {
		points[dragged].x = xMouse;
		points[dragged].y = winHeight - yMouse;
		glutPostRedisplay();
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Computer Graphics");
	init();
	glutDisplayFunc(display);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutMainLoop();
	return 0;
}