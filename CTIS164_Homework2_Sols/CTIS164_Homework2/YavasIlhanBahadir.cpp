/*************
	CTIS 164
 -------------
 STUDENT : Ilhan Bahadir Yavas
 SECTION : 04
 HOMEWORK: Homework #2
 -------------
 PROBLEMS: .
 -------------
 ADDITIONAL FEATURES:
 * Show kill number
 * Base can take a damage and if base health reach the 0 game end 
 * Display base health
 * Cool explotion effect
 * Bomb drop time depend on the distance between plane and target
 * Target Cursor never goes to base
 * Tanks disappear again immediately after entering the base
 * 
**************/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>



/* TEMPLATE CONSTANTS AND VARIABLES */
#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800
#define PI 3.1415
#define SIZE  10 //defaut size
#define  TIMER_PERIOD  17 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

/* USER GLOBAL VARIABLES and CONSTANTS */

typedef struct{
	float x;
	float y;
}tank_t;

tank_t tankPoints[5];//store all Tank position 

int  winWidth, winHeight; // current Window width and height
bool activeTimer = true;
bool bombing = false;  // records whether the attack has started or not
bool bombDrop = false; // Records whether the bomb has begun to fall
bool explode = false; // Records whether the explosion effect has started or not.
bool explodeB = false; // Records whether the BASE explosion effect has started or not.
bool gameOn = false; // current state of the game
int boom_tim = 20; // duration of the explosion effect
int planex = WINDOW_WIDTH / 2 + 100, planey =  300; //plane coordinates
int bombx, bomby = planey; //Bomb coordinates
int base_health = 5; // Base Health
int targetx = 0, targety = -100;//cursor coordinates
int kill = 0; // Kill counter
float rem_time = 20.0; // remaining time

/*Draw circle and wire circle*/
void circle(int x, int y, int r)
{

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
/*Draw quarter circle*/
void quarter_circle(int x, int y, int r)
{

	float angle;
	glBegin(GL_POLYGON);
	glVertex2f(x,y);
	for (int i = 0; i < 100; i++)
	{
		angle = 0.5 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}
void quarter_circle2(int x, int y, int r)
{

	float angle;
	glBegin(GL_POLYGON);
	glVertex2f(x,y);
	for (int i = 0; i < 100; i++)
	{
		angle = -0.5 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}
/*Draw ellipse*/
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
void Ellipse_wire(float cx, float cy, float rx, float ry, int num)
{
	float theta = 2 * 3.1415926 / float(num);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = 1;//we start at angle = 0 
	float y = 0;

	glBegin(GL_LINE_LOOP);
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

/*Display complex object */
void tanks(int size, float *x, float *y)
{
	glLineWidth(2);
	glColor3ub(7, 124, 0);
	circle(*x - (size * 9 / 4), *y - (size * 1 / 4), size * 2 / 5);
	circle(*x - (size * 7 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle(*x - (size * 4 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle(*x - (size * 0 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle(*x + (size * 4 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle(*x + (size * 7 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle(*x + (size * 9 / 4), *y - (size * 1 / 4), size * 2 / 5);

	glColor3ub(11, 91, 11);
	circle_wire(*x - (size * 9 / 4), *y - (size * 1 / 4), size * 2 / 5);
	circle_wire(*x - (size * 7 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle_wire(*x - (size * 4 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle_wire(*x - (size * 0 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle_wire(*x + (size * 4 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle_wire(*x + (size * 7 / 4), *y - (size * 3 / 4), size * 2 / 6);
	circle_wire(*x + (size * 9 / 4), *y - (size * 1 / 4), size * 2 / 5);


	glColor3ub(7, 124, 0);
	glBegin(GL_QUADS);
	glVertex2f(*x - (3 * size), *y);
	glVertex2f(*x + (3 * size), *y);
	glVertex2f(*x + (2 * size), *y + (size));
	glVertex2f(*x - (2 * size), *y + (size));
	glEnd();

	glColor3ub(11, 91, 11);
	glBegin(GL_LINE_LOOP);
	glVertex2f(*x - (3 * size), *y);
	glVertex2f(*x + (3 * size), *y);
	glVertex2f(*x + (2 * size), *y + (size));
	glVertex2f(*x - (2 * size), *y + (size));
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(*x - (3 * size), *y);
	glVertex2f(*x + (3 * size), *y);
	glVertex2f(*x + (2 * size), *y - (size));
	glVertex2f(*x - (2 * size), *y - (size));
	glEnd();

	circle(*x - size, *y + (size * 3 / 2), size / 2);
	circle(*x + size, *y + (size * 3 / 2), size / 2);
	glRectf(*x + size, *y + (2 * size), *x - size, *y + size);
	glBegin(GL_LINES);
	glVertex2f(*x + size, *y + (size * 3 / 2));
	glVertex2f(*x + (size * 4), *y + (size * 3 / 2));
	glEnd();

}

void initializePoints(tank_t *arr) // initialize all point
{
	for (int i = 0; i < 5; i++)
	{
		arr[i].y = -(rand() % 280) - 20;
		arr[i].x = (rand() % 100) - 900;
	}
}

void drawAllTanks()
{
	for (int i = 0; i < 5; i++)
	{
		tanks(SIZE, &tankPoints[i].x, &tankPoints[i].y);
	}
}

void tree(int size, int x, int y)
{
	glColor3ub(1, 151, 12);
	glBegin(GL_TRIANGLES);
	glVertex2f(x - size, y);
	glVertex2f(x + size, y);
	glVertex2f(x, y + (size * 3));
	glEnd();

	glColor3ub(64, 34, 34);
	glRectf(x - 5, y - size, x + 5, y);
}

void Airplane(int x, int y)
{
	glColor3ub(127, 137, 147);
	glBegin(GL_POLYGON);
	glVertex2f(x-138 / 5 ,y+12 / 5);
	glVertex2f(x-291 / 5,y-16 / 5);
	glVertex2f(x-348 / 5,y-37 / 5);
	glVertex2f(x-355 / 5,y-59 / 5);
	glVertex2f(x-285 / 5,y-77 / 5);
	glVertex2f(x-71 / 5,y-77 / 5);
	glVertex2f(x+72 / 5,y-68 / 5);
	glVertex2f(x+372 / 5,y-47 / 5);
	glVertex2f(x+382 / 5,y-37 / 5);
	glVertex2f(x+370 / 5,y-26 / 5);
	glEnd();

	glColor3ub(0,0,0);
	glBegin(GL_POLYGON);
	glVertex2f(x - 290 / 5, y - 16 / 5);
	glVertex2f(x - 235 / 5, y + 28 / 5);
	glVertex2f(x - 139 / 5, y + 10 / 5);
	glEnd();

	glColor3ub(60, 65, 70);
	glBegin(GL_POLYGON);
	glVertex2f(x + 82 / 5, y + 29/5);
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
	Ellipse(x-10, y-57/5, 26, 6, 10);
}

void bunker(int x, int y) // Draw bunker
{
	glColor3ub(0, 0, 0);
	glRectf(x, y, x + 30, y + 10);
	glColor3ub(128, 128, 128);
	quarter_circle2(x + 5, y + 10, -10);
	quarter_circle2(x - 5, y - 10, -10);
	glRectf(x + 5, y + 10, x + 30, y + 20);
	glRectf(x - 5, y - 10, x + 30, y);
	glBegin(GL_POLYGON);
	glVertex2f(x + 30, y + 20);
	glVertex2f(x + 30, y - 10);
	glVertex2f(x + 60, y - 10);
	glEnd();
}
//Draw simple bomb
void bomb(int x, int y)
{
	glColor3ub(50, 10, 10);
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y + 10);
	glVertex2f(x - 10, y + 20);
	glVertex2f(x + 10, y + 20);
	glEnd();
	glColor3ub(50, 55, 60);
	Ellipse(x, y, 6, 15, 10);
}

 // Bomb Explode effect
void Explode(int x, int y) 
{
	float angle;
	glColor3ub(255, 255, 60);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	for (int i = 0; i < 100; i++)
	{
		angle = 1 * PI * i / 100;
		glVertex2f(x + ((rand() % 50) + 10) * cos(angle), y + ((rand() % 50) + 10) * sin(angle));
	}
	glEnd();

	glColor3ub(255, 60, 60);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	for (int i = 0; i < 100; i++)
	{
		angle = 1 * PI * i / 100;
		glVertex2f(x + ((rand() % 20) + 20) * cos(angle), y+ ((rand() % 20) + 20) * sin(angle));
	}
	glEnd();
}

 // Base explode effect
void ExplodeB(int x, int y)
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

	glColor3ub(255, 60, 60);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	for (int i = 0; i < 100; i++)
	{
		angle = 1 * PI * i / 100;
		glVertex2f(x + ((rand() % 20) + 40) * cos(angle), y+ ((rand() % 20) + 40) * sin(angle));
	}
	glEnd();
}

/*GAME area BackGround*/
void display_GameArea()
{

	glColor3ub(0, 0, 120);
	glRectf(winWidth, winHeight , -winWidth, 0);
	glColor3ub(70, 79, 0);
	glRectf(winWidth, 0, -winWidth, -winHeight);
	glColor3ub(120, 120, 120);
	glBegin(GL_POLYGON);
	glVertex2f(380,0);
	glVertex2f(400,0);
	glVertex2f(20,-400);
	glVertex2f(0,-400);
	glEnd();
	tree(20, 150, -20);
	tree(20, 100, -12);
	tree(20, -350, -50);	
	tree(20, -10, -10);
	tree(20, -20, -50);
	tree(20, -90, -26);
	tree(20, -200, -50);
	tree(20, -280, -15);
	tree(20, -100, -40);
	tree(20, -300, -10);
	tree(20, -320, -50);
	tree(20, -400, -30);
	tree(20, -460, -12);
	tree(20, -520, -30);
	tree(20, -570, -50);
	tree(20, -620, -0);
	tree(20, -650, -10);
	tree(20, 200, -50);
	tree(20, 280, -15);
	tree(20, 100, -40);
	tree(20, 300, -10);
	tree(20, 320, -50);
	tree(20, 400, -30);
	tree(20, 460, -12);
	tree(20, 520, -30);
	tree(20, 570, -50);
	tree(20, 620, -0);
	tree(20, 650, -10);
	bunker(550, -200);
	glColor3ub(120, 120, 120);
	vprint(-250, winHeight / 2 - 50, GLUT_BITMAP_HELVETICA_18, "DESTROY ALL ENEMY TANKS BEFORE REACH THE BASE");
}

void remaining_time()
{
	glColor3ub(0, 0, 0);
	glRectf(winWidth, -winHeight, -winWidth, 80 - winHeight/2);

	glColor3ub(255, 255, 255);
	vprint(-winWidth / 2 + 10, -winHeight / 2 + 50, GLUT_BITMAP_HELVETICA_18, "Remaing Time");
	vprint(-winWidth / 2 + 10, -winHeight / 2 + 30, GLUT_BITMAP_HELVETICA_18, "%0.2f",rem_time);
	vprint(-winWidth / 2 + 200, -winHeight / 2 + 50, GLUT_BITMAP_HELVETICA_18, "Kill");
	vprint(-winWidth / 2 + 200, -winHeight / 2 + 30, GLUT_BITMAP_HELVETICA_18, "%d",kill);
	vprint(-winWidth / 2 + 300, -winHeight / 2 + 50, GLUT_BITMAP_HELVETICA_18, "Base Health");
	vprint(-winWidth / 2 + 300, -winHeight / 2 + 30, GLUT_BITMAP_HELVETICA_18, "%d",base_health);
}

void key_area()
{
	if (gameOn)
	{
		glColor3ub(255, 255, 10);
		vprint(-winWidth / 2 + 500, -winHeight / 2 + 30, GLUT_BITMAP_HELVETICA_18, "<Space> Set target for allied aircraft, <F1> Pause/Restart , <Arrow Keys> move the target cursor");
	}
	else
	{
		glColor3ub(255, 255, 10);
		vprint(-winWidth / 2 + 700, -winHeight / 2 + 30, GLUT_BITMAP_HELVETICA_18, "<F1> Start Game ");

		if (base_health > 0) //  based on remaining health we find whether the player has won and the appropriate message is printed
		{
			glColor3ub(10, 255, 10);
			vprint(-50, 50, GLUT_BITMAP_HELVETICA_18, "GAME OVER");
			vprint(-50, 20, GLUT_BITMAP_HELVETICA_18, "YOU SURVIVED");
		}
		else
		{
			glColor3ub(255, 10, 10);
			vprint(-50, 50, GLUT_BITMAP_HELVETICA_18, "GAME OVER");
			vprint(-50, 20, GLUT_BITMAP_HELVETICA_18, "YOU DIED");
		}
	}
		
}
/*Draw target cursor and lazer*/
void target_cursor(int x, int y)
{
	glColor3ub(200, 0, 0);
	glLineWidth(5);
	Ellipse_wire(x,y,50,15,300);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
	if (!bombing) // when start bombing stop lazer 
	{
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2f(x, y);
		glVertex2f(550, -195);
		glEnd();
	}
}


/*Display Active Objects*/
void AnimatedObjects()
{
	if(base_health != 0)
		target_cursor(targetx, targety);

	drawAllTanks();
	
	if(explode)
		Explode(targetx, targety);
	else
		Explode(1000, 1000);

	if (explodeB)
		ExplodeB(600, -220);
	else
		ExplodeB(1000, 1000);

	if (bombDrop)
		bomb(bombx, bomby);
	else
		bomb(1000, 1000);
	
	Airplane(planex, planey);
}

/*Display all areas*/
void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	display_GameArea();
	remaining_time();
	key_area();
	AnimatedObjects();
	glutSwapBuffers();

}

void onKeyDown(unsigned char key, int x, int y)
{
	//exit when ESC is pressed.
	if (key == 27)
		exit(0);
	//Start strike when Spacebar is pressed
	if (key == ' ')
		bombing = true;

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
	if (!bombing && activeTimer) // Only able to move target cursor when not strike and timer is active
	{
		switch (key) {

		case GLUT_KEY_UP:
			if (targety < -20)
				targety += 15;
			else
				targety = -20;
			break;

		case GLUT_KEY_DOWN:
			if (targetx - targety < 330 && targety > -WINDOW_HEIGHT / 2 + 100)
				targety -= 15;
			else if (targety < -WINDOW_HEIGHT / 2 + 100)
			{
				targety = -WINDOW_HEIGHT / 2 + 100;
			}
			else if (targetx - targety > 330)
			{
				targety = targetx - 330;
			}

			break;

		case GLUT_KEY_LEFT:
			if (targetx > -WINDOW_WIDTH / 2 + 100)
				targetx -= 15;
			else
				targetx = -WINDOW_WIDTH / 2 + 100;
			break;

		case GLUT_KEY_RIGHT:
			if (targetx - targety < 330)
				targetx += 15;
			else
				targetx = targety + 330;
			break;
		}
	}
	if (key == GLUT_KEY_F1)
	{
		activeTimer = !activeTimer;//Pause the scene
		if (rem_time <= 0)
		{
			//reset all variables if game end
			gameOn = true;
			rem_time = 20.0;
			kill = 0;
			base_health = 5;
		}

	}
	glutPostRedisplay();
}
bool checkCircle(float px, float py, float cx, float cy, float R)
{
	float dx = (px - cx);
	float dy = (py - cy);
	float d = sqrt(dx * dx + dy * dy);
	return d <= R;
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
		float diff = planey - targety;//Calculate difference between airplane and target height

		/*Timer*/
		if (rem_time > 0 )
		{
			rem_time -= 0.017;
			gameOn = true;
		}
		if (rem_time < 0)
		{
			rem_time = 0.0;
			activeTimer = !activeTimer;
			gameOn = false;
		}

		for (int i = 0; i < 5; i++)
		{
			if (tankPoints[i].x > winWidth / 2)
			{
				tankPoints[i].x = (rand() % 100) - 900;
				tankPoints[i].y = -(rand() % 280) - 20;
			}
			else {
				tankPoints[i].x += 1.75;

			}
		}
;
		/*When start strike decrease the airplane x vector and if reach end of the frame reset position*/
 		if (bombing)
		{
			if (planex < -WINDOW_WIDTH / 2)
			{
				planex = WINDOW_WIDTH / 2 + 100;
				bombing = false;
			}
			else
			{
				planex -= 10;
			}
		}
 		if (planex < WINDOW_WIDTH / 2 && planex > targetx && bombx < targetx + diff  && bomby > targety)  // when airplane reach the drop point start drop sequence
		{
			bombDrop = true;
			bombx -= 10;
			bomby -= 10;

		}
		else
		{
			bombDrop = false;
			bomby = planey;
			bombx = planex;
		}
		if (checkCircle(bombx, bomby, targetx, targety, 70)) //Ensure that it will not interact with tanks before reaching the target
		{
			explode = true;
			for (int i = 0; i < 5; i++)
			{
				if (checkCircle(targetx, targety, tankPoints[i].x, tankPoints[i].y, 70))
				{
					tankPoints[i].x = (rand() % 100) - 900;
					tankPoints[i].y = -(rand() % 280) - 20;
					kill++;

				}
			}
		}
		/*Animate explodes*/
 		if (explode)
		{
			if (boom_tim == 0)
			{
				explode = false;
				boom_tim = 20;
			}
			else
			{
				boom_tim -= 1;
				explode = true;
			}
		}
		if (explodeB)
		{
			if (boom_tim == 0)
			{
				explodeB = false;
				boom_tim = 20;
			}
			else
			{
				boom_tim -= 1;
				explodeB = true;
			}
		}
		/*if game end reset all objects*/
		if (!gameOn) 
		{
			base_health = 5;
			initializePoints(tankPoints);
			planex = WINDOW_WIDTH / 2 + 100;
			bomby = planey;
			bombx = planex;
			bombing = false;
			bombDrop = false;

		}
		/*If tanks reach the base decrease the base heath and reset tank position*/
		for (int i = 0; i < 5; i++)
		{
			if (tankPoints[i].x - tankPoints[i].y > 350)
			{
				base_health--;
				tankPoints[i].x = (rand() % 100) - 900;
			}
		}
		/*If base health reach 0 stop game*/
		if (base_health <= 0)
		{
			explodeB = true;
			initializePoints(tankPoints);
			rem_time = 0;
			gameOn = false;
			activeTimer = false;
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
	initializePoints(tankPoints);
	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);
	glutKeyboardUpFunc(onKeyUp);


#if  TIMER_ON == 1
	/* timer event*/
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif
	Init();
	glutMainLoop();
}