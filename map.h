#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_WIDTH			125
#define MAX_HEIGHT			125
#define CELL_SIZE			40.0

#define NORMAL_ADJUSTMENT	20.0

#define	MOVE_FORWARD		0x1
#define MOVE_BACKWARD		0x2
#define ROTATE_LEFT			0x4
#define ROTATE_RIGHT		0x8
#define LOOK_UP				0x10
#define LOOK_DOWN			0x20

#define MIN_FAI				-45.0
#define MAX_FAI				45.0
#define MIN_THETA			0.0
#define MAX_THETA			360.0

#define ROTATE_SPEED		3.00
#define PI					3.14159265

GLuint keyState = 0;
GLint flying = 0;
GLfloat moveSpeed = 4.00;

// Camera variables.
GLfloat theta = 0.0, fai = 0.0;
GLfloat camX, camY, camZ;
GLfloat vecX, vecY, vecZ;
GLfloat vx, vy, vz;
GLfloat bodyHeight = 200.0;

GLint width, height;

// Seperate X/Z-coords and Y-coords.
// To make Y-coords data continuous, in order to use memcmp() somewhere to make the program run faster.
GLfloat terrXZ[MAX_HEIGHT][MAX_WIDTH][2];
GLfloat terrY[MAX_HEIGHT][MAX_WIDTH];

// Normals, calculated using Slope Algorithm
GLfloat normal[MAX_HEIGHT][MAX_WIDTH][3];

// Texture attached to every cell. Random generated when map generates.
GLint texture[MAX_HEIGHT-1][MAX_WIDTH-1];

// Also used for camera position calculating.
GLfloat terrainLeftBorder, terrainBottomBorder;

// Load a 24-bit BMP file to generate terrain.
// Every pixel's color represents the Y-coord of a point.
// X-coord and Z-coord comes from the position where the pixel holds in the BMP.
void loadMap(const char* fileName) {
	GLint i, j, randTex;
	GLint byteLine, byteTotal;
	GLubyte* pixels = 0;
	GLubyte* bmpLineBegin = 0;
	GLubyte* pBmp = 0;
	FILE* pFile = fopen(fileName, "rb");
	printf("Loading map from: %s .. ", fileName);
	if (pFile == 0) {
		printf("File not found.\n");
		return;
	}
	else {
		printf("Done.\n");
	}
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	terrainLeftBorder = -(width * CELL_SIZE / 2.0);
	terrainBottomBorder = height * CELL_SIZE / 2.0;
	fseek(pFile, 0x0036, SEEK_SET);
	byteLine = width * 3;
	while (byteLine % 4 != 0) byteLine++;
	byteTotal = byteLine * height;
	pixels = (GLubyte*)malloc(byteTotal);
	if (pixels == 0) {
		fclose(pFile);
		return;
	}
	if (fread(pixels, byteTotal, 1, pFile) <= 0) {
		free(pixels);
		fclose(pFile);
		return;
	}
	bmpLineBegin = pixels;

	// Calculate coordinates
	for (i = 0; i < height; i++) {
		pBmp = bmpLineBegin;
		for (j = 0; j < width; j++) {
			terrXZ[i][j][0] = terrainLeftBorder + j * CELL_SIZE;
			terrXZ[i][j][1] = terrainBottomBorder - i * CELL_SIZE;
			terrY[i][j] = (pBmp[0] + pBmp[1] + pBmp[2]) / 1.0;
			pBmp += 3;
		}
		bmpLineBegin += byteLine;
	}
	free(pixels);

	// Calculate normals, using Slope algorithm
	for (i = 0; i < height; i++) {
		normal[i][0][0] = normal[i][width-1][0] = normal[i][0][2] = normal[i][width-1][2] = 0.0;
		normal[i][0][1] = normal[i][width-1][1] = 1.0;
	}
	for (j = 1; j < width-1; j++) {
		normal[0][j][0] = normal[height-1][j][0] = normal[0][j][2] = normal[height-1][j][2] = 0.0;
		normal[0][j][1] = normal[height-1][j][1] = 1.0;
	}
	for (i = 1; i < height-1; i++) {
		for (j = 1; j < width-1; j++) {
			normal[i][j][0] = terrY[i+1][j] - terrY[i-1][j];
			normal[i][j][1] = -NORMAL_ADJUSTMENT;
			normal[i][j][2] = terrY[i][j+1] - terrY[i][j-1];
		}
	}

	// Allocating random textures to each cell
	srand((unsigned)time(NULL));
	for (i = 1; i < height; i++) {
		for (j = 1; j < width; j++) {
			randTex = rand() % 3;
			texture[i-1][j-1] = randTex;
		}
	}
}

GLfloat deg2rad(GLfloat x) {
	return x * PI / 180.0;
}

// Replace one point's height with its surrounding points' heights, to smooth the terrain.
// This function is called in smootherize().
GLfloat mean(GLint i, GLint j) {
	return
		((terrY[i-1][j-1] + terrY[i-1][j] + terrY[i-1][j+1]
		+ terrY[i][j-1] + terrY[i][j] + terrY[i][j+1]
		+ terrY[i+1][j-1] + terrY[i+1][j] + terrY[i+1][j+1]) / 9.0);
}

void smootherize(GLint n) {
	GLfloat temp[2][MAX_WIDTH];
	GLint i, j, k;
	for (k = 0; k < n; k++) {
		for (i = 1; i < height; i++) {
			if (i < height - 1) {
				for (j = 1; j < width-1; j++) {
					temp[1][j] = mean(i, j);	
				}
			}
			if (i > 1) {
				memcpy(&terrY[i-1][1], &temp[0][1], (width-2)*sizeof(GLfloat));
			}
			if (i < height - 1) {
				memcpy(&temp[0][1], &temp[1][1], (width-2)*sizeof(GLfloat));
			}
		}
	}
}

void calcCamY(void) {
	GLint row, col;
	GLfloat d1, d2, d3, d4;
	row = floor((terrainBottomBorder - camZ) / CELL_SIZE);
	col = floor((camX - terrainLeftBorder) / CELL_SIZE);
	d1 = camX - terrXZ[row][col][0];
	d2 = terrXZ[row+1][col+1][0] - camX;
	d3 = camZ - terrXZ[row+1][col+1][1];
	d4 = terrXZ[row][col][1] - camZ;
	camY = (d2*d4*terrY[row+1][col] + d1*d4*terrY[row+1][col+1] + d2*d3*terrY[row][col] + d1*d3*terrY[row][col+1]) / (CELL_SIZE * CELL_SIZE);
	// printf("%.3f, %.3f (terrY[%d][%d] = %.3f) => %.3f\n", camX, camZ, row, col, terrY[row][col], camY);
}

void calcVector(void) {
	vecX = cos(deg2rad(fai)) * sin(deg2rad(theta));
	vecY = sin(deg2rad(fai));
	vecZ = cos(deg2rad(fai)) * cos(deg2rad(theta));
	if (flying) {
		vx = moveSpeed * vecX;
		vy = moveSpeed * vecY;
		vz = moveSpeed * vecZ;
	}
	else {
		vx = moveSpeed * sin(deg2rad(theta));
		vy = 0;
		vz = moveSpeed * cos(deg2rad(theta));
	}
}

void camUpdate(void) {
	if (!flying) {
		calcCamY();
	}
	calcVector();
}

void camMoveForward(void) {
	camX += vx;
	camY += vy;
	camZ += vz;
	camUpdate();
}

void camMoveBackward(void) {
	camX -= vx;
	camY -= vy;
	camZ -= vz;
	camUpdate();
}

void camRotateLeft(void) {
	theta += ROTATE_SPEED;
	if (theta >= MAX_THETA) theta -= MAX_THETA;
	camUpdate();
}

void camRotateRight(void) {
	theta -= ROTATE_SPEED;
	if (theta < MIN_THETA) theta += MAX_THETA;
	camUpdate();
}

void camLookUp(void) {
	if (fai < MAX_FAI) fai += ROTATE_SPEED;
	camUpdate();
}

void camLookDown(void) {
	if (fai > MIN_FAI) fai -= ROTATE_SPEED;
	camUpdate();
}

void state2action(GLuint state) {
	switch (state) {
	case MOVE_FORWARD:
		camMoveForward();  break;
	case MOVE_BACKWARD:
		camMoveBackward();  break;
	case ROTATE_LEFT:
		camRotateLeft();  break;
	case ROTATE_RIGHT:
		camRotateRight();  break;
	case LOOK_UP:
		camLookUp();  break;
	case LOOK_DOWN:
		camLookDown();  break;
	default:
		break;
	}
}

void testKeyState(void) {
	GLuint state = 1;
	GLint i;
	for (i = 0; i < 6; i++) {
		if (state & keyState) {
			state2action(state);
		}
		state <<= 1;
	}	
}