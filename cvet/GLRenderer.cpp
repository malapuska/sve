#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")
#define _USE_MATH_DEFINES
#include <math.h>

#define TO_RAD 3.14 / 180.0f
#define TO_DEG 180.0f / 3.14

CGLRenderer::CGLRenderer(void)
{
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);

	okolina = LoadTexture("Env.jpg");

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// ovde se dalje radi sve ostalo iscrtavanje scene ----------

	/*
	
		!!!
		ovde sam vec odustao pa je i ovaj deo uradio claude i okej je sve to, radi 
		ali brate bas je ovaj blanket pretezak
		!!!

	*/

	 // --- SVETLO 0 - iza posmatraca, prati kameru ---
	// postavljamo PRE UpdateCamera, u koordinatnom sistemu kamere
	// direkciono svetlo ima w=0, pravac je "iza" tj. (0,0,1) u prostoru kamere
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// globalno ambijentalno osvetljenje
	float globalAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	float ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// light0 - iza posmatraca (u prostoru kamere z+ je iza)
	float light0Pos[] = { 0.0f, 0.0f, 1.0f, 0.0f };  // w=0 znaci direkciono
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

	// --- KAMERA ---
	UpdateCamera();

	// --- SVETLO 1 - fiksni pravac (0,1,1), postavljamo POSLE UpdateCamera ---
	// u svetskom koordinatnom sistemu
	float light1Pos[] = { 0.0f, 1.0f, 1.0f, 0.0f };  // w=0 znaci direkciono
	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);

	// --- KOORDINATNE OSE (opciono, za debug) ---
	// DrawAxes();

	// --- CVET 1 - u koordinatnom pocetku ---
	glPushMatrix();
	DrawFlower();
	glPopMatrix();

	// --- CVET 2 - pomeren 9 jedinica po X osi ---
	glPushMatrix();
	glTranslatef(9.0f, 0.0f, 0.0f);
	DrawFlower();
	glPopMatrix();

	// --- CVET 3 - pomeren 9 jedinica po Z osi ---
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 9.0f);
	DrawFlower();
	glPopMatrix();

	// --- SFERA SA TEKSTUROM (pozadinska, centrirana na kameru) ---
	// resetujemo translaciju kamere, zadrzavamo samo rotaciju
	glPushMatrix();
	glLoadIdentity();
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
	DrawSphere(500, 36, 18);
	glPopMatrix();

	// ----------------------------------------------------------

	glFlush();
	SwapBuffers(pDC->m_hDC);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &okolina);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
	glLineWidth(2);

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	glColor3f(0, 0, 1);	// y - crvena osa
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(1, 0, 0);	// z - zelena osa
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);

	glColor3f(0, 1, 0);	// x - plava osa
	glVertex3f(0, 0, 0);
	glVertex3f(10, 0, 0);

	glEnd();
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT idTeksture;
	DImage tekstura;
	tekstura.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &idTeksture);

	glBindTexture(GL_TEXTURE_2D, idTeksture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tekstura.Width(), tekstura.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, tekstura.GetDIBBits());
	return idTeksture;
}

void CGLRenderer::PolarToCertesian(double R, double phi, double theta, double& x, double& y, double& z)
{
	// theta = elevacija od ekvatora (-pi/2 do pi/2)
	x = R * cos(theta) * cos(phi);
	y = R * sin(theta);
	z = R * cos(theta) * sin(phi);
}

void CGLRenderer::DrawSphere(float R, int n, int m)
{
	// ova funkcija radi tako sto se ide od juga ka severu lopte po prstenovima 
	// spolja petlja prelazi sa prstana na prsten a unutrasnja radi jedan prsten

	double dphi = 2 * M_PI / (double)n;
	double dTheta = M_PI / (double)m;

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, okolina);

	double theta = -M_PI / 2.0;
	
	for (int i = 0; i < m; i++)
	{
		double phi = 0.0;
		glBegin(GL_QUAD_STRIP);

		for (int j = 0; j <= n; j++)
		{
			double x1, y1, z1;
			PolarToCertesian(R, phi, theta, x1, y1, z1);

			double x2, y2, z2;
			PolarToCertesian(R, phi, theta + dTheta, x2, y2, z2);

			double tx = (double)j / (double)n;
			double ty1 = (double)i / (double)m;
			double ty2 = (double)(i + 1) / (double)m;

			glTexCoord2f(tx, ty1);
			glVertex3f(x1, y1, z1);

			glTexCoord2f(tx, ty2);
			glVertex3f(x2, y2, z2);

			phi += dphi;
		}

		glEnd();
		theta += dTheta;
	}

	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::CalcRotAxis(double x1, double y1, double z1, double x2, double y2, double z2, double& x3, double& y3, double& z3)
{
	// vektor od tacke 1 do tacke 2
	double vx = x2 - x1;
	double vy = y2 - y1;
	double vz = z2 - z1;

	// duzina vektora
	double len = sqrt(vx * vx + vy * vy + vz * vz);

	// normalizacija
	x3 = vx / len;
	y3 = vy / len;
	z3 = vz / len;
}

void CGLRenderer::DrawSphFlower(float R, int n, int m, float factor, unsigned char R1, unsigned char G1, unsigned char B1, unsigned char R2, unsigned char G2, unsigned char B2)
{
	// !!!
	// ovo mi je napisao claude i kao mi je jasno kad citam liniju po liniju sa komentarima ali ovo je strasno stvarno
	// stvarno ne znam kako im nije bilo zao da daju ovako nesto jer je bas hardcore
	// !!!

	double dPhi = 2 * M_PI / (double)n;		// korak ugla po ekvatoru (izmedju segmenata)
	double dTheta = M_PI / (double)m;		// korak ugla po meridijanu (po visini)

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	// ambijentalna komponenta = 20% boje juznog pola
	float ambient[] = { R1 / 255.0f * 0.2f, G1 / 255.0f * 0.2f, B1 / 255.0f * 0.2f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	// spekularna (refleksija) je bela
	float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	float shininess = 64.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	double phi = 0.0;

	// SPOLJNA PETLJA - svaka iteracija je jedan vertikalni segment (latica)
	for (int i = 0; i < n; i++)
	{
		glPushMatrix(); // push za ceo segment - na kraju ce se resetovati za sledeci segment

		double theta = -M_PI / 2.0; // krece od juznog pola

		// UNUTRASNJA PETLJA - svaka iteracija je jedan horizontalni pojas unutar segmenta
		for (int j = 0; j < m; j++)
		{
			// donja dva temena ovog pojasa - oko njih se rotira ovaj i svi gornji pojasevi
			double bx1, by1, bz1;						// levo donje teme pojasa
			double bx2, by2, bz2;						// desno donje teme pojasa
			PolarToCertesian(R, phi, theta, bx1, by1, bz1);
			PolarToCertesian(R, phi + dPhi, theta, bx2, by2, bz2);

			// osa rotacije kroz ta dva donja temena
			double ax, ay, az;
			CalcRotAxis(bx1, by1, bz1, bx2, by2, bz2, ax, ay, az);

			// rotiramo koordinatni sistem kumulativno - bez push/pop pa se akumulira
			// svaki pojas rotira malo, gornji nasledjuju rotaciju donjih
			glTranslatef(bx1, by1, bz1);
			glRotatef(ugaoOtvaranja * factor / m, ax, ay, az); // mali ugao po pojasu
			glTranslatef(-bx1, -by1, -bz1);

			// interpolacija boje za ovaj pojas
			double t = (double)j / (double)m; // 0 = jug, 1 = sever
			float dr = R1 / 255.0f + t * (R2 / 255.0f - R1 / 255.0f);
			float dg = G1 / 255.0f + t * (G2 / 255.0f - G1 / 255.0f);
			float db = B1 / 255.0f + t * (B2 / 255.0f - B1 / 255.0f);
			glColor3f(dr, dg, db);

			// crtamo jedan quad (cetiri temena)
			double x1, y1, z1;				// levo donje
			double x2, y2, z2;				// desno donje
			double x3, y3, z3;				// levo gore
			double x4, y4, z4;				// desno gore
			PolarToCertesian(R, phi, theta, x1, y1, z1);
			PolarToCertesian(R, phi + dPhi, theta, x2, y2, z2);
			PolarToCertesian(R, phi, theta + dTheta, x3, y3, z3);
			PolarToCertesian(R, phi + dPhi, theta + dTheta, x4, y4, z4);

			glBegin(GL_QUADS);
			glNormal3f(x1 / R, y1 / R, z1 / R); glVertex3f(x1, y1, z1);
			glNormal3f(x2 / R, y2 / R, z2 / R); glVertex3f(x2, y2, z2);
			glNormal3f(x4 / R, y4 / R, z4 / R); glVertex3f(x4, y4, z4);
			glNormal3f(x3 / R, y3 / R, z3 / R); glVertex3f(x3, y3, z3);
			glEnd();

			theta += dTheta; // prelazimo na sledeci pojas
		}

		glPopMatrix(); // resetujemo sve transformacije ovog segmenta za sledeci

		phi += dPhi; // sledeci segment
	}

	glDisable(GL_COLOR_MATERIAL);
}

void CGLRenderer::DrawFlower()
{
	DrawSphFlower(2, 26, 18, 1, 0, 192, 0, 0, 255, 0);

	glPushMatrix();
	glTranslatef(0.0f, -0.5f, 0.0f);
	DrawSphFlower(1.5f, 36, 18, 0.5f, 0, 0, 255, 255, 0, 0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -1.0f, 0.0f);
	DrawSphFlower(1.0f, 36, 18, 0.25f, 192, 192, 0, 192, 192, 192);
	glPopMatrix();
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}
