GLuint texDirt[3][3];
GLint filter = 0;

GLfloat texCoordQuads[4][2] = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};

// This piece of code comes from the Internet, I just modified it to suit my program. Thanks a lot to the author.
void loadTexture(GLint index, const char* fileName) {
	GLint width, height, byteTotal;
	GLubyte* pixels = 0;
	FILE* pFile = fopen(fileName, "rb");
	printf("Loading texture from: %s .. ", fileName);
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
	fseek(pFile, 0x0036, SEEK_SET);
	{
		GLint byteLine = width * 3;
		while (byteLine % 4 != 0)
			byteLine++;
		byteTotal = byteLine * height;
	}
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
	glGenTextures(3, texDirt[index]);
	glBindTexture(GL_TEXTURE_2D, texDirt[index][0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, texDirt[index][1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, texDirt[index][2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	free(pixels);
}