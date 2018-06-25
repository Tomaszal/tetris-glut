#include <GL/glut.h>
#include <time.h>
#include <stdlib.h>

#define B 3  // Vanish zone heigth
#define M 22 // Field heigth
#define N 11 // Field width

const float S = 0.05; // Square size

/* Figure matrix
 * 0 1 2 3
 * 4 5 6 7
 */

const int figures[7][4] = {
	{4, 5, 6, 7}, // I
	{1, 2, 5, 6}, // O
	{4, 5, 6, 1}, // T
	{0, 1, 5, 6}, // S
	{4, 5, 1, 2}, // Z
	{0, 4, 5, 6}, // J
	{4, 5, 6, 2}, // L
};

const float colors[7][3] = {
	{0.000, 0.545, 0.545}, // Cyan
	{1.000, 0.843, 0.000}, // Yellow
	{0.545, 0.000, 0.545}, // Purple
	{0.196, 0.804, 0.196}, // Green
	{0.698, 0.133, 0.133}, // Red
	{0.098, 0.098, 0.439}, // Blue
	{1.000, 0.549, 0.000}, // Orange
};

struct point {
	int x, y;
} a[4], b[4], c[4];

int delay = 300;
int field[M][N] = {0};
int shape, shapeNext;

int gameOver = 0;

int check(void) {
	for (int i = 0; i < 4; i++) {
		if (a[i].x < 0 || a[i].x >= N || a[i].y >= M || field[a[i].y][a[i].x]) {
			return 0;
		}
	}

	return 1;
}

void generateFigure(void) {
	// Randomly choose a figure
	shapeNext = rand() % 7;

	// Generate the figure
	for (int i = 0; i < 4; i++) {
		c[i].x = figures[shapeNext][i] % 4 + N * 0.5 - 1;
		c[i].y = figures[shapeNext][i] / 4;
	}
}

void spawnFigure(void) {
	// Switch to the next shape
	shape = shapeNext;

	// Copy the next figure
	for (int i = 0; i < 4; i++) {
		a[i] = c[i];
	}

	// Drop the figure 1 square down
	for (int i = 0; i < 4; i++) {
		b[i] = a[i];
		a[i].y += 1;
	}

	// End the game if there is a collision
	if (!check()) {
		gameOver = 1;
	}

	// Generate the next figure
	generateFigure();
}

void drawText(
		float x,
		float y,
		char *string,
		float Ox,
		float Oy,
		void *font) {
	
	// if (Ox == NULL) Ox = 0.0f;
	// if (Oy == NULL) Ox = 0.0f;
	if (font == NULL) font = GLUT_BITMAP_HELVETICA_18;

	// Offset the text
	y = -1.0 + y + Oy;
	x = -1.0 + x + Ox;

	// Set the position
	glRasterPos2f(x, y);

	// Draw individual characters
	do {
		glutBitmapCharacter(font, *string);
	} while (*string++ != '\0');
}

void drawRectangle(
		float x,
		float y,
		float width,
		float height,
		float Ox,
		float Oy,
		GLenum type) {

	if (type == 0) type = GL_POLYGON;

	// Offset the rectangle
	y = -1.0 + y + Oy;
	x = -1.0 + x + Ox;

	// Start drawing
	glBegin(type);

	// Set the rectangle verticies
	glVertex2f(x, y - height);
	glVertex2f(x + width, y - height);
	glVertex2f(x + width, y);
	glVertex2f(x, y);

	// End drawing
	glEnd();
}

void setFigureColor(int c) {
	glColor3f(colors[c][0], colors[c][1], colors[c][2]);
}

void draw(void) {
	float Ox = 1.0 - N * S * 0.5;
	float Oy = 1.0 - M * S * 0.5;

	// Start drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Invert Y axis
	gluOrtho2D(-1.0, 1.0, 1.0, -1.0);

	// Draw the current figure
	for (int i = 0; i < 4; i++) {
		if (a[i].y >= B) {
			setFigureColor(shape);
			drawRectangle(a[i].x * S, a[i].y * S, S, S, Ox, Oy, 0);
		}
	}

	// Draw the next figure
	if (!gameOver) {
		glColor3f(1.0, 1.0, 1.0);
		drawText(0, 0, (char *) "Next figure:", Ox + N * S * 0.5 - 2 * S, Oy - 3 * S, NULL);

		for (int i = 0; i < 4; i++) {
			setFigureColor(shapeNext);
			drawRectangle(c[i].x * S, c[i].y * S, S, S, Ox, Oy, 0);
		}
	}

	// Draw the field squares
	for (int i = -1; i < M; i++) {
		for (int j = 0; j < N; j++) {
			if (i >= B) {
				if (field[i][j] != 0) {
					setFigureColor(field[i][j] - 1);
					drawRectangle(j * S, i * S, S, S, Ox, Oy, 0);
				}
			}

			// Draw the grid
			glColor3f(0.3, 0.3, 0.3);
			drawRectangle(j * S, i * S, S, S, Ox, Oy, GL_LINE_LOOP);
		}
	}

	// Draw the field outline
	glColor3f(1.0, 1.0, 1.0);
	drawRectangle(0, 0, N * S, 4 * S, Ox, Oy + 2 * S, GL_LINE_LOOP);
	drawRectangle(0, 0, N * S, (M - B) * S, Ox, Oy + (M - 1) * S, GL_LINE_LOOP);

	if (gameOver) {
		glColor3f(1.0, 0.3, 0.0);
		drawText(0, 0, (char *) "Game over!", Ox + N * S * 0.5 - 2 * S, Oy + 0.2 * S, NULL);
	}

	glutSwapBuffers();
}

void timerFunc() {
	// Gravity
	for (int i = 0; i < 4; i++) {
		b[i] = a[i];
		a[i].y += 1;
	}

	// Field collision
	if (!check()) {
		for (int i = 0; i < 4; i++) {
			field[b[i].y][b[i].x] = shape + 1;
		}

		if (!gameOver) {
			spawnFigure();
		}
	}

	// Check lines
	int k = M - 1;
	for (int i = M - 1; i > 0; i--) {
		int count = 0;

		for (int j = 0; j < N; j++) {
			if (field[i][j]) {
				count++;
			}

			field[k][j] = field[i][j];
		}

		if (count < N) {
			k--;
		}
	}

	// Redraw
	glutPostRedisplay();

	// Reset the timer
	if (!gameOver) {
		glutTimerFunc(delay, timerFunc, 0);
	}
}

void move(int x) {
	// Movement
	for (int i = 0; i < 4; i++) {
		b[i] = a[i];
		a[i].x += x;
	}

	// Reset the figure if it's out of bounds
	if (!check()) {
		for (int i = 0; i < 4; i++) {
			a[i] = b[i];
		}
	}

	// Redraw
	glutPostRedisplay();
}

void rotate(void) {
	// Don't rotate if the figure is an O
	if (shape != 1) {
		// Set rotation center
		struct point p = a[1];

		// Rotate each square around the rotation center
		for (int i = 0; i < 4; i++) {
			b[i] = a[i];
			int x = a[i].y - p.y;
			int y = a[i].x - p.x;
			a[i].x = p.x - x;
			a[i].y = p.y + y;
		}
	}

	// Reset the figure if it's out of bounds
	if (!check()) {
		for (int i = 0; i < 4; i++) {
			a[i] = b[i];
		}
	}

	// Redraw
	glutPostRedisplay();
}

void pressKey(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			exit(0);
			break;
		case 'w':
			rotate();
			break;
		case 'a':
			move(-1);
			break;
		case 'd':
			move(1);
			break;
		case 's':
			delay = 100;
			break;
	}
}

void releaseKey(unsigned char key, int x, int y) {
	if (key == 's') {
		delay = 300;
	}
}

void resizeWindow(int w, int h) {
	// Make viewport a square with the size of the smallest side
	int size = (w < h) ? w : h;
	glViewport(0, 0, size, size);
}

int main(int argc, char **argv) {
	// Init randomizer
	srand(time(0));

	generateFigure();
	spawnFigure();

	// Init GLUT and create a window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(640,640);
	glutCreateWindow("Tetris");

	// Register callbacks
	glutDisplayFunc(draw);
	glutReshapeFunc(resizeWindow);
	// glutIdleFunc(update);
	glutKeyboardFunc(pressKey);
	glutKeyboardUpFunc(releaseKey);
	glutTimerFunc(delay, timerFunc, 0);

	// Enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
