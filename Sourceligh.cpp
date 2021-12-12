/*************************************************************************************/

//  Szkielet programu do tworzenia modelu sceny 3-D z wizualizacj¹ osi 
//  uk³adu wspó³rzêdnych dla rzutowania perspektywicznego

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
// inicjalizacja po³o¿enia obserwatora

static GLfloat thetax = 0.0;   // k¹t obrotu obiektu
static GLfloat thetay = 0.0;   // k¹t obrotu obiektu
static GLfloat thetax1 = 0.0;   // k¹t obrotu obiektu
static GLfloat thetay1 = 0.0;   // k¹t obrotu obiektu

static GLfloat thetax2 = 0.0;   // k¹t obrotu obiektu
static GLfloat thetay2 = 0.0;   // k¹t obrotu obiektu

static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static GLfloat cameraz = 0.0;

static GLint status1 = 0;       // stan klawiszy myszy
static GLint status2 = 0;		// 0 - nie naciœniêto ¿adnego klawisza
								// 1 - naciœniêty zostaæ lewy klawisz
static GLint status3 = 1;		//model obrotu (1 - obrót œwia³ami, 0 - obrót kamer¹)

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int y_pos_old = 0;

static int delta_x = 0;        // ró¿nica pomiêdzy pozycj¹ bie¿¹c¹
static int delta_y = 0;			// i poprzedni¹ kursora myszy

static int R1 = 10;

const int N = 200; //rozmiar tablicy wiercho³ków NxN
static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // trzy k¹ty obrotu
int model = 3;  // 1- punkty, 2- siatka, 3 - wype³nione trójk¹ty

//tablice wspó³rzêdnych wierzcho³ków
float x[N][N];
float y[N][N];
float z[N][N];

float Nx[N][N];
float Ny[N][N];
float Nz[N][N];



//tablice wartoœci RGB wierzcho³ków
int R[N][N];
int G[N][N];
int B[N][N];


GLfloat light_position[] = { 10.0, 0.0, 10.0, 1.0 };
// po³o¿enie Ÿród³a
GLfloat light_position1[] = { -10.0, 0.0, -10.0, 1.0 };
// po³o¿enie Ÿród³a
/*************************************************************************************/

/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia wartoœci odpowiednich zmiennych globalnych

void Mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora
		y_pos_old = y;					 // jako pozycji poprzedniej
		status1 = 1;          // wciêniêty zosta³ lewy klawisz myszy
	}

	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora
		y_pos_old = y;					 // jako pozycji poprzedniej
		status2 = 1;          // wciêniêty zosta³ lewy klawisz myszy


	}

	else	status1 = status2 = 0;          // nie zosta³ wciêniêty ¿aden klawisz
}

/*************************************************************************************/
// Funkcja "monitoruje" po³o¿enie kursora myszy i ustawia wartoœci odpowiednich
// zmiennych globalnych

void Motion(GLsizei x, GLsizei y) {

	delta_x = x - x_pos_old;     // obliczenie ró¿nicy po³o¿enia kursora myszy

	x_pos_old = x;            // podstawienie bie¿¹cego po³o¿enia jako poprzednie

	delta_y = y - y_pos_old;     // obliczenie ró¿nicy po³o¿enia kursora myszy

	y_pos_old = y;            // podstawienie bie¿¹cego po³o¿enia jako poprzednie

	glutPostRedisplay();     // przerysowanie obrazu sceny
}

//Funkcja losuj¹ca kolor dla ka¿dego wierzcho³ka
void Colors() {
	for (int i = 0; i < N; i++) {
		for (int k = 0; k < N; k++) {
			//dla wartoœci RGB losuje 0 lub 1
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
	float u = 0, v = 0; //zminne u i v wykorzystywane w funkcjach okreœlaj¹cych wierzcho³ki jajka;
	float fN = N; //wartoœæ sta³ej N jako float
	float l=0;
	float xu;
	float yu;
	float zu;
	
	float xv;
	float yv;
	float zv;

	for (int i = 0; i < N; i++) {
		u = float(i / (fN)); //zmienna zmniejszana proporcjonalnie do sta³ej N, aby znajadowa³a siê w zakresie u¿ytej funkcji [0,1]
		for (int k = 0; k < N; k++) {
			v = float(k / (fN)); //zmienna jest zmniejszana proporcjonalnie do sta³ej N, aby znajadowa³a siê w zakresie u¿ytej funkcji [0,1]
			//na podstawie u¿ytej funkcji wyzaczane s¹ wierzcho³ki 
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
	//wyznaczenie kolorów wierzcho³ków
}

//Funkcja rysuj¹ca jajko na 3 rózne sposoby
void Egg(void) {

	if (model == 1) { //punkty
		glColor3f(1, 1, 0); //kolor ryswoania jest ¿ó³ty
		for (int i = 0; i < N; i++) {
			for (int k = 0; k < N; k++) {
				glBegin(GL_POINTS);
				glVertex3f(x[i][k], y[i][k], z[i][k]);
				glEnd();
			}
		}
	}

	else if (model == 2) { //siatka
		glColor3f(1, 1, 0); //kolor ryswoania jest ¿ó³ty

		//rysowane s¹ pionowe linie siatki jajka
		for (int i = 0; i < N; i++) {
			for (int k = 0; k < N; k++) {
				glBegin(GL_LINES); //funkcja rysuje linie
				glVertex3f(x[i][k], y[i][k], z[i][k]);
				glVertex3f(x[(i + 1) % N][k], y[(i + 1) % N][k], z[(i + 1) % N][k]);
				glEnd();

			}
		}

		//rysowane s¹ ukoœne linie siatki jajka
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

		//rysowane s¹ ukoœne linie ³¹cz¹ce krawêdzie siatki v=0 i v=N-1
		//kierunek skoœnych lini jest zamieniany w po³owie wspó³rzêdnej "u" tablicy wierzcho³ków,
		//aby kiernek wszystkich ukoœnych lini na jajku by³ taki sam
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

		//rysowane s¹ poziome linie siatki jajka
		for (int i = 0; i < N; i++) {
			for (int k = 0; k < N - 1; k++) {
				glBegin(GL_LINES);
				glVertex3f(x[i][k], y[i][k], z[i][k]);
				glVertex3f(x[i][k + 1], y[i][k + 1], z[i][k + 1]);
				glEnd();
			}
		}

		//rysowane s¹ poziome linie ³¹cz¹ce krawêdzie siatki v=0 i v=N-1
		for (int i = 0; i < N - 1; i++) {
			glBegin(GL_LINES);
			glVertex3f(x[i + 1][N - 1], y[i + 1][N - 1], z[i + 1][N - 1]);
			glVertex3f(x[N - 1 - i][0], y[N - 1 - i][0], z[N - 1 - i][0]);
			glEnd();
		}

	}

	else if (model == 3) { //wype³nione trójk¹ty

		for (int i = 0; i < N - 1; i++) {
			for (int k = 0; k < N - 1; k++) {
				//rysowane s¹ 2 trójk¹ty o wspólnym boku pomiêdzy 4 s¹siaduj¹cymi w tablicy wiercho³kami
				glBegin(GL_POLYGON);  //funkcja rysuje wielok¹t
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

		//Rysowane s¹ trójk¹ty pomiêdzy krawêdziami u=0 i u=N-1
		for (int i = 0; i < N - 1; i++) {
			glBegin(GL_POLYGON);  //funkcja rysuje wielok¹t
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

		// rysowane s¹ trójk¹ty miêdzy krawêdziami v=0 i v=N-1
		for (int i = 0; i < N - 1; i++) {
			glBegin(GL_POLYGON);  //funkcja rysuje wielok¹t
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
			glBegin(GL_POLYGON);  //funkcja rysuje wielok¹t
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


// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana, gdy trzeba
// przerysowaæ scenê)


void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym

	glLoadIdentity();
	// Czyszczenie macierzy bie??cej

	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// Zdefiniowanie po³o¿enia obserwatora
	//Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej powy¿ej

	if (status3 == 0) {							// jeœli model obrou kamer¹
		if (status1 == 1) {						// jeœli lewy klawisz myszy wciêniêty
			thetax += delta_x * pix2angle / 20;	// modyfikacja k¹ta obrotu o kat proporcjonalny do ró¿nicy po³o¿eñ kursora myszy
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
	else if (status3 == 1) {						//jeœli model obrotu œwiat³em
		if (status1 == 1) {							// jeœli lewy klawisz myszy wciêniêty
			thetax1 += delta_x * pix2angle / 20;	// modyfikacja k¹ta obrotu o kat proporcjonalny do ró¿nicy po³o¿eñ kursora myszy
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
			thetax2 += delta_x * pix2angle / 20;	// modyfikacja k¹ta obrotu o kat proporcjonalny do ró¿nicy po³o¿eñ kursora myszy
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
	glRotatef(90, 0.0, 1.0, 0.0);  //obrót obiektu o nowy k¹t
	//glRotatef(thetay, 1.0, 0.0, 0.0);  //obrót obiektu o nowy k¹t

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glColor3f(1.0f, 1.0f, 1.0f);
	// Ustawienie koloru rysowania na bia³y
	Egg();
	//glutSolidTeapot(3.0);
	// Narysowanie czajnika
	glFlush();
	// Przekazanie poleceñ rysuj¹cych do wykonania
	glutSwapBuffers();
}
/*************************************************************************************/

// Funkcja ustalaj¹ca stan renderowania

void MyInit(void)
{
/*************************************************************************************/
// Definicja materia³u z jakiego zrobiony jest czajnik

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia

	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego               

	GLfloat mat_shininess = { 20.0 };
	// wspó³czynnik n opisuj¹cy po³ysk powierzchni


/*************************************************************************************/
// Definicja Ÿród³a œwiat³a


	//GLfloat light_position[] = { -10.0, 0.0, -10.0, 1.0 };
	// po³o¿enie Ÿród³a


	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular[] = { 1.0, 1.0, 0.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant = { 1.0 };
	// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_linear = { 0.05 };
	// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_quadratic = { 0.001 };
	// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

/*************************************************************************************/
// Ustawienie parametrów materia³u i Ÿród³a œwiat³a

/*************************************************************************************/
// Ustawienie patrametrów materia³u


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	/*************************************************************************************/
	// Ustawienie parametrów Ÿród³a

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	/*************************************************************************************/
// Definicja Ÿród³a œwiat³a


	


	GLfloat light_ambient1[] = { 0.1, 0.1, 0.1, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse1[] = { 0.0, 0.0, 1.0, 1.0 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular1[] = { 1.0, 1.0, 1.0, 0.5 };
	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant1 = { 1.0 };
	// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_linear1 = { 0.05 };
	// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

	GLfloat att_quadratic1 = { 0.001 };
	// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
	// odleg³oœci od Ÿród³a

/*************************************************************************************/

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant1);
	//glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear1);
	//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic1);

	/*************************************************************************************/
	// Ustawienie opcji systemu oœwietlania sceny

	glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
	glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny
	glEnable(GL_LIGHT0);     // w³¹czenie Ÿród³a o numerze 0
	glEnable(GL_LIGHT1);     // w³¹czenie Ÿród³a o numerze 1

	glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora

/*************************************************************************************/

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcz¹cy (wype³nienia okna) ustawiono na czarny
	initEgg();
}

/*************************************************************************************/


// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokoœæ i szerokoœæ okna) s¹ 
// przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);
	// Prze³¹czenie macierzy bie¿¹cej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bie¿¹cej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci
	// relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna

	glMatrixMode(GL_MODELVIEW);
	// Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej

}

//fynkcja zwortna wyznaczaj¹ model rysowania jajka
void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1; //punkty
	if (key == 's') model = 2; //siatka
	if (key == 'w') model = 3; //wypoe³nione trójk¹ty
	if (key == 'r') status3 = 0; //rotacja kamery
	if (key == 'l') status3 = 1; //rotacja œwiat³a

	RenderScene(); // przerysowanie obrazu sceny
}

/*************************************************************************************/

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli

void main(void)
{

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Rzutowanie perspektywiczne");

	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹
	// (callback function).  Bêdzie ona wywo³ywana za ka¿dym razem
	// gdy zajdzie potrzeba przerysowania okna


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹
	// za zmiany rozmiaru okna                       


	MyInit();
	// Funkcja MyInit() (zdefiniowana powy¿ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst¹pieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// W³¹czenie mechanizmu usuwania niewidocznych elementów sceny
	glutKeyboardFunc(keys);
	glutMouseFunc(Mouse);
	// Ustala funkcjê zwrotn¹ odpowiedzialn¹ za badanie ruchu myszy
	glutMotionFunc(Motion);

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}

/*************************************************************************************/