GLuint isLightOpen = 0;

GLfloat worldLightPosition[] = {0.0, 100.0, 1000.0, 1.0};
GLfloat worldLightAmbient[] = {0.5, 0.5, 0.5, 1.0};
GLfloat worldLightDiffuse[] = {0.8, 0.8, 0.8, 1.0};
GLfloat worldLightSpecular[] = {1.0, 1.0, 1.0, 1.0};

GLfloat dirtMatAmbientAndDiffuse[] = {0.5, 0.5, 0.5, 1.0};
GLfloat dirtMatSpecular[] = {0.75, 0.75, 0.75, 1.0};
GLfloat dirtMatEmission[] = {0.1, 0.1, 0.1, 1.0};
GLfloat dirtMatShininess[] = {40.0};

void lightInit(void) {
	glLightfv(GL_LIGHT0, GL_POSITION, worldLightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, worldLightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, worldLightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, worldLightSpecular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dirtMatAmbientAndDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, dirtMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, dirtMatEmission);
	glMaterialfv(GL_FRONT, GL_SHININESS, dirtMatShininess);
}

