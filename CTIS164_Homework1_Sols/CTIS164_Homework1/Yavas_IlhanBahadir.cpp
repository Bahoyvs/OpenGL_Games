/*************
	CTIS 164 
 -------------
 STUDENT : Ilhan Bahadir Yavas
 SECTION : 04
 HOMEWORK: Homework #1
 -------------
 PROBLEMS: Exploiton effect has a weird bug.
 -------------
 ADDITIONAL FEATURES: 
 * The camera can switch between the thermal camera and the normal camera with arrow keys.
 * You can destroy tanks with mouse click.
 * Aiming Cursor resets its position when the mouse leaves the designated area and can lock onto the tank.
**************/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



/* TEMPLATE CONSTANTS AND VARIABLES */
#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define  TIMER_PERIOD  17 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer


/* USER GLOBAL VARIABLES and CONSTANTS */
#define TERMAL_CAM 0
#define NORMAL_CAM 1


int  winWidth, winHeight; // current Window width and height
bool activeTimer = true;
int tankx, tanky; //tank coordinates
int targetx, targety;//cursor coordinates
int cam_mode = TERMAL_CAM; //Camera mode 

/*Explotin Effect Variables*/ 
float explosionRadius = 0.0; 
float explosionX = 0.0;
float explosionY = 0.0;
float ExplotionAlpha = 1.0;
bool Explode = false;

bool Locked = true; //cursor lock mode

/*Draw circle and wire circle*/
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

// display text with variables.
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

	/*Display complex object */
void tanks(int size, int x, int y)
{
	if(cam_mode == TERMAL_CAM)
		glColor3ub(68, 68, 68);
	else
		glColor3ub(7, 124, 0);
	circle(x - (size * 9 / 4), y - (size * 1 / 4), size * 2 / 5);
	circle(x - (size * 7 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle(x - (size * 4 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle(x - (size * 0 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle(x + (size * 4 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle(x + (size * 7 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle(x + (size * 9 / 4), y - (size * 1 / 4), size * 2 / 5);
	if (cam_mode == TERMAL_CAM)
		glColor3ub(110, 110, 110);
	else
		glColor3ub(11, 91, 11);
	circle_wire(x - (size * 9 / 4), y - (size * 1 / 4), size * 2 / 5);
	circle_wire(x - (size * 7 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle_wire(x - (size * 4 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle_wire(x - (size * 0 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle_wire(x + (size * 4 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle_wire(x + (size * 7 / 4), y - (size * 3 / 4), size * 2 / 6);
	circle_wire(x + (size * 9 / 4), y - (size * 1 / 4), size * 2 / 5);
	
	if (cam_mode == TERMAL_CAM)
		glColor3ub(68, 68, 68);
	else
		glColor3ub(7, 124, 0);
	glBegin(GL_QUADS);
	glVertex2f(x-(3*size),y);
	glVertex2f(x+(3*size),y);
	glVertex2f(x + (2 * size), y + (size));
	glVertex2f(x - (2 * size), y + (size));
	glEnd();
	if (cam_mode == TERMAL_CAM)
		glColor3ub(110, 110, 110);
	else
		glColor3ub(11, 91, 11);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x - (3 * size), y);
	glVertex2f(x + (3 * size), y);
	glVertex2f(x + (2 * size), y + (size));
	glVertex2f(x - (2 * size), y + (size));
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(x - (3 * size), y);
	glVertex2f(x + (3 * size), y);
	glVertex2f(x + (2 * size), y - (size));
	glVertex2f(x - (2 * size), y - (size));
	glEnd();
	
	circle(x - size, y + (size * 3 / 2), size / 2);
	circle(x + size, y + (size * 3 / 2), size / 2);
	glRectf(x + size, y + (2 * size), x - size, y + size);
	glBegin(GL_LINES);
	glVertex2f(x + size, y + (size * 3 / 2));
	glVertex2f(x + (size*4), y + (size * 3 / 2));
	glEnd();

}

void lockedtanks(int size, int x, int y)
{
	tanks(size, x, y);
	if (cam_mode == TERMAL_CAM)
		glColor3ub(50, 255, 50);
	else
		glColor3ub(255, 255, 255);
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	glVertex2f(x - size*3, y - size);
	glVertex2f(x - size*3, y - size*2);
	glVertex2f(x - size*2, y - size*2);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(x + size * 3, y - size);
	glVertex2f(x + size * 3, y - size * 2);
	glVertex2f(x + size * 2, y - size * 2);
	glEnd();


	glBegin(GL_LINE_STRIP);
	glVertex2f(x - size * 3, y + size);
	glVertex2f(x - size * 3, y + size * 2);
	glVertex2f(x - size * 2, y + size * 2);
	glEnd();


	glBegin(GL_LINE_STRIP);
	glVertex2f(x + size * 3, y + size);
	glVertex2f(x + size * 3, y + size * 2);
	glVertex2f(x + size * 2, y + size * 2);
	glEnd();

}

void houses(int size, int x, int y) 
{
	if (cam_mode == TERMAL_CAM)
		glColor3ub(68, 68, 68);
	else
		glColor3ub(188, 194, 0);
	glRectf(x - size, y - size, x + size, y);

	if (cam_mode == TERMAL_CAM)
		glColor3ub(91, 91, 91);
	else
		glColor3ub(194, 26, 0);  // yellow
	glBegin(GL_TRIANGLES);
	glVertex2f(x-size, y);
	glVertex2f(x+size, y);
	glVertex2f(x, y+size);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(x - size, y);
	glVertex2f(x - size+5, y+size);
	glVertex2f(x+5, y + (size*2));
	glVertex2f(x, y + size);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(x + size, y);
	glVertex2f(x + size + 5, y + size);
	glVertex2f(x + 5, y + (size * 2));
	glVertex2f(x, y + size);
	glEnd();
	if (cam_mode == TERMAL_CAM)
		glColor3ub(68, 68, 68);
	else
		glColor3ub(188, 194, 0);
	glBegin(GL_QUADS);
	glVertex2f(x + size, y);
	glVertex2f(x + size + 5, y + size);
	glVertex2f(x + size+6,(size/2)-6+y);
	glVertex2f(x + size, y - size);
	glEnd();
	if (cam_mode == TERMAL_CAM)
		glColor3ub(153, 153, 153);
	else
		glColor3ub(15, 15, 15);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x - size, y);
	glVertex2f(x + size, y);
	glVertex2f(x, y + size);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(x - size, y);
	glVertex2f(x - size + 5, y + size);
	glVertex2f(x + 5, y + (size * 2));
	glVertex2f(x, y + size);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(x + size, y);
	glVertex2f(x + size + 5, y + size);
	glVertex2f(x + 5, y + (size * 2));
	glVertex2f(x, y + size);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(x + size, y);
	glVertex2f(x + size + 5, y + size);
	glVertex2f(x + size + 6, (size / 2) - 6 + y);
	glVertex2f(x + size, y - size);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(x - size, y);
	glVertex2f(x - size, y - size);
	glVertex2f(x + size,y - size);
	glVertex2f(x + size, y);
	glEnd();

}

void tree(int size, int x, int y)
{
	if (cam_mode == TERMAL_CAM)
		glColor3ub(153, 153, 153);
	else
		glColor3ub(1, 151, 12);
	glBegin(GL_TRIANGLES);
	glVertex2f(x - size, y);
	glVertex2f(x + size, y);
	glVertex2f(x, y + (size * 3));
	glEnd();
	if (cam_mode == TERMAL_CAM)
		glColor3ub(68, 68, 68);
	else
		glColor3ub(64, 34, 34);
	glRectf(x - 5, y - size, x + 5, y);
}

void tree_group(int size,int x, int y) // I grouped trees to make displaying easier 
{
	tree(size, x + (size * 2), y + (size * 2));
	tree(size, x + (size * 3), y - (size * 2));
	tree(size, x - (size * 1), y - (size * 3));
	tree(size, x - (size * 3), y - (size * 1));
	tree(size, x - (size * 1), y + (size * 2));
	tree(size, x, y);
}
/*Display User Interface and Cursor*/
void display_UI()
{

	//
	// clear window to black
	//
	glColor3ub(1, 35, 6);
	glRectf(winWidth / 2, winHeight/2, winWidth / 2 - 50, -winHeight/2);
	glRectf(-winWidth / 2, winHeight/2, -winWidth / 2 + 50, -winHeight/2);
	glRectf(winWidth / 2, winHeight/2, -winWidth / 2 , winHeight/2 - 50);
	glRectf(-winWidth / 2, -winHeight/2, winWidth / 2 , -winHeight/2 + 50);

	if (cam_mode == TERMAL_CAM)
		glColor3ub(50,255,50);
	else
		glColor3ub(255,255, 255);
	vprint(-winWidth / 2 + 100, winHeight / 2 - 100, GLUT_BITMAP_HELVETICA_18, "SHADOW-1");
	vprint(-winWidth / 2 + 100, winHeight / 2 - 130, GLUT_BITMAP_HELVETICA_18, "Online");
	vprint(-180, winHeight / 2 - 40, GLUT_BITMAP_HELVETICA_18, " Lockheed AC-130 Air Support Mission");
	if(cam_mode == TERMAL_CAM)
		vprint(winWidth / 2 - 200, winHeight / 2 - 100, GLUT_BITMAP_HELVETICA_18, "Termal Camera");
	else
		vprint(winWidth / 2 - 200, winHeight / 2 - 100, GLUT_BITMAP_HELVETICA_18, "NORM");

	vprint(winWidth / 2 - 200, -winHeight / 2 + 100, GLUT_BITMAP_HELVETICA_18, "26   25MM");
	vprint(-70, -winHeight / 2 + 160, GLUT_BITMAP_HELVETICA_18, "25MM ARMED");
	
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2f(0, 0);
	glEnd();

	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-500, 200);
	glVertex2f(-500, 250);
	glVertex2f(-450,250);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(500, 200);
	glVertex2f(500, 250);
	glVertex2f(450, 250);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(-500, -200);
	glVertex2f(-500, -250);
	glVertex2f(-450, -250);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(500, -200);
	glVertex2f(500, -250);
	glVertex2f(450, -250);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(winWidth / 2 - 100, winHeight / 2 - 200);
	glVertex2f(winWidth / 2 - 100, -winHeight / 2 + 200);
	glEnd();



}
void Cursor_Target(int x, int y)
{
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	glVertex2f(x - 10, y - 5);
	glVertex2f(x - 10, y - 10);
	glVertex2f(x - 5, y - 10);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(x - 10, y + 5);
	glVertex2f(x - 10, y + 10);
	glVertex2f(x - 5, y + 10);
	glEnd();


	glBegin(GL_LINE_STRIP);
	glVertex2f(x + 10, y + 5);
	glVertex2f(x + 10, y + 10);
	glVertex2f(x + 5, y + 10);
	glEnd();


	glBegin(GL_LINE_STRIP);
	glVertex2f(x + 10, y - 5);
	glVertex2f(x + 10, y - 10);
	glVertex2f(x + 5, y - 10);
	glEnd();

}

/*Explotion Effect*/
void explotion(int x, int y)
{
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++) {
		float theta = i * 3.14159265359 / 180;
		float x1 = explosionRadius * cos(theta)+x;
		float y1 = explosionRadius * sin(theta)+y;
		if(cam_mode == TERMAL_CAM)
			glColor4f(1.0, 1.0, 1.0, ExplotionAlpha);
		else
			glColor4f(1.0, 1.0, 0.0, ExplotionAlpha);
		glVertex2f(x1, y1);
	}
	glEnd();

}


/*BackGround*/
void display_back()
{
	if (cam_mode == TERMAL_CAM)
		glColor3ub(10, 10, 10);
	else
		glColor3ub(128, 90, 12);
	glRectf(winWidth, winHeight, -winWidth, -winHeight);
}

/*Display all pasive objects*/
void display_map()
{
	houses(15, 220, 260);
	houses(15, 260, 260);
	houses(15, 380, 180);
	houses(15, -160, 140);
	houses(15, -140,10);
	houses(20, 120, 20);
	houses(15, -90, 220);
	houses(15, 500, 180);
	houses(15, -450, 220);
	houses(11, 440, 0);
	houses(15, 375, -20);
	houses(15, -290, -60);
	houses(15, -50, -50);
	houses(15, -150, -300);
	houses(15, -60, -100);
	houses(15,-500, 220);
	houses(15, -450, -100);
	houses(20, -400, -100);
	houses(15, -200, -220);
	houses(15, 0, -100);
	houses(15,600, -220);
	houses(15, -200, 220);
	houses(15, -200, 0);
	houses(10, 200, 0);
	houses(15, 700, -220);

	tree_group(10, -500, 280);
	tree_group(10, 500, 250);
	tree_group(10, 400, 300);
	tree_group(10, 600, 350);
	tree_group(10, 200, 200);
	tree_group(10, 0, 350);
	tree_group(10, 50, 100);
	tree_group(10, 650, 0);
	tree_group(10, 600, 20);
	tree_group(10, -200, -300);
	tree_group(10, 200, 200);
	tree_group(10, 200, -200);
	tree_group(10, 200, -300);
	tree_group(10, 200, -60);
	tree_group(10, 350, -250);
	tree_group(10, 320, -180);
	tree_group(10, -400, 180);
	tree_group(10, -450, 0);
	tree_group(10, -520, -10);
	tree_group(10, -570, 50);
	tree_group(10, -620, -2);
	tree(10, 50, -300);
	tree(10, -250, 300);
	tree(10, -200, 310);
	tree(10, -300, -300);
	tree(10, -600, -300);
	tree(10, -450, -200);
	tree(10, -650, -200);
	tree_group(10, -400, 180);
}

/*Display Active Objects*/
void AnimatedObjects()
{
	if(!Locked)
		tanks(10, tankx, tanky);
	else
		lockedtanks(10, tankx, tanky);

}

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	display_back();
	display_map();
	AnimatedObjects();
	if (Explode)
		explotion(explosionX, explosionY);
	display_UI();
	if(!Locked)
		Cursor_Target(targetx, targety);

	glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y) 
{
	//exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ')
		activeTimer = !activeTimer;//Pause the scene

	glutPostRedisplay();
}
void onKeyUp(unsigned char key, int x, int y) 
{
	//exit when ESC is pressed.
	if (key == 27)
		exit(0);

	glutPostRedisplay();
}

void onSpecialKeyDown(int key, int x, int y) 
{
	//Swich the camera mode
	if (key == GLUT_KEY_LEFT) {
		if (cam_mode == TERMAL_CAM)
			cam_mode = NORMAL_CAM;
		else
			cam_mode--;
	}

	if (key == GLUT_KEY_RIGHT) {
		if (cam_mode == NORMAL_CAM)
			cam_mode = TERMAL_CAM;
		else
			cam_mode++;
	}

	glutPostRedisplay();
}

/*Check the distant between target object and cursor*/
bool checkCircle(float px, float py, float cx, float cy, float R) 
{
	float dx = (px - cx);
	float dy = (py - cy);
	float d = sqrt(dx * dx + dy * dy);
	return d <= R;
}

void onMove(int x, int y) 
{
	int x2 = x - winWidth / 2;
	int	y2 = winHeight / 2 - y;
	//resets the cursor if the mouse leaves the designated area
	if (y2 > 250 || y2 < -250 || x2 > 500 || x2 < -500)
	{ 
		targety = 0;
		targetx = 0;
	}
	else
	{
		targety = y2;
		targetx = x2;
	}
	//if the cursor approaches the tank it will lock the cursor to the tank
	if (!(y2 > 250 || y2 < -250 || x2 > 500 || x2 < -500))
	{
		if (checkCircle(x2, y2, tankx, tanky, 50))
			Locked = true;
		else
			Locked = false;
	}

	glutPostRedisplay();
}



void onClick(int button, int stat, int x, int y) 
{

	int x2 = x - winWidth / 2;
	int	y2 = winHeight / 2 - y;

	//if the clicked location is close to the tank destroy the tank and resets the tank position
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) 
	{
		if (!(y2 > 250 || y2 < -250 || x2 > 500 || x2 < -500))
		{
			explosionX = targetx;
			explosionY = targety;
			if (checkCircle(x2, y2, tankx, tanky, 50))
			{
				tankx = 1000;
				tanky = 1000;				
			}
			//else
				//printf("miss shot\n");
		}
		Explode = true;
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

#if TIMER_ON == 1 
void onTimer(int v) 
{ 
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	if (activeTimer)
	{
		//If tanks reach the end of the frame reset position and start random position
		if (tankx > winWidth / 2)
		{
			tankx = -700;
			tanky = (rand() % 400) - 200;
		}
		else
		{
			tankx++;
		}
		//Increase the explotion effect size by time (Problematic part)
		if (explosionRadius < 60.0)
		{
			explosionRadius += 0.5;
			ExplotionAlpha -= 0.01;
		}
		else
		{
			Explode = false;
			explosionRadius = 0.0;
			ExplotionAlpha = 2.0;
		}
	}

	glutPostRedisplay();
}
#endif

void Init() {
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

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	/* timer event*/
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif
	Init();
	glutMainLoop();
}


