#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL\glut.h>
#include "texture.h"
#include "map.h"
#include "light.h"

DWORD time1, time2;

#define FPS_PLANNED					40

void inputAndLoad(void) {
	char fileName[100];
	char path[255] = "res\\maps\\";
	printf("Input the terrain bmp file (Leave here empty for default): ");
	gets(fileName);
	if (strcmp(fileName, "") == 0) {
		strcpy(fileName, "default.bmp");
	}
	strcat(path, fileName);
	loadMap(path);
}

void init(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	loadTexture(0, "res\\textures\\dirt_1.bmp");
	loadTexture(1, "res\\textures\\dirt_2.bmp");
	loadTexture(2, "res\\textures\\dirt_3.bmp");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	camX = camZ = 0.0;
	camUpdate();
}

void onReshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void onKeyDown(unsigned char key, int x, int y) {
	switch (key) {
	case 'l':
		if (isLightOpen) {
			glDisable(GL_LIGHT0);
			isLightOpen = 0;
			printf("Light: OFF\n");
		}
		else {
			glEnable(GL_LIGHT0);
			isLightOpen = 1;
			printf("Light: ON\n");
		}
		break;
	case 'q':
		if (flying) {
			flying = 0;
			moveSpeed = 4.00;
		}
		else {
			flying = 1;
			moveSpeed = 10.00;
		}
		break;
	case 'w':
		keyState = keyState | MOVE_FORWARD;  break;
	case 's':
		keyState = keyState | MOVE_BACKWARD;  break;
	case 'a':
		keyState = keyState | ROTATE_LEFT;  break;
	case 'd':
		keyState = keyState | ROTATE_RIGHT;  break;
	case 'r':
		keyState = keyState | LOOK_UP;  break;
	case 'f':
		keyState = keyState | LOOK_DOWN;  break;
	default:
		break;
	}
}

void onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		keyState = keyState & ~MOVE_FORWARD;  break;
	case 's':
		keyState = keyState & ~MOVE_BACKWARD;  break;
	case 'a':
		keyState = keyState & ~ROTATE_LEFT;  break;
	case 'd':
		keyState = keyState & ~ROTATE_RIGHT;  break;
	case 'r':
		keyState = keyState & ~LOOK_UP;  break;
	case 'f':
		keyState = keyState & ~LOOK_DOWN;  break;
	default:
		break;
	}
}

void paintScene(void) {
	GLint i, j;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 0.1, 10000000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camX, camY+bodyHeight, camZ, camX+vecX, camY+bodyHeight+vecY, camZ+vecZ, 0.0, 1.0, 0.0);
	lightInit();
	for (i = 1; i < height; i++) {
		for (j = 1; j < width; j++) {
			glBindTexture(GL_TEXTURE_2D, texDirt[texture[i-1][j-1]][0]);
			glBegin(GL_QUADS);
				glTexCoord2fv(texCoordQuads[0]);
				glVertex3f(terrXZ[i-1][j-1][0], terrY[i-1][j-1], terrXZ[i-1][j-1][1]);
				glTexCoord2fv(texCoordQuads[1]);
				glVertex3f(terrXZ[i-1][j][0], terrY[i-1][j], terrXZ[i-1][j][1]);
				glTexCoord2fv(texCoordQuads[2]);
				glVertex3f(terrXZ[i][j][0], terrY[i][j], terrXZ[i][j][1]);
				glTexCoord2fv(texCoordQuads[3]);
				glVertex3f(terrXZ[i][j-1][0], terrY[i][j-1], terrXZ[i][j-1][1]);
			glEnd();
		}
	}
}

void onDisplay(void) {
	DWORD dtime;
	long sleepTime;
	testKeyState();
	paintScene();
	glutSwapBuffers();
	time2 = GetTickCount();
	dtime = time2 - time1;
	sleepTime = 1000 / FPS_PLANNED - dtime;
	if (sleepTime < 2) {
		sleepTime = 2;
	}
	Sleep(sleepTime);
	time1 = GetTickCount();
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	inputAndLoad();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Terrain 3D");
	smootherize(8);
	init();
	glutDisplayFunc(&onDisplay);
	glutReshapeFunc(&onReshape);
	glutKeyboardFunc(&onKeyDown);
	glutKeyboardUpFunc(&onKeyUp);
	time1 = GetTickCount();
	glutMainLoop();
	return 0;
}