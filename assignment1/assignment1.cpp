// CS 465
// Project
// Author: Zachary Mitchell

#include "../shared/gltools.h"	// OpenGL toolkit
#include <iostream>
using namespace std;
#define PI 3.14159265
#define FORWARD_SPEED 8.f //per second
#define ANGULAR_VELOCITY 35.f //per second
#define MOUSE_ANGULAR_SCALE 0.1f

struct Input
{
	float	vertical = 0; //up = +, down = -
	float	horizontal = 0; //right = +, left = -
	float	forward = 0; //forward = +, backwards = -
	float	strafe = 0; //right = +, left = -1
};
struct Camera
{
	float	x = 0;
	float	y = 0;
	float	z = 0;
	float	rotX = 0;
	float	rotY = 0;
};

void mouseMovement(int x, int y);
void renderScene(void);
void timerFunction(int value);
void setupRC(void);
void changeSize(int w, int h);
void specialKeysDown(int key, int x, int y);
void specialKeysUp(int key, int x, int y);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void updateScene(double deltaTime);

Input input; //input state
Camera camera; //camera position

bool warped; //flag for warping the mouse
bool animTrigger = false;
float timer = 0;	//	timer for animation

int mouseLastx; //keep track of mouse position
int mouseLasty;

// Light values and coordinates

struct Color {
	float r = 0;
	float g = 0;
	float b = 0;
	Color(float red, float green, float blue) : r(red), g(green), b(blue){}
} userLightColor(1, 1, 1);

GLfloat	 lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat  ambientNoLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat	 none[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat  full[] = { 1.0f, 1.0f, 1.0f, 1.0f };

bool specularFlag = true;
bool diffuseFlag = true;
bool ambientFlag = true;
bool smoothFlag = true;

#define NUM_TEXTURES 8
GLuint  textureObjects[NUM_TEXTURES];
#define METALFLOOR 0
#define SUSPENSION 1
#define CONCRETE 2	
#define RAILING 3
#define TRAM 4
#define GUARD 5
#define DOORTOP 6
#define DOORBOTTOM 7

const char *szCubeFaces[6] = { "GalaxyRt.tga", "GalaxyLf.tga", "GalaxyUp.tga", "GalaxyDn.tga", "GalaxyBk.tga", "GalaxyFt.tga" };

GLenum  cube[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

void adjustLight()
{
	diffuse[0] = userLightColor.r;
	diffuse[1] = userLightColor.g;
	diffuse[2] = userLightColor.b;

	specular[0] = userLightColor.r;
	specular[1] = userLightColor.g;
	specular[2] = userLightColor.b;

	ambientLight[0] = .5;
	ambientLight[1] = .5;
	ambientLight[2] = .5;

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	if (ambientFlag)
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	else
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, none);

	if (diffuseFlag)
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	else
		glLightfv(GL_LIGHT0, GL_DIFFUSE, none);

	if (specularFlag)
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	else
		glLightfv(GL_LIGHT0, GL_SPECULAR, none);

	if (smoothFlag)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);
}

void drawSkyBox(void)
{
	GLfloat fExtent = 500.0f;
	glEnable(GL_TEXTURE_CUBE_MAP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_QUADS);
	//////////////////////////////////////////////
	// Negative X
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);


	///////////////////////////////////////////////
	//  Postive X
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);


	////////////////////////////////////////////////
	// Negative Z 
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);


	////////////////////////////////////////////////
	// Positive Z 
	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);


	//////////////////////////////////////////////////
	// Positive Y
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);


	///////////////////////////////////////////////////
	// Negative Y
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);
	glEnd();
	glDisable(GL_TEXTURE_CUBE_MAP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}

///////////////////////////////////////////////////////////
// Called when timer fires
void timerFunction(int value)
{
	//update scene with the amount of time that's passed since last call
	updateScene(16 / 1000.0);

	// Redraw the scene with new coordinates
	glutPostRedisplay();
	glutTimerFunc(16, timerFunction, 1);
}

///////////////////////////////////////////////////////////
// called when mouse moves
void mouseMovement(int x, int y)
{
	if (!warped)
	{
		int mouseDiffx = x - glutGet(GLUT_WINDOW_WIDTH) / 2;
		int mouseDiffy = y - glutGet(GLUT_WINDOW_HEIGHT) / 2;
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		camera.rotX -= ((GLfloat)mouseDiffy)*MOUSE_ANGULAR_SCALE;
		camera.rotY += ((GLfloat)mouseDiffx)*MOUSE_ANGULAR_SCALE;

		warped = true;
	}
}

///////////////////////////////////////////////////////////
// Called to update scene
void updateScene(double deltaTime)
{
	warped = false;

	GLfloat horizontalMovement = 1;
	GLfloat verticalMovement = 0;

	horizontalMovement = FORWARD_SPEED;

	camera.rotX -= input.vertical * ANGULAR_VELOCITY * deltaTime; //negative due to flight-like controls
	if (camera.rotX <= -90)//if looking directly below
		camera.rotX = -90; //stop viewer from looking behind
	if (camera.rotX >= 90) //if looking directly above
		camera.rotX = 90; //stop viewer from looking behind

	camera.rotY += input.horizontal * ANGULAR_VELOCITY * deltaTime;


	horizontalMovement = cos(camera.rotX*PI / 180)*FORWARD_SPEED*deltaTime;
	verticalMovement = sin(camera.rotX*PI / 180)*FORWARD_SPEED*deltaTime;

	//moving forward or backwards
	camera.z -= cos(camera.rotY*PI / 180)*horizontalMovement * input.forward;
	camera.x += sin(camera.rotY*PI / 180)*horizontalMovement * input.forward;
	camera.y += verticalMovement * input.forward;

	//strafing left or right - do not take verticle movement into consideration
	camera.z -= cos((camera.rotY + 90)*PI / 180)*FORWARD_SPEED*deltaTime*input.strafe;
	camera.x += sin((camera.rotY + 90)*PI / 180)*FORWARD_SPEED*deltaTime*input.strafe;
}

void drawCylinder(float radius, float height, float inc)
{
	glBegin(GL_QUADS);
	for (float alpha = 0; alpha < 360; alpha += inc)
	{
		float topLeftX = cos(alpha * (PI / 180.0))*radius;
		float topLeftZ = -sin(alpha * (PI / 180.0))*radius;
		float topLeftY = height / 2;
		glTexCoord2f(alpha / 360.0, 1);
		glNormal3f(topLeftX / radius, 0, topLeftZ / radius);
		glVertex3f(topLeftX, topLeftY, topLeftZ);

		float bottomLeftX = cos(alpha * (PI / 180.0))*radius;
		float bottomLeftZ = -sin(alpha * (PI / 180.0))*radius;
		float bottomLeftY = -height / 2;
		glTexCoord2f(alpha / 360.0, 0);
		glNormal3f(bottomLeftX / radius, 0, bottomLeftZ / radius);
		glVertex3f(bottomLeftX, bottomLeftY, bottomLeftZ);

		float bottomRightX = cos((alpha + inc) * (PI / 180.0))*radius;
		float bottomRightZ = -sin((alpha + inc) * (PI / 180.0))*radius;
		float bottomRightY = -height / 2;
		glTexCoord2f((alpha + inc) / 360.0, 0);
		glNormal3f(bottomRightX / radius, 0, bottomRightZ / radius);
		glVertex3f(bottomRightX, bottomRightY, bottomRightZ);

		float topRightX = cos((alpha + inc) * (PI / 180.0))*radius;
		float topRightZ = -sin((alpha + inc) * (PI / 180.0))*radius;
		float topRightY = height / 2;
		glTexCoord2f((alpha + inc) / 360.0, 1);
		glNormal3f(topRightX / radius, 0, topRightZ / radius);
		glVertex3f(topRightX, topRightY, topRightZ);
	}
	glEnd();
	glPushMatrix();
	glTranslatef(0, height / 2, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (float rad = 0; rad <= 2 * PI; rad += 0.1)
	{
		glVertex3f(cos(rad)*radius, 0, -sin(rad)*radius);
	}
	glVertex3f(cos(0)*radius, 0, -sin(0)*radius);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, -height / 2, 0);
	glRotatef(180, 1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (float rad = 0; rad <= 2 * PI; rad += 0.1)
	{
		glVertex3f(cos(rad)*radius, 0, -sin(rad)*radius);
	}
	glVertex3f(cos(0)*radius, 0, -sin(0)*radius);
	glEnd();
	glPopMatrix();
}

void makeWall(int col, int row, float scale)
{
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	for (int i = 0; i <= col * 100; i += 100)
		for (int j = 0; j <= row * 100; j += 100)
		{
			glTexCoord2f(1.0f * scale, 1.0f * scale);
			glVertex3f(i + 100, 0, j);
			glTexCoord2f(0.0f, 1.0f * scale);
			glVertex3f(i, 0, j);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(i, 0, j + 100);
			glTexCoord2f(1.0f * scale, 0.0f);
			glVertex3f(i + 100, 0, j + 100);
		}
	glEnd();
}

void drawWalkway(void) {
	//Floor
	glBindTexture(GL_TEXTURE_2D, textureObjects[METALFLOOR]);

	glPushMatrix();
	glScalef(.01f, 1.0f, .01f);
	makeWall(1, 1, 1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -2.0f);
	glScalef(.01f, 1.0f, .01f);
	makeWall(1, 1, 1.0f);
	glPopMatrix();

	for (int i = 0; i < 20; i++) {
		glPushMatrix();
		glTranslatef(2.0f, 0.0f, 0 - i);
		glScalef(.01f, 1.0f, .01f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 20; i++) {
		glPushMatrix();
		glTranslatef(3.0f, 0.0f, 0 - i);
		glScalef(.01f, 1.0f, .01f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(5.0f, 0.0f, -19);
	glScalef(.005f, 1.0f, .005f);
	makeWall(1, 1, .5f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0f, 0.0f, -19.0f);
	glScalef(.005f, 1.0f, .005f);
	makeWall(1, 1, .5f);
	glPopMatrix();

	//	Suspensions
	glBindTexture(GL_TEXTURE_2D, textureObjects[SUSPENSION]);

	glPushMatrix();
	glTranslatef(1.0f, 1.7f, -12.0f);
	glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
	drawCylinder(.02f, 12.0f, 10.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0f, 1.7f, -6.0f);
	glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
	drawCylinder(.02f, 12.0f, 10.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(6.0f, 1.7f, -6.0f);
	glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
	drawCylinder(.02f, 12.0f, 10.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(6.0f, 1.7f, -12.0f);
	glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
	drawCylinder(.02f, 12.0f, 10.0f);
	glPopMatrix();
}

void drawCrowbar(void) {
	glColor3f(1.0f, 1.0f, 0.0f);

	glTranslatef(.5f, -.3f, -1.0f);
	glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
	drawCylinder(.02f, .8f, 10.0f);

	glTranslatef(0.0f, .35f, -.1f);
	glRotatef(-110.0f, 1.0f, 0.0f, 0.0f);
	drawCylinder(.02f, .2f, 10.0f);

	glTranslatef(0.0f, .1f, -.1f);
	glRotatef(110.0f, 1.0f, 0.0f, 0.0f);
	drawCylinder(.02f, .2f, 10.0f);
}

void drawOuterWall(void) {
	glBindTexture(GL_TEXTURE_2D, textureObjects[CONCRETE]);

	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glTranslatef(1.0f, 1 + i, -19.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 5; i++) {
		glPushMatrix();
		glTranslatef(1 + i, 3.0f, -19.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(5.0f, 2 - i, -19.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(5.0f, 0 + i, -19.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(2.0f, 1 + i, -19.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.005f, 00.f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glTranslatef(2 + i, 2.0f, -19.0f);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glTranslatef(2 + i, 0.0f, -20.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glTranslatef(1.0f, 1 + i, -20.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 3; i++) {
		glPushMatrix();
		glTranslatef(6.0f, 1 + i, -19.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 5; i++) {
		glPushMatrix();
		glTranslatef(1 + i, 3.0f, -20.0f);
		glScalef(.005f, 0.0f, .005f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}
}

void drawRailing(void) {
	glBindTexture(GL_TEXTURE_2D, textureObjects[RAILING]);
	glDepthMask(GL_FALSE);

	glPushMatrix();
	glTranslatef(0.0f, 1.5f, -2.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(.02f, 0.0f, .02f);
	makeWall(0, 0, 1.0f);
	glPopMatrix();

	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(0.0f, 1.5f, 2.0f - (i * 2.0f));
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.02f, 0.0f, .02f);
		makeWall(0, 0, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 8; i++) {
		glPushMatrix();
		glTranslatef(2.0f, -.5f, -4 - (i * 2));
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.02f, 0.0f, .02f);
		makeWall(0, 0, 1.0f);
		glPopMatrix();
	}

	for (int i = 0; i < 10; i++) {
		glPushMatrix();
		glTranslatef(5.0f, -.5f, 2 - (i * 2));
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.02f, 0.0f, .02f);
		makeWall(0, 0, 1.0f);
		glPopMatrix();
	}
	glDepthMask(GL_TRUE);
}

void drawRoomOneBorders(void) {
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 4; j++) {
			glPushMatrix();
			glTranslatef(9.0f, (j * 2.0f) - 3.0f, 3.0f - (i * 2));
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			glScalef(.02f, 0.0f, .02f);
			makeWall(1, 1, 1.0f);
			glPopMatrix();
		}
	}

	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 4; j++) {
			glPushMatrix();
			glTranslatef(-2.0f, (j * 2.0f) - 3.0f, -.5f -(i * 2));
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			glScalef(.02f, 0.0f, .02f);
			makeWall(1, 1, 1.0f);
			glPopMatrix();
		}
	}

	for (int j = 0; j < 4; j++) {
		glPushMatrix();
		glTranslatef(-2.0f, (j * 2.0f) + 1.0f, -20.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.02f, 0.0f, .02f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	for (int j = 0; j < 4; j++) {
		glPushMatrix();
		glTranslatef(6.0f, (j * 2.0f) + 1.0f, -20.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(.02f, 0.0f, .02f);
		makeWall(1, 1, 1.0f);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(2.0f, 7.0f, -20.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(.02f, 0.0f, .02f);
	makeWall(1, 1, 1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.0f, 0.0f, -20.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(.02f, 0.0f, .02f);
	makeWall(1, 1, 1.0f);
	glPopMatrix();

	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 5; j++) {
			glPushMatrix();
			glTranslatef(-2.0f + (j * 2.0f), 7.0f, 4.0f - (i * 2.0f));
			glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
			glScalef(.02f, 0.0f, .02f);
			makeWall(1, 1, 1.0f);
			glPopMatrix();
		}
	}
}

void drawGuard(void) {
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureObjects[GUARD]);
	glTranslatef(3.0f, 1.7f, -15.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(.018f, 1.0f, .018f);
	makeWall(0, 0, 1.0f);
	glPopMatrix();
}

void drawDoor(void) {
	if (animTrigger) {
		if (timer <= 1) {
			timer += .005;
		}
	}

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureObjects[DOORTOP]);
	glTranslatef(2.0f, 2.0f + timer, -20.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(.03f, 1.0f, .01f);
	makeWall(0, 0, 1.0f);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureObjects[DOORBOTTOM]);
	glTranslatef(2.0f, 1.0f - timer, -20.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(.03f, 1.0f, .01f);
	makeWall(0, 0, 1.0f);
	glPopMatrix();
}

///////////////////////////////////////////////////////////
// Called to draw scene
void renderScene(void)
{
	GLUquadricObj *pObj;	// Quadric Object
	pObj = gluNewQuadric();
	gluQuadricNormals(pObj, GLU_SMOOTH);

	adjustLight();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset Model view matrix stack
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	handy crowbar space
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	drawCrowbar();
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

	//	View space
	glRotatef(-camera.rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(camera.rotY, 0.0f, 1.0f, 0.0f);

	drawSkyBox();

	glTranslatef(-camera.x, -camera.y, -camera.z);
	if (camera.z <= -5.0f && !animTrigger) {
		animTrigger = true;
	}
	//	World space

	glTranslatef(-1.0f, -1.0f, 9.0f);

	//	Walkway space
	glPushMatrix();
	drawWalkway();
	glPopMatrix();

	//	Tram space
	glColor3f(.5, .5, .5);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureObjects[TRAM]);
	glTranslatef(5.0f, 2.5f, 2.0f);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(.05f, 1.0f, -.025f);
	makeWall(0, 0, 1.0f);
	glPopMatrix();

	//	Draw door
	glPushMatrix();
	drawDoor();
	glPopMatrix();

	//	Outer Wall space
	glPushMatrix();
	drawOuterWall();
	glPopMatrix();

	//	First room borders
	glPushMatrix();
	drawRoomOneBorders();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 0);
	glDisable(GL_LIGHTING);
	glTranslatef(3.5f, 2.0f, -19.5f);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glutSolidSphere(.1, 40, 40);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	glColor3f(1, 1, 1);
	drawRailing();
	drawGuard();
	glPopMatrix();
	glDisable(GL_BLEND);

	//Flush drawing commands
	glutSwapBuffers();
	glutPostRedisplay();
}


///////////////////////////////////////////////////////////
// Setup the rendering context
void setupRC(void)
{
	GLbyte *pBytes;
	GLint iWidth, iHeight, iComponents;
	GLenum eFormat;

	// White background
	glClearColor(0.5f, 0.95f, 1.0f, 1.0f);

	// Set color shading model to flat
	glShadeModel(GL_SMOOTH);
	glEnable(GL_RESCALE_NORMAL);

	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);

	//initial positions
	camera.x = 0;
	camera.y = 0.5;
	camera.z = 10.0;

	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	warped = false;

	//remove cursor
	glutSetCursor(GLUT_CURSOR_NONE);

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	// Supply a slight ambient light so the objects can be seen
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	// The light is composed of just a diffuse and specular components
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// Enable this light in particular
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);

	glMateriali(GL_FRONT, GL_SHININESS, 128);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);

	// Load textures
	glGenTextures(NUM_TEXTURES, textureObjects);

	glBindTexture(GL_TEXTURE_2D, textureObjects[METALFLOOR]);
	pBytes = gltLoadTGA("metalfloor.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, textureObjects[SUSPENSION]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pBytes = gltLoadTGA("suspension.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);

	glBindTexture(GL_TEXTURE_2D, textureObjects[CONCRETE]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pBytes = gltLoadTGA("concrete.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);

	glBindTexture(GL_TEXTURE_2D, textureObjects[RAILING]);
	pBytes = gltLoadTGA("railing.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, textureObjects[TRAM]);
	pBytes = gltLoadTGA("tram.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, textureObjects[GUARD]);
	pBytes = gltLoadTGA("guard.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, textureObjects[DOORTOP]);
	pBytes = gltLoadTGA("doorTop.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, textureObjects[DOORBOTTOM]);
	pBytes = gltLoadTGA("doorBottom.tga", &iWidth, &iHeight, &iComponents, &eFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
	// Load Cube Map images
	for (int i = 0; i < 6; i++)
	{
		// Load this texture map
		pBytes = gltLoadTGA(szCubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
		gluBuild2DMipmaps(cube[i], iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
		glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);
	}

	// Set up texture maps        
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void changeSize(int w, int h)
{
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset projection matrix stack
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	GLfloat fAspect;
	fAspect = (GLfloat)w / (GLfloat)h;

	gluPerspective(45, fAspect, 0.001, 1000);

	// Reset Model view matrix stack
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

///////////////////////////////////////////////////////////
// Respond to arrow keys by moving the camera frame of reference
void specialKeysDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		input.vertical = 1;
		break;
	case GLUT_KEY_DOWN:
		input.vertical = -1;
		break;
	case GLUT_KEY_LEFT:
		input.horizontal = -1;
		break;
	case GLUT_KEY_RIGHT:
		input.horizontal = 1;
		break;
	default:
		break;
	}
}


///////////////////////////////////////////////////////////
// Respond when arrow keys are depressed
void specialKeysUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		input.vertical = 0;
		break;
	case GLUT_KEY_DOWN:
		input.vertical = 0;
		break;
	case GLUT_KEY_LEFT:
		input.horizontal = 0;
		break;
	case GLUT_KEY_RIGHT:
		input.horizontal = 0;
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////
// Respond to keyboard keys associated with ASCII (pressed)
void keyboardDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		input.forward = 1;
		break;
	case 's':
		input.forward = -1;
		break;
	case 'a':
		input.strafe = -1;
		break;
	case 'd':
		input.strafe = 1;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

}
///////////////////////////////////////////////////////////
// Respond to keyboard keys associated with ASCII (depressed)
void keyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		input.forward = 0;
		break;
	case 's':
		input.forward = 0;
		break;
	case 'a':
		input.strafe = 0;
		break;
	case 'd':
		input.strafe = 0;
		break;
	default:
		break;
	}

}

///////////////////////////////////////////////////////////
// Main program entry point
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutCreateWindow("Project");
	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);
	glutSpecialFunc(specialKeysDown);
	glutSpecialUpFunc(specialKeysUp);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutPassiveMotionFunc(mouseMovement);
	glutTimerFunc(16, timerFunction, 1);
	setupRC();

	glutMainLoop();

	return 0;
}