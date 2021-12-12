/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj� osi 
//  uk�adu wsp�rz�dnych dla rzutowania perspektywicznego

/*************************************************************************************/
#define _USE_MATH_DEFINES

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <iostream>

using namespace std;

typedef float point3[3];

static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
// inicjalizacja po�o�enia obserwatora

static GLfloat thetax = 0.0;   // k�t obrotu obiektu
static GLfloat thetay = 0.0;   // k�t obrotu obiektu
static GLfloat thetax1 = 0.0;   // k�t obrotu obiektu
static GLfloat thetay1 = 0.0;   // k�t obrotu obiektu

static GLfloat thetax2 = 0.0;   // k�t obrotu obiektu
static GLfloat thetay2 = 0.0;   // k�t obrotu obiektu

static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static GLfloat cameraz = 0.0;

static GLint status1 = 0;       // stan klawiszy myszy
static GLint status2 = 0;		// 0 - nie naci�ni�to �adnego klawisza
								// 1 - naci�ni�ty zosta� lewy klawisz
static GLint status3 = 1;		//model obrotu (1 - obr�t �wia�ami, 0 - obr�t kamer�)

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int y_pos_old = 0;

static int delta_x = 0;        // r�nica pomi�dzy pozycj� bie��c�
static int delta_y = 0;			// i poprzedni� kursora myszy

static int R1 = 10;

const int N = 200; //rozmiar tablicy wiercho�k�w NxN
static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // trzy k�ty obrotu
int model = 3;  // 1- punkty, 2- siatka, 3 - wype�nione tr�jk�ty

//tablice wsp�rz�dnych wierzcho�k�w
float x[N][N];
float y[N][N];
float z[N][N];

float Nx[N][N];
float Ny[N][N];
float Nz[N][N];



//tablice warto�ci RGB wierzcho�k�w
int R[N][N];
int G[N][N];
int B[N][N];


GLfloat light_position[] = { 10.0, 0.0, 10.0, 1.0 };
// po�o�enie �r�d�a
GLfloat light_position1[] = { -10.0, 0.0, -10.0, 1.0 };
// po�o�enie �r�d�a
/*************************************************************************************/

/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia warto�ci odpowiednich zmiennych globalnych

void Mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora
		y_pos_old = y;					 // jako pozycji poprzedniej
		status1 = 1;          // wci�ni�ty zosta� lewy klawisz myszy
	}

	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora
		y_pos_old = y;					 // jako pozycji poprzedniej
		status2 = 1;          // wci�ni�ty zosta� lewy klawisz myszy


	}

	else	status1 = status2 = 0;          // nie zosta� wci�ni�ty �aden klawisz
}

/*************************************************************************************/
// Funkcja "monitoruje" po�o�enie kursora myszy i ustawia warto�ci odpowiednich
// zmiennych globalnych

void Motion(GLsizei x, GLsizei y) {

	delta_x = x - x_pos_old;     // obliczenie r�nicy po�o�enia kursora myszy

	x_pos_old = x;            // podstawienie bie��cego po�o�enia jako poprzednie

	delta_y = y - y_pos_old;     // obliczenie r�nicy po�o�enia kursora myszy

	y_pos_old = y;            // podstawienie bie��cego po�o�enia jako poprzednie

	glutPostRedisplay();     // przerysowanie obrazu sceny
}

//Funkcja losuj�ca kolor dla ka�dego wierzcho�ka
void Colors() {
	for (int i = 0; i < N; i++) {
		for (int k = 0; k < N; k++) {
			//dla warto�ci RGB losuje 0 lub 1
			if (x[i][k] == 0 && z[i][k] == 0) {
				R[i][k] = 1;
				G[i][k] = 1;
				B[i][k] = 1;
			}
			else {
				R[i][k] = rand() % 2;
				G[i][k] = rand() % 2;
				B[i][k] = rand() % 2;
			}
		}
	}
}

void initEgg() {
	float u = 0, v = 0; //zminne u i v wykorzystywane w funkcjach okre�laj�cych wierzcho�ki jajka;
	float fN = N; //warto�� sta�ej N jako float
	float l=0;
	float xu;
	float yu;
	float zu;
	
	float xv;
	float yv;
	float zv;

	for (int i = 0; i < N; i++) {
		u = float(i / (fN)); //zmienna zmniejszana proporcjonalnie do sta�ej N, aby znajadowa�a si� w zakresie u�ytej funkcji [0,1]
		for (int k = 0; k < N; k++) {
			v = float(k / (fN)); //zmienna jest zmniejszana proporcjonalnie do sta�ej N, aby znajadowa�a si� w zakresie u�ytej funkcji [0,1]
			//na podstawie u�ytej funkcji wyzaczane s� wierzcho�ki 
			x[i][k] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
			y[i][k] = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5;
			z[i][k] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);

			xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
			yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
			zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);

			xv = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);
			yv = 0;
			zv = -M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);

			Nx[i][k] = yu * zv - zu * yv;
			Ny[i][k] = zu * xv - xu * zv;
			Nz[i][k] = xu * yv - yu * xv;

			l = sqrt(Nx[i][k] * Nx[i][k] + Ny[i][k] * Ny[i][k] + Nz[i][k] * Nz[i][k]);
			Nx[i][k] /= l;
			Ny[i][k] /= l;
			Nz[i][k] /= l;

			if (i > N / 2) {
				Nx[i][k] *= -1;
				Ny[i][k] *= -1;
				Nz[i][k] *= -1;
			}
		}
	}
	Colors();
	//wyznaczenie kolor�w wierzcho�k�w
}

//Funkcja rysuj�ca jajko na 3 r�zne sposoby
void Egg(void) {

	if (model == 1) { //punkty
		glColor3f(1, 1, 0); //kolor ryswoania jest ��ty
		for (int i = 0; i < N; i++) {
			for (int k = 0; k < N; k++) {
				glBegin(GL_POINTS);
				glVertex3f(x[i][k], y[i][k], z[i][k]);
				glEnd();
			}
		}
	}

	else if (model == 2) { //siatka
		glColor3f(1, 1, 0); //kolor ryswoania jest ��ty

		//rysowane s� pionowe linie siatki jajka
		for (int i = 0; i < N; i++) {
			for (int k = 0; k < N; k++) {
				glBegin(GL_LINES); //funkcja rysuje linie
				glVertex3f(x[i][k], y[i][k], z[i][k]);
				glVertex3f(x[(i + 1) % N][k], y[(i + 1) % N][k], z[(i + 1) % N][k]);
				glEnd();

			}
		}

		//rysowane s� uko�ne linie siatki jajka
		for (int k = 0; k < N - 1; k++) {
			int i;
			for (i = 0; i < (N - 1) / 2; i++) {
				glBegin(GL_LINES);
				glVertex3f(x[i + 1][k], y[i + 1][k], z[i + 1][k]);
				glVertex3f(x[i][k + 1], y[i][k + 1], z[i][k + 1]);
				glEnd();
			}
			for (; i < N - 1; i++) {
				glBegin(GL_LINES);
				glVertex3f(x[i][k], y[i][k], z[i][k]);
				glVertex3f(x[i + 1][k + 1], y[i + 1][k + 1], z[i + 1][k + 1]);
				glEnd();
			}
		}

		//rysowane s� uko�ne linie ��cz�ce kraw�dzie siatki v=0 i v=N-1
		//kierunek sko�nych lini jest zamieniany w po�owie wsp�rz�dnej "u" tablicy wierzcho�k�w,
		//aby kiernek wszystkich uko�nych lini na jajku by� taki sam
		int i;
		for (i = 0; i < N / 2; i++) {
			glBegin(GL_LINES);
			glVertex3f(x[i + 2][N - 1], y[i + 2][N - 1], z[i + 2][N - 1]);
			glVertex3f(x[N - 1 - i][0], y[N - 1 - i][0], z[N - 1 - i][0]);
			glEnd();
		}
		for (i; i < N - 1; i++) {
			glBegin(GL_LINES);
			glVertex3f(x[i + 1][N - 1], y[i + 1][N - 1], z[i + 1][N - 1]);
			glVertex3f(x[N - 2 - i][0], y[N - 2 - i][0], z[N - 2 - i][0]);
			glEnd();
		}

		//rysowane s� poziome linie siatki jajka
		for (int i = 0; i < N; i++) {
			for (int k = 0; k < N - 1; k++) {
				glBegin(GL_LINES);
				glVertex3f(x[i][k], y[i][k], z[i][k]);
				glVertex3f(x[i][k + 1], y[i][k + 1], z[i][k + 1]);
				glEnd();
			}
		}

		//rysowane s� poziome linie ��cz�ce kraw�dzie siatki v=0 i v=N-1
		for (int i = 0; i < N - 1; i++) {
			glBegin(GL_LINES);
			glVertex3f(x[i + 1][N - 1], y[i + 1][N - 1], z[i + 1][N - 1]);
			glVertex3f(x[N - 1 - i][0], y[N - 1 - i][0], z[N - 1 - i][0]);
			glEnd();
		}

	}

	else if (model == 3) { //wype�nione tr�jk�ty

		for (int i = 0; i < N - 1; i++) {
			for (int k = 0; k < N - 1; k++) {
				//rysowane s� 2 tr�jk�ty o wsp�lnym boku pomi�dzy 4 s�siaduj�cymi w tablicy wiercho�kami
				glBegin(GL_POLYGON);  //funkcja rysuje wielok�t
				glColor3f(R[i][k], G[i][k], B[i][k]);
				glNormal3f(Nx[i][k], Ny[i][k], Nz[i][k]);
				glVertex3f(x[i][k], y[i][k], z[i][k]);

				glColor3f(R[i + 1][k], G[i + 1][k], B[i + 1][k]);
				glNormal3f(Nx[i + 1][k], Ny[i + 1][k], Nz[i + 1][k]);
				glVertex3f(x[i + 1][k], y[i + 1][k], z[i + 1][k]);

				glColor3f(R[i][k + 1], G[i][k + 1], B[i][k + 1]);
				glNormal3f(Nx[i][k + 1], Ny[i][k + 1], Nz[i][k + 1]);
				glVertex3f(x[i][k + 1], y[i][k + 1], z[i][k + 1]);
				glEnd();

				glBegin(GL_POLYGON);
				glColor3f(R[i + 1][k + 1], G[i + 1][k + 1], B[i + 1][k + 1]);
				glNormal3f(Nx[i + 1][k + 1], Ny[i + 1][k + 1], Nz[i + 1][k + 1]);
				glVertex3f(x[i + 1][k + 1], y[i + 1][k + 1], z[i + 1][k + 1]);

				glColor3f(R[i + 1][k], G[i + 1][k], B[i + 1][k]);
				glNormal3f(Nx[i + 1][k], Ny[i + 1][k], Nz[i + 1][k]);
				glVertex3f(x[i + 1][k], y[i + 1][k], z[i + 1][k]);

				glColor3f(R[i][k + 1], G[i][k + 1], B[i][k + 1]);
				glNormal3f(Nx[i][k + 1], Ny[i][k + 1], Nz[i][k + 1]);
				glVertex3f(x[i][k + 1], y[i][k + 1], z[i][k + 1]);
				glEnd();
			}
		}

		//Rysowane s� tr�jk�ty pomi�dzy kraw�dziami u=0 i u=N-1
		for (int i = 0; i < N - 1; i++) {
			glBegin(GL_POLYGON);  //funkcja rysuje wielok�t
			glColor3f(R[0][i], G[0][i], B[0][i]);
			glNormal3f(Nx[0][i], Ny[0][i], Nz[0][i]);
			glVertex3f(x[0][i], y[0][i], z[0][i]);

			glColor3f(R[N - 1][i + 1], G[N - 1][i + 1], B[N - 1][i + 1]);
			glNormal3f(Nx[N - 1][i + 1], Ny[N - 1][i + 1], Nz[N - 1][i + 1]);
			glVertex3f(x[N - 1][i + 1], y[N - 1][i + 1], z[N - 1][i + 1]);

			glColor3f(R[N - 1][i], G[N - 1][i], B[N - 1][i]);
			glNormal3f(Nx[N - 1][i], Ny[N - 1][i], Nz[N - 1][i]);
			glVertex3f(x[N - 1][i], y[N - 1][i], z[N - 1][i]);
			glEnd();
		}

		// rysowane s� tr�jk�ty mi�dzy kraw�dziami v=0 i v=N-1
		for (int i = 0; i < N - 1; i++) {
			glBegin(GL_POLYGON);  //funkcja rysuje wielok�t
			glColor3f(R[i][N - 1], G[i][N - 1], B[i][N - 1]);
			glNormal3f(Nx[i][N - 1], Ny[i][N - 1], Nz[i][N - 1]);
			glVertex3f(x[i][N - 1], y[i][N - 1], z[i][N - 1]);

			glColor3f(R[i + 1][N - 1], G[i + 1][N - 1], B[i + 1][N - 1]);
			glNormal3f(Nx[i + 1][N - 1], Ny[i + 1][N - 1], Nz[i + 1][N - 1]);
			glVertex3f(x[i + 1][N - 1], y[i + 1][N - 1], z[i + 1][N - 1]);

			glColor3f(R[N - 1 - i][0], G[N - 1 - i][0], B[N - 1 - i][0]);
			glNormal3f(Nx[N - 1 - i][0], Ny[N - 1 - i][0], Nz[N - 1 - i][0]);
			glVertex3f(x[N - 1 - i][0], y[N - 1 - i][0], z[N - 1 - i][0]);
			glEnd();
		}
		for (int i = 0; i < N - 1; i++) {
			glBegin(GL_POLYGON);  //funkcja rysuje wielok�t
			glColor3f(R[i + 1][N - 1], G[i + 1][N - 1], B[i + 1][N - 1]);
			glNormal3f(Nx[i + 1][N - 1], Ny[i + 1][N - 1], Nz[i + 1][N - 1]);
			glVertex3f(x[i + 1][N - 1], y[i + 1][N - 1], z[i + 1][N - 1]);

			glColor3f(R[N - 1 - i][0], G[N - 1 - i][0], B[N - 1 - i][0]);
			glNormal3f(Nx[N - 1 - i][0], Ny[N - 1 - i][0], Nz[N - 1 - i][0]);
			glVertex3f(x[N - 1 - i][0], y[N - 1 - i][0], z[N - 1 - i][0]);

			glColor3f(R[N - 2 - i][0], G[N - 2 - i][0], B[N - 2 - i][0]);
			glNormal3f(Nx[N - 2 - i][0], Ny[N - 2 - i][0], Nz[N - 2 - i][0]);
			glVertex3f(x[N - 2 - i][0], y[N - 2 - i][0], z[N - 2 - i][0]);
			glEnd();
		}
	}
}


// Funkcja okre�laj�ca co ma by� rysowane (zawsze wywo�ywana, gdy trzeba
// przerysowa� scen�)


void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym

	glLoadIdentity();
	// Czyszczenie macierzy bie??cej

	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// Zdefiniowanie po�o�enia obserwatora
	//Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej powy�ej

	if (status3 == 0) {							// je�li model obrou kamer�
		if (status1 == 1) {						// je�li lewy klawisz myszy wci�ni�ty
			thetax += delta_x * pix2angle / 20;	// modyfikacja k�ta obrotu o kat proporcjonalny do r�nicy po�o�e� kursora myszy
			thetay += delta_y * pix2angle / 20;
			if (thetay > M_PI / 2 - 0.000001) {
				thetay = M_PI / 2 - 0.000001;
			}
			else if (thetay < -M_PI / 2 + 0.000001) {
				thetay = -M_PI / 2 + 0.000001;
			}
		}
		else if (status2 == 1) {
			R1 += delta_y;
			if (R1 < 1) R1 = 1;
			else if (R1 > 25) R1 = 25;
		}


		viewer[0] = R1 * cos(thetax) * cos(thetay);
		viewer[1] = R1 * sin(thetay);
		viewer[2] = R1 * sin(thetax) * cos(thetay);
	}
	else if (status3 == 1) {						//je�li model obrotu �wiat�em
		if (status1 == 1) {							// je�li lewy klawisz myszy wci�ni�ty
			thetax1 += delta_x * pix2angle / 20;	// modyfikacja k�ta obrotu o kat proporcjonalny do r�nicy po�o�e� kursora myszy
			thetay1 += delta_y * pix2angle / 20;
			if (thetay1 > M_PI / 2 - 0.000001) {
				thetay1 = M_PI / 2 - 0.000001;
			}
			else if (thetay1 < -M_PI / 2 + 0.000001) {
				thetay1 = -M_PI / 2 + 0.000001;
			}
			light_position[0] = R1 * cos(-thetax1) * cos(-thetay1);
			light_position[1] = R1 * sin(-thetay1);
			light_position[2] = R1 * sin(-thetax1) * cos(-thetay1);
		}
		else if (status2 == 1) {
			thetax2 += delta_x * pix2angle / 20;	// modyfikacja k�ta obrotu o kat proporcjonalny do r�nicy po�o�e� kursora myszy
			thetay2 += delta_y * pix2angle / 20;
			if (thetay2 > M_PI / 2 - 0.000001) {
				thetay2 = M_PI / 2 - 0.000001;
			}
			else if (thetay2 < -M_PI / 2 + 0.000001) {
				thetay2 = -M_PI / 2 + 0.000001;
			}
			light_position1[0] = R1 * cos(-thetax2) * cos(-thetay2);
			light_position1[1] = R1 * sin(-thetay2);
			light_position1[2] = R1 * sin(-thetax2) * cos(-thetay2);
		}

	}
	glRotatef(90, 0.0, 1.0, 0.0);  //obr�t obiektu o nowy k�t
	//glRotatef(thetay, 1.0, 0.0, 0.0);  //obr�t obiektu o nowy k�t

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glColor3f(1.0f, 1.0f, 1.0f);
	// Ustawienie koloru rysowania na bia�y
	Egg();
	//glutSolidTeapot(3.0);
	// Narysowanie czajnika
	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania
	glutSwapBuffers();
}
/*************************************************************************************/

// Funkcja ustalaj�ca stan renderowania

void MyInit(void)
{
/*************************************************************************************/
// Definicja materia�u z jakiego zrobiony jest czajnik

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki ka =[kar,kag,kab] dla �wiat�a otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki kd =[kdr,kdg,kdb] �wiat�a rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wsp�czynniki ks =[ksr,ksg,ksb] dla �wiat�a odbitego               

	GLfloat mat_shininess = { 20.0 };
	// wsp�czynnik n opisuj�cy po�ysk powierzchni


/*************************************************************************************/
// Definicja �r�d�a �wiat�a


	//GLfloat light_position[] = { -10.0, 0.0, -10.0, 1.0 };
	// po�o�enie �r�d�a


	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = { 1.0, 1.0, 0.0, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant = { 1.0 };
	// sk�adowa sta�a ds dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

	GLfloat att_linear = { 0.05 };
	// sk�adowa liniowa dl dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

	GLfloat att_quadratic = { 0.001 };
	// sk�adowa kwadratowa dq dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

/*************************************************************************************/
// Ustawienie parametr�w materia�u i �r�d�a �wiat�a

/*************************************************************************************/
// Ustawienie patrametr�w materia�u


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	/*************************************************************************************/
	// Ustawienie parametr�w �r�d�a

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	/*************************************************************************************/
// Definicja �r�d�a �wiat�a


	


	GLfloat light_ambient1[] = { 0.1, 0.1, 0.1, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse1[] = { 0.0, 0.0, 1.0, 1.0 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular1[] = { 1.0, 1.0, 1.0, 0.5 };
	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant1 = { 1.0 };
	// sk�adowa sta�a ds dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

	GLfloat att_linear1 = { 0.05 };
	// sk�adowa liniowa dl dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

	GLfloat att_quadratic1 = { 0.001 };
	// sk�adowa kwadratowa dq dla modelu zmian o�wietlenia w funkcji
	// odleg�o�ci od �r�d�a

/*************************************************************************************/

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant1);
	//glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear1);
	//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic1);

	/*************************************************************************************/
	// Ustawienie opcji systemu o�wietlania sceny

	glShadeModel(GL_SMOOTH); // w�aczenie �agodnego cieniowania
	glEnable(GL_LIGHTING);   // w�aczenie systemu o�wietlenia sceny
	glEnable(GL_LIGHT0);     // w��czenie �r�d�a o numerze 0
	glEnable(GL_LIGHT1);     // w��czenie �r�d�a o numerze 1

	glEnable(GL_DEPTH_TEST); // w��czenie mechanizmu z-bufora

/*************************************************************************************/

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcz�cy (wype�nienia okna) ustawiono na czarny
	initEgg();
}

/*************************************************************************************/


// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych
// w przypadku zmiany rozmiar�w okna.
// Parametry vertical i horizontal (wysoko�� i szeroko�� okna) s� 
// przekazywane do funkcji za ka�dym razem gdy zmieni si� rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);
	// Prze��czenie macierzy bie��cej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bie��cej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametr�w dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielko�ci okna okna widoku (viewport) w zale�no�ci
	// relacji pomi�dzy wysoko�ci� i szeroko�ci� okna

	glMatrixMode(GL_MODELVIEW);
	// Prze��czenie macierzy bie��cej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bie��cej

}

//fynkcja zwortna wyznaczaj� model rysowania jajka
void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1; //punkty
	if (key == 's') model = 2; //siatka
	if (key == 'w') model = 3; //wypoe�nione tr�jk�ty
	if (key == 'r') status3 = 0; //rotacja kamery
	if (key == 'l') status3 = 1; //rotacja �wiat�a

	RenderScene(); // przerysowanie obrazu sceny
}

/*************************************************************************************/

// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli

void main(void)
{

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Rzutowanie perspektywiczne");

	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn�
	// (callback function).  B�dzie ona wywo�ywana za ka�dym razem
	// gdy zajdzie potrzeba przerysowania okna


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn�
	// za zmiany rozmiaru okna                       


	MyInit();
	// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst�pieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// W��czenie mechanizmu usuwania niewidocznych element�w sceny
	glutKeyboardFunc(keys);
	glutMouseFunc(Mouse);
	// Ustala funkcj� zwrotn� odpowiedzialn� za badanie ruchu myszy
	glutMotionFunc(Motion);

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}

/*************************************************************************************/