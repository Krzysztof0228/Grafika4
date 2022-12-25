#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>

#define GLEW_STATIC
#define FREEGLUT_STATIC
#define M_PI 3.14159265358979323846

using namespace std;

typedef float point3[3];
static GLfloat viewer[] = { 0.0, 0.0, 10.0};

int model = 1; // 1-punkty, 2-siatka, 3-wypełnione trójkąty

static GLfloat theta = 0.0; //kąt obrotu obiektu
static GLfloat phi = 0.0;
static GLfloat R = 10.0;
static GLfloat pix2angle; //przelicznik pikseli na stopnie

static GLint status = 0; //stan klawiszy myszy
						 //0 - nie naciśnięto żadnego klawisza
						 //1 - naciśnięty został lewy klawisz

static int x_pos_old = 0; //poprzednia pozycja kursora myszy
static int y_pos_old = 0;

float fix = 1.0;

static int delta_x = 0; //różnica pomiędzy pozycją bieżącą, a poprzednią kursora myszy
static int delta_y = 0;

static GLfloat center_x = 0.0; //Punkt na, który potrzy obserwator
static GLfloat center_y = 0.0;
static GLfloat center_z = 0.0;

void Axes(void)
{
	//Początek i koniec obrazu osi x
	point3 x_min = { -5.0, 0.0, 0.0 };
	point3 x_max = { 5.0, 0.0, 0.0 };

	//Początek i koniec obrazu osi y
	point3 y_min = { 0.0, -5.0, 0.0 };
	point3 y_max = { 0.0, 5.0, 0.0 };

	//Początek i koniec obrazu osi z
	point3 z_min = { 0.0, 0.0, -5.0 };
	point3 z_max = { 0.0, 0.0, 5.0 };

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

// Wzory opisujące jajko
float policzX(float u, float v)
{
	float PIV = M_PI * v;
	return((-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(PIV));
}

float policzY(float u, float v)
{
	return(160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2));
}

float policzZ(float u, float v)
{
	float PIV = M_PI * v;
	return((-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(PIV));
}

struct Punkt
{
	float x;
	float y;
	float z;
	float xRGB;
	float yRGB;
	float zRGB;
};

const int N = 20;

struct Punkt Jajko[N][N];

float krok = 1.0 / N;

// Funkcja wypełniająca tablicę punktami jajka
void policz()
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			Jajko[i][j].x = policzX(i * krok, j * krok);
			Jajko[i][j].y = policzY(i * krok, j * krok);
			Jajko[i][j].z = policzZ(i * krok, j * krok);

			Jajko[i][j].xRGB = ((double)rand() / (RAND_MAX));
			Jajko[i][j].yRGB = ((double)rand() / (RAND_MAX));
			Jajko[i][j].zRGB = ((double)rand() / (RAND_MAX));
		}
	}
}

void Jajo()
{
	policz();

	// Model jajka narysowany za pomocą punktów
	if (model == 1)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				glBegin(GL_POINTS);
				glColor3f(1.0f, 1.0f, 1.0f);
				glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
				glEnd();
			}
		}
	}

	// Model jajka narysowany za pomocą siatki
	else if (model == 2)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				//linie pionowe
				if (i + 1 < N)
				{
					glBegin(GL_LINES);
					glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
					glVertex3f(Jajko[i + 1][j].x, Jajko[i + 1][j].y - 5, Jajko[i + 1][j].z);
					glEnd();
				}
				//dorysowanie linii pionowych na dole jajka
				else
				{
					glBegin(GL_LINES);
					glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
					glVertex3f(Jajko[0][0].x, Jajko[0][0].y - 5, Jajko[0][0].z);
					glEnd();
				}

				//linie poziome
				if (j + 1 < N)
				{
					glBegin(GL_LINES);
					glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
					glVertex3f(Jajko[i][j + 1].x, Jajko[i][j + 1].y - 5, Jajko[i][j + 1].z);
					glEnd();
				}
				//linie poziome łączące połowy jajka
				else
				{
					glBegin(GL_LINES);
					glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
					glVertex3f(-Jajko[i][0].x, Jajko[i][j].y - 5, 0);
					glEnd();
				}

				//linie ukośne dla jednej połowy jajka
				if (j + 1 < N && i != 0 && i < (int)N / 2 - 1)
				{
					glBegin(GL_LINES);
					glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
					glVertex3f(Jajko[i + 1][j + 1].x, Jajko[i + 1][j + 1].y - 5, Jajko[i + 1][j + 1].z);
					glEnd();
				}
				//linie ukośne dla drugiej połowy jajka
				if (j + 1 < N && i > 1 && i >= (int)N / 2 + 1)
				{
					glBegin(GL_LINES);
					glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
					glVertex3f(Jajko[i - 1][j + 1].x, Jajko[i - 1][j + 1].y - 5, Jajko[i - 1][j + 1].z);
					glEnd();
				}
				// linie ukośne łączące połowy jajka
				if (i != 0 && j == N - 2 && i < (int)N / 2 - 1)
				{
					glBegin(GL_LINES);
					glVertex3f(Jajko[i][N - 1].x, Jajko[i][N - 1].y - 5, Jajko[i][N - 1].z);
					glVertex3f(-Jajko[i + 1][0].x, Jajko[i + 1][j].y - 5, -Jajko[i + 1][0].z);
					glEnd();
				}

				if (i < (int)N / 2 - 1 && i != 0 && j == N - 2)
				{
					glBegin(GL_LINES);
					glVertex3f(-Jajko[i][N - 1].x, Jajko[i][N - 1].y - 5, -Jajko[i][N - 1].z);
					glVertex3f(Jajko[i + 1][0].x, Jajko[i + 1][0].y - 5, -Jajko[i + 1][0].z);
					glEnd();
				}
			}
		}
	}

	// Model jajka narysowany za pomocą trójkątów
	else if (model == 3)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				if (j < N - 1)
				{
					//Trójkąty połowy góry oraz dołu i trójkąty skośne w jedną stronę
					glBegin(GL_TRIANGLES);
					glColor3f(Jajko[i][j].xRGB, Jajko[i][j].yRGB, Jajko[i][j].zRGB);
					glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
					glColor3f(Jajko[(i + 1) % N][j].xRGB, Jajko[(i + 1) % N][j].yRGB, Jajko[(i + 1) % N][j].zRGB);
					glVertex3f(Jajko[(i + 1) % N][j].x, Jajko[(i + 1) % N][j].y - 5, Jajko[(i + 1) % N][j].z);
					glColor3f(Jajko[i][j + 1].xRGB, Jajko[i][j + 1].yRGB, Jajko[i][j + 1].zRGB);
					glVertex3f(Jajko[i][j + 1].x, Jajko[i][j + 1].y - 5, Jajko[i][j + 1].z);
					glEnd();
					//Rysowanie drugiej połowy dołu i góry oraz trójkąty skośne w drugą stronę
					glBegin(GL_TRIANGLES);
					glColor3f(Jajko[(i + 1) % N][j].xRGB, Jajko[(i + 1) % N][j].yRGB, Jajko[(i + 1) % N][j].zRGB);
					glVertex3f(Jajko[(i + 1) % N][j].x, Jajko[(i + 1) % N][j].y - 5, Jajko[(i + 1) % N][j].z);
					glColor3f(Jajko[(i + 1) % N][j + 1].xRGB, Jajko[(i + 1) % N][j + 1].yRGB, Jajko[(i + 1) % N][j + 1].zRGB);
					glVertex3f(Jajko[(i + 1) % N][j + 1].x, Jajko[(i + 1) % N][j + 1].y - 5, Jajko[(i + 1) % N][j + 1].z);
					glColor3f(Jajko[i][j + 1].xRGB, Jajko[i][j + 1].yRGB, Jajko[i][j + 1].zRGB);
					glVertex3f(Jajko[i][j + 1].x, Jajko[i][j + 1].y - 5, Jajko[i][j + 1].z);
					glEnd();
				}
				//Trójkąty łączące połowy jajka
				else
				{
					glBegin(GL_TRIANGLES);
					glColor3f(Jajko[(i + 1) % N][j].xRGB, Jajko[(i + 1) % N][j].yRGB, Jajko[(i + 1) % N][j].zRGB);
					glVertex3f(Jajko[(i + 1) % N][j].x, Jajko[(i + 1) % N][j].y - 5, Jajko[(i + 1) % N][j].z);
					glColor3f(Jajko[(N - i) % N][0].xRGB, Jajko[(N - i) % N][0].yRGB, Jajko[(N - i) % N][0].zRGB);
					glVertex3f(Jajko[(N - i) % N][0].x, Jajko[(N - i) % N][0].y - 5, Jajko[(N - i) % N][0].z);
					glColor3f(Jajko[N - i - 1][0].xRGB, Jajko[N - i - 1][0].yRGB, Jajko[N - i - 1][0].zRGB);
					glVertex3f(Jajko[N - i - 1][0].x, Jajko[N - i - 1][0].y - 5, Jajko[N - i - 1][0].z);
					glEnd();
				}
				if (i > 0)
				{
					glBegin(GL_TRIANGLES);
					glColor3f(Jajko[i][j].xRGB, Jajko[i][j].yRGB, Jajko[i][j].zRGB);
					glVertex3f(Jajko[i][j].x, Jajko[i][j].y - 5, Jajko[i][j].z);
					glColor3f(Jajko[(i + 1) % N][j].xRGB, Jajko[(i + 1) % N][j].yRGB, Jajko[(i + 1) % N][j].zRGB);
					glVertex3f(Jajko[(i + 1) % N][j].x, Jajko[(i + 1) % N][j].y - 5, Jajko[(i + 1) % N][j].z);
					glColor3f(Jajko[N - i][0].xRGB, Jajko[N - i][0].yRGB, Jajko[N - i][0].zRGB);
					glVertex3f(Jajko[N - i][0].x, Jajko[N - i][0].y - 5, Jajko[N - i][0].z);
					glEnd();
				}
			}
		}
	}
	else if (model == 4) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glutWireTeapot(3.0);
	}
}

// Funkcja odczytująca klawisze
void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1; // Punkty
	if (key == 'w') model = 2; // Linie
	if (key == 's') model = 3; // Trójkąty
	if (key == 'c') model = 4; // Czajnik
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(viewer[0], viewer[1], viewer[2], center_x, center_y, center_z, 0.0, fix, 0.0);
	Axes();

	if (model == 1 || model == 2 || model == 3)
	{
		if (status == 1)
		{
			theta += delta_x * pix2angle / 20.0;
			phi += delta_y * pix2angle / 20.0;
			if (phi > 2 * M_PI)
				phi = 0;
			if (phi < 0)
				phi = 2 * M_PI;
			if (phi > M_PI / 2)
				fix = -1.0;
			else
				fix = 1.0;
			if (phi > M_PI + (M_PI / 2))
				fix = 1.0;
		}
		else if (status == 2)
		{
			R += delta_y * pix2angle / 20.0;
		}

		viewer[0] = R * cos(theta) * cos(phi);
		viewer[1] = R * sin(phi);
		viewer[2] = R * sin(theta) * cos(phi);
	}
	
	else if (model == 4)
	{
		if (status == 1)
		{
			theta += delta_x * pix2angle;
			phi += delta_y * pix2angle;
		}
		else if (status == 2)
		{
			viewer[2] += delta_y * 0.2;

			if (viewer[2] < 0.5)
			{
				viewer[2] = 0.5;
			}

			if (viewer[2] > 25.0)
			{
				viewer[2] = 25.0;
			}
		}
		glRotatef(theta, 0.0, 1.0, 0.0); // Obrót obiektu o nowy kąt
		glRotatef(phi, 1.0, 0.0, 0.0);
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	Jajo();

	glFlush();

	glutSwapBuffers();
}

void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal; //przeliczanie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	//Ustawienie parametrów dla rzutu perspektywicznego
	gluPerspective(70, 1.0, 1.0, 30.0);

	if (horizontal <= vertical)
	{
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	}
	else
	{
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	}

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
}

void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) // Wciścnięty lewy klawisz
	{
		x_pos_old = x;
		y_pos_old = y;
		status = 1;
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) // Wciśnięty prawy klawisz
	{
		x_pos_old = x;
		y_pos_old = y;
		status = 2;
	}

	else status = 0; // Nie został wciśnięty żaden klawisz
}

void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old; // Obliczenie różnicy położenia kursora myszy
	delta_y = y - y_pos_old;
	x_pos_old = x; // Podstawienie bieżącego położenia jako poprzednie
	y_pos_old = y;
	glutPostRedisplay(); // Przerysowanie obrazu sceny
}

int main()
{
	cout << "p - punkty" << endl;
	cout << "w - siatka" << endl;
	cout << "s - trojkaty" << endl;
	cout << "c - czajnik" << endl;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Interakcja z uzytkownikiem");

	glutDisplayFunc(RenderScene);

	glutReshapeFunc(ChangeSize);

	glutMouseFunc(Mouse);

	glutMotionFunc(Motion);

	MyInit();

	glEnable(GL_DEPTH_TEST);

	glutKeyboardFunc(keys);

	glutMainLoop();

	return 0;
}
