/*************
	CTIS 164
 -------------
 STUDENT : Ilhan Bahadir Yavas
 SECTION : 04
 HOMEWORK: Homework #3
 -------------
 PROBLEMS: 
 * Rocket launcher doesn't follow plane when mouse stops moving after locking onto plane
 * After the mouse approaches the plane and starts to lock, if the mouse does not move at all, it does not go into a LOCKED state even if the lock time expires.
 -------------
 ADDITIONAL FEATURES:
 * Rocket launcher follows the mouse position
 * Visualized locking system
 * Thanks to the locking system, rocket firing does not occur before the rocket launcher is locked to the plane.
 * After the rocket launcher locks the plane, the rocket launcher's movable part, instead of following the mouse position, follows the plane.
 * Locking system shows the time remaining to lock onto the plane
 * Realistic optimized smoke effect
 * Cool explosion effect
 *
**************/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532

//Aircraft lock state 
#define IDLE 0
#define STARTLOCKING 1
#define LOCKED 2

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool fire = false;
int lock = IDLE; //Current lock state
float locktime = 5.0; // time required or remaining time to lock
int explodeX, explodeY; // explosion effect coordinate
bool explode = false; // explode state
int boom_tim = 50; // explode time

/* User defined Types and variables for the application */
#define FOV  30    // Field of View Angle
#define ENEMY_SPEED 10
#define PLAYER_SPEED 5

// variable for smoke effect
struct Point
{
	float x;
	float y;
	float A;
	float r;
};

typedef struct {
	double x, y;
} point_t;

// Rocket variables
typedef struct {
	vec_t pos;
	double angle;
	double radius;
	vec_t vel;
} rocket_t;

//Aircraft variables 
typedef struct {
	vec_t pos;
	vec_t vel;  //velocity
} plane_t;

point_t rpg = { -600,-300 };
rocket_t  R = { { -800, -500 } };
plane_t P = { { -800, 260 } };

//smoke effect initialize
Point* smokePoints = NULL; // Pointer to the array of captured points
int numSmokePoint = 0;

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void Ellipse(float cx, float cy, float rx, float ry, int num)
{
	float theta = 2 * 3.1415926 / float(num);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = 1;//we start at angle = 0 
	float y = 0;

	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num; ii++)
	{
		//apply radius and offset
		glVertex2f(x * rx + cx, y * ry + cy);//output vertex 

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}

// display text with variables.
void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void vertex(point_t P, vec_t Tr, double angle) {
	double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
	double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
	glVertex2d(xp, yp);
}

// Draw complex object
void drawRocket(rocket_t p)
{
	float angle = fabs(atan2(P.pos.y - p.pos.y ,P.pos.x - p.pos.x));

	glColor3f(0.5, 0.5, 1.0);
	glBegin(GL_QUADS);
	vertex({ 40, 5 }, p.pos, angle);
	vertex({ 40, -5 }, p.pos, angle);
	vertex({ 0, -5 }, p.pos, angle);
	vertex({ 0, 5 }, p.pos, angle);
	glEnd();
	glColor3f(1, 1, 1.0);
	glBegin(GL_TRIANGLES);
	vertex({ 40, 5 }, p.pos, angle);
	vertex({ 40, -5 }, p.pos, angle);
	vertex({ 60, 0 }, p.pos, angle);
	glEnd();

	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLES);
	vertex({ 12, 5 }, p.pos, angle);
	vertex({ 0, 5 }, p.pos, angle);
	vertex({ 0, 12 }, p.pos, angle);
	glEnd();

	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLES);
	vertex({ 12, -5 }, p.pos, angle);
	vertex({ 0, -5 }, p.pos, angle);
	vertex({ 0, -12 }, p.pos, angle);
	glEnd();
}

void drawAircraft(int x, int y)
{
	glColor3ub(127, 137, 147);
	glBegin(GL_POLYGON);
	glVertex2f(x - 138 / 5, y + 12 / 5);
	glVertex2f(x - 291 / 5, y - 16 / 5);
	glVertex2f(x - 348 / 5, y - 37 / 5);
	glVertex2f(x - 355 / 5, y - 59 / 5);
	glVertex2f(x - 285 / 5, y - 77 / 5);
	glVertex2f(x - 71 / 5, y - 77 / 5);
	glVertex2f(x + 72 / 5, y - 68 / 5);
	glVertex2f(x + 372 / 5, y - 47 / 5);
	glVertex2f(x + 382 / 5, y - 37 / 5);
	glVertex2f(x + 370 / 5, y - 26 / 5);
	glEnd();

	glColor3ub(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(x - 290 / 5, y - 16 / 5);
	glVertex2f(x - 235 / 5, y + 28 / 5);
	glVertex2f(x - 139 / 5, y + 10 / 5);
	glEnd();

	glColor3ub(60, 65, 70);
	glBegin(GL_POLYGON);
	glVertex2f(x + 82 / 5, y + 29 / 5);
	glVertex2f(x + 93 / 5, y + 34 / 5);
	glVertex2f(x + 195 / 5, y + 26 / 5);
	glVertex2f(x + 192 / 5, y - 40 / 5);
	glVertex2f(x + 88 / 5, y - 33 / 5);
	glVertex2f(x + 78 / 5, y - 27 / 5);
	glEnd();

	glColor3ub(50, 55, 60);
	glBegin(GL_POLYGON);
	glVertex2f(x + 195 / 5, y + 26 / 5);
	glVertex2f(x + 192 / 5, y - 40 / 5);
	glVertex2f(x + 233 / 5, y - 30 / 5);
	glVertex2f(x + 231 / 5, y + 13 / 5);
	glEnd();

	glColor3ub(50, 55, 60);
	glBegin(GL_POLYGON);
	glVertex2f(x + 295 / 5, y + 62 / 5);
	glVertex2f(x + 353 / 5, y + 62 / 5);
	glVertex2f(x + 374 / 5, y - 55 / 5);
	glVertex2f(x + 293 / 5, y - 78 / 5);
	glVertex2f(x + 272 / 5, y - 57 / 5);
	glEnd();
	Ellipse(x - 10, y - 57 / 5, 26, 6, 10);
}

void RPG(int x , int y) 
{
	float angle = atan2(rpg.y - y +40, rpg.x - x +10);
	//draw movable parts
	glColor3ub(143, 149, 123);
	glBegin(GL_QUADS);
	vertex({ -27, +43 }, { -605, -340 }, angle);
	vertex({ 139, +43 }, { -605, -340 }, angle);
	vertex({ 139, +62 }, { -605, -340 }, angle);
	vertex({ -27, +62 }, { -605, -340 }, angle);
	glEnd();

	glBegin(GL_QUADS);
	vertex({ -27, +23 }, { -605, -340 }, angle);
	vertex({ 139, +23 }, { -605, -340 }, angle);
	vertex({ 139, +42 }, { -605, -340 }, angle);
	vertex({ -27, +42 }, { -605, -340 }, angle);
	glEnd();

	//base
	glBegin(GL_POLYGON);
	glVertex2f(x-22,y-32);
	glVertex2f(x-22,y-54);
	glVertex2f(x-14,y-54);
	glVertex2f(x-14,y-41);
	glVertex2f(x+47,y-41);
	glVertex2f(x + 90, y - 32);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(x + 90, y - 32);
	glVertex2f(x + 47, y - 41);
	glVertex2f(x+47,y-54);
	glVertex2f(x+100,y-54);
	glVertex2f(x+100,y-46);
	glVertex2f(x+107,y-34);
	glVertex2f(x + 90, y - 32);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(x + 153, y - 24);
	glVertex2f(x + 90, y - 24);
	glVertex2f(x + 90, y - 32);
	glVertex2f(x + 107, y - 34);
	glVertex2f(x+141,y-34);
	glVertex2f(x + 165, y - 27);

	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(x+165,y-27);
	glVertex2f(x+181,y-27);
	glVertex2f(x+181,y);
	glVertex2f(x+153,y);
	glVertex2f(x+153,y-24);
	glEnd();

	glColor3ub(20, 20, 20);
	circle(x+2, y-57, 15);
	circle(x+32, y-57, 15);
	glColor3ub(143, 149, 123);
	circle(x + 2, y - 57, 7);
	circle(x + 32, y - 57, 7);


	glColor3ub(143, 149, 123);
	glBegin(GL_POLYGON);
	glVertex2f(x + 296, y - 49);
	glVertex2f(x + 258, y - 49);
	glVertex2f(x + 254, y - 42);
	glVertex2f(x + 228, y - 42);
	glVertex2f(x + 216, y - 20);
	glVertex2f(x + 248, y - 20);
	glVertex2f(x + 257, y - 9);
	glVertex2f(x + 281, y - 9);
	glVertex2f(x + 296, y - 30);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(x + 254, y - 42);
	glVertex2f(x + 228, y - 42);
	glVertex2f(x + 216, y - 20);
	glVertex2f(x + 248, y - 20);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(x + 228, y - 42);
	glVertex2f(x + 226, y - 46);
	glVertex2f(x + 216, y - 46);
	glVertex2f(x + 216, y - 20);
	glEnd();

	glColor3ub(72, 76, 68);
	glBegin(GL_POLYGON);
	glVertex2f(x + 226, y -57);
	glVertex2f(x + 276, y -57);
	glVertex2f(x + 296, y -49);
	glVertex2f(x + 258, y -49);
	glVertex2f(x + 254, y -42);
	glVertex2f(x + 228, y -42);
	glVertex2f(x + 226, y -46);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(x + 226, y - 57);
	glVertex2f(x + 226, y - 46);
	glVertex2f(x + 172, y -46);
	glVertex2f(x + 172, y -39);
	glVertex2f(x + 114, y -39);
	glVertex2f(x + 105, y -44);
	glVertex2f(x + 105, y - 57);
	glEnd();
	glColor3ub(60, 60, 60);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x + 226, y - 57);
	glVertex2f(x + 276, y - 57);
	glVertex2f(x + 296, y - 49);
	glVertex2f(x + 258, y - 49);
	glVertex2f(x + 254, y - 42);
	glVertex2f(x + 228, y - 42);
	glVertex2f(x + 226, y - 46);
	glVertex2f(x + 172, y - 46);
	glVertex2f(x + 172, y - 39);
	glVertex2f(x + 114, y - 39);
	glVertex2f(x + 105, y - 44);
	glVertex2f(x + 105, y - 57);
	glEnd();


	glColor3ub(20, 20, 20);
	circle(x + 124, y - 57, 15);
	circle(x + 154, y - 57, 15);
	circle(x + 242, y - 57, 15);
	glColor3ub(143, 149, 123);
	circle(x + 124, y - 57, 7);
	circle(x + 154, y - 57, 7);
	circle(x + 242, y - 57, 7);

	glColor3ub(170, 202, 211);
	glBegin(GL_POLYGON);
	glVertex2f(x + 271, y - 16);
	glVertex2f(x + 282, y - 16);
	glVertex2f(x + 288, y - 29);
	glVertex2f(x + 271, y - 29);
	glEnd();

	glLineWidth(2);
	glColor3ub(36, 43, 46);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x + 271, y - 16);
	glVertex2f(x + 282, y - 16);
	glVertex2f(x + 288, y - 29);
	glVertex2f(x + 271, y - 29);
	glEnd();

}

void Aircraft(int x, int y)//Draw lock system visualize
{
	drawAircraft(x, y);
	switch (lock)
	{
	case 0:
		break;
	case 1:
		glColor3ub(255, 255, 255);
		vprint(x-12, y+25, GLUT_BITMAP_HELVETICA_18, "%0.2f", locktime);

		glLineWidth(2);
		glBegin(GL_LINE_STRIP);
		glVertex2f(x - 10 * 8, y - 10);
		glVertex2f(x - 10 * 8, y - 10 * 2);
		glVertex2f(x - 10 * 7, y - 10 * 2);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex2f(x + 10 * 8, y - 10);
		glVertex2f(x + 10 * 8, y - 10 * 2);
		glVertex2f(x + 10 * 7, y - 10 * 2);
		glEnd();


		glBegin(GL_LINE_STRIP);
		glVertex2f(x - 10 * 8, y + 10);
		glVertex2f(x - 10 * 8, y + 10 * 2);
		glVertex2f(x - 10 * 7, y + 10 * 2);
		glEnd();


		glBegin(GL_LINE_STRIP);
		glVertex2f(x + 10 * 8, y + 10);
		glVertex2f(x + 10 * 8, y + 10 * 2);
		glVertex2f(x + 10 * 7, y + 10 * 2);
		glEnd();
		break;
	case 2:	
		glColor3ub(255, 0, 0);

		vprint(x - 20, y + 25, GLUT_BITMAP_HELVETICA_12, "LOCKED");

		glLineWidth(2);
		glBegin(GL_LINE_LOOP);
		glVertex2f(x - 10 * 8, y - 10 * 2);
		glVertex2f(x + 10 * 8, y - 10 * 2);
		glVertex2f(x + 10 * 8, y + 10 * 2);
		glVertex2f(x - 10 * 8, y + 10 * 2);
		glEnd();

		break;
	}
}

void smoke()
{
	for (int i = 0; i < numSmokePoint; ++i)
	{
		glColor4f(30, 30, 30, smokePoints[i].A);
		circle(smokePoints[i].x, smokePoints[i].y, smokePoints[i].r);
	}
}

void Explode(int x, int y)
{
	float angle;
	glColor3ub(255, 255, 60);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	for (int i = 0; i < 100; i++)
	{
		angle = 1 * PI * i / 100;
		glVertex2f(x + ((rand() % 50) + 40) * cos(angle), y + ((rand() % 50) + 40) * sin(angle));
	}
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	for (int i = 0; i < 100; i++)
	{
		angle = 1 * PI * -i / 100;
		glVertex2f(x + ((rand() % 50) + 40) * cos(angle), y + ((rand() % 50) + 40) * sin(angle));
	}
	glEnd();

	glColor3ub(255, 60, 60);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	for (int i = 0; i < 100; i++)
	{
		angle = 1 * PI * i / 100;
		glVertex2f(x + ((rand() % 20) + 40) * cos(angle), y + ((rand() % 20) + 40) * sin(angle));
	}
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	for (int i = 0; i < 100; i++)
	{
		angle = 1 * PI * -i / 100;
		glVertex2f(x + ((rand() % 20) + 40) * cos(angle), y + ((rand() % 20) + 40) * sin(angle));
	}
	glEnd();
}

void background() //draw background
{
	glColor3ub(0, 0, 120);
	glRectf(winWidth, winHeight, -winWidth, -250);
	glColor3ub(70, 79, 0);
	glRectf(winWidth, -250, -winWidth, -winHeight);
}

void display() //display all object
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	background();
	drawRocket(R);
	RPG(-600,-300);
	Aircraft(P.pos.x, P.pos.y);
	smoke();
	if (explode)
		Explode(explodeX, explodeY);
	else
		Explode(1000, 1000);	
	glColor3f(1, 0, 0);
	vprint(-100, winHeight / 2 - 50, GLUT_BITMAP_HELVETICA_18, "DESTROY ENEMY AIRCRAFT");
	glColor3f(0, 0, 0);
	vprint(-200, -winHeight / 2 + 70, GLUT_BITMAP_HELVETICA_18, "In order to fire, you must first come over the plane with the mouse and follow it until it locks.");
	vprint(-200, -winHeight / 2 + 50, GLUT_BITMAP_HELVETICA_18, "Once you've locked onto the plane, you can fire it whenever you want.");

	glutSwapBuffers();
}

bool checkCircle(float px, float py, float cx, float cy, float R)
{
	float dx = (px - cx);
	float dy = (py - cy);
	float d = sqrt(dx * dx + dy * dy);
	return d <= R;
}

void Locking(int x, int y )//change locking state
{
	if (lock != LOCKED) // if already locked state dont change anything
	{
		if (checkCircle(x, y, P.pos.x, P.pos.y, 70))//check the distance between mouse position and plane position
		{
			lock = STARTLOCKING; // first set locking state STARTLOCKING while lock time end
			if (locktime <= 0.0)// if locking time reach to end 
			{
				lock = LOCKED; // set lock state
				// clear all smoke effect before fire for optimization
				free(smokePoints); 
				smokePoints = NULL; 
				numSmokePoint = 0;
			}
		}
		else // if mouse leave plane reset state and lock time 
		{
			lock = IDLE;
			locktime = 1.0;
		}
	}

}

void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	glutPostRedisplay();
}

void onSpecialKeyDown(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	glutPostRedisplay();
}

void onSpecialKeyUp(int key, int x, int y)
{

	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;

	}

	glutPostRedisplay();
}

void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMove(int x, int y) 
{
	//change rpg target location based on which lock is in what state
	if (lock != LOCKED)
	{
		rpg.x = x - WINDOW_WIDTH / 2;
		rpg.y = WINDOW_HEIGHT / 2 - y;
	}
	else
	{
		rpg.x = P.pos.x;
		rpg.y = P.pos.y;
	}

	//set mouse variable for lock system
	int x2 = x - winWidth / 2;
	int	y2 = winHeight / 2 - y;

	Locking(x2, y2);

	glutPostRedisplay();
}

void onClick(int button, int stat, int x, int y)
{
	if (fire == false && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
	{
		// fire only if the status of the lock is LOCKED
		if (lock == LOCKED)
		{
			R = { { -620, -300 } };
			fire = true;
		}
	}



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void movePlayer(rocket_t * p, double speed) 
{
	double dx = speed * cos(p->angle * D2R);
	double dy = speed * sin(p->angle * D2R);
	vec_t oldPos = p->pos;
	p->pos = { p->pos.x + dx, p->pos.y + dy };
}

void turnPlayer(rocket_t* p, double turn) {
	p->angle += turn;
	if (p->angle < 0) p->angle += 360;
	if (p->angle >= 360) p->angle -= 360;
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	if (lock == STARTLOCKING)//timer for locking 
	{
		locktime -= 0.017;
	}
	//initialize smoke points
	Point capturedPoint;
	if (fire)
	{
		if (!checkCircle(R.pos.x, R.pos.y, P.pos.x, P.pos.y, 50))
		{
			R.vel = mulV(ENEMY_SPEED, unitV(subV(P.pos, R.pos)));
			R.pos = addV(R.pos, R.vel);

			float posX = R.pos.x;
			float posY = R.pos.y;

			// Apply transformation to the captured point
			capturedPoint.x = posX + (rand() % 20);
			capturedPoint.y = posY + (rand() % 20);
			capturedPoint.A = 1.0;
			capturedPoint.r = 10;

			// Allocate memory for a new point
			Point* newCapturedPoints = (Point*)malloc((numSmokePoint + 1) * sizeof(Point));
			// Copy the existing points to the new memory block
			for (int i = 0; i < numSmokePoint; ++i)
			{
				newCapturedPoints[i] = smokePoints[i];
			}
			// Add the new point to the end
			newCapturedPoints[numSmokePoint] = capturedPoint;

			// Free the memory occupied by the old points array
			free(smokePoints);

			// Update the pointer to the new points array
			smokePoints = newCapturedPoints;

			// Increment the number of captured points
			numSmokePoint++;

		}
		else
		{
			//if rocket reach the plane start explosion effect and reset game
			explode = true;
			explodeX = P.pos.x;
			explodeY = P.pos.y;
			R = { { -800, -500 } };
			fire = false;
			P.pos.x = 800;
			P.pos.y = (rand() % 400) - 50;
			lock = IDLE;

		}

	}
	//explosion effect time initialize
	if (explode)
	{
		if (boom_tim == 0)
		{
			explode = false;
			boom_tim = 50;
		}
		else
		{
			boom_tim -= 1;
			explode = true;
		}
	}

	//apply effect for all smoke point
	for (int i = 0; i < numSmokePoint; ++i)
	{
		smokePoints[i].A -= 0.01;
		smokePoints[i].r += 1;
	}

	//if plane reach end of the frame reset position
	if (P.pos.x < -WINDOW_WIDTH / 2 - 200)
	{
		P.pos.x = WINDOW_WIDTH / 2 + 100;
		lock = IDLE;
	}
	else
	{
		P.pos.x -= 4;
	}

	glutPostRedisplay();

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}

void main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Ilhan Bahadir Yavas");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}