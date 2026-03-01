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

	teksturaOkoline = LoadTexture("T1.jpg");

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// ovo neki glupi parametri iskreno ni ne znam sta rade ali valjda za ovog lokalnog posmatraca kao
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	float belaSvetlost[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//
	//float pozicijaLight0[] = { 0.0f, 0.0f, 1.0f, 0.0f };  // w=0 direkciono, z+ je iza
	//glLightfv(GL_LIGHT0, GL_POSITION, pozicijaLight0);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, belaSvetlost);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, belaSvetlost);

	UpdateCamera();

	float pozicijaLight1[] = { 0.0f, 5.0f, -10.0f, 1.0f };  // w=1 poziciono
	glLightfv(GL_LIGHT1, GL_POSITION, pozicijaLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, belaSvetlost);
	glLightfv(GL_LIGHT1, GL_SPECULAR, belaSvetlost);

	glDisable(GL_LIGHTING);  
	DrawWall(50.0);
	glEnable(GL_LIGHTING);

	SetMetalicMaterial();
	glPushMatrix();
	glRotatef(ugao, 0, 0, 1);  
	DrawGear(0.5, 1, 40, 0.5, 0.5);
	glPopMatrix();

	double R1 = R(0.5, 1, 40, 0.5);		// spoljasnji R centralnog
	double R2 = R(0.5, 2, 20, 0.5);		// spoljasnji R drugog
	double rastojanje = R1 + R2;		// da bi zupci bili u kontaktu

	SetMetalicMaterial();
	glPushMatrix();
	glTranslatef(rastojanje, 0, 0);   
	glRotatef(30, 0, 0, 1);            
	DrawGear(0.5, 2, 20, 0.5, 0.5);
	//NacrtajZvezdu(5, 3, 10, 3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 5.0f, -10.0f);  // pozicija light1

	// materijal koji isijava - emisiona komponenta je bela
	float emisija[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float bezBoje[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_EMISSION, emisija);   // isijava belu svetlost
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bezBoje);
	glMaterialfv(GL_FRONT, GL_AMBIENT, bezBoje);
	glMaterialfv(GL_FRONT, GL_SPECULAR, bezBoje);

	glutSolidSphere(0.5, 20, 20);

	// resetujemo emisionu komponentu da ne utice na ostale objekte
	float bezEmisije[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_EMISSION, bezEmisije);
	glPopMatrix();

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
	gluPerspective(45, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &teksturaOkoline);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::RotirajPogled(float deltaXY, float deltaXZ)
{
	ugaoKamereXY += deltaXY;
	ugaoKamereXZ += deltaXZ;

	if(ugaoKamereXZ > 90)
		ugaoKamereXZ = 90;

	if (ugaoKamereXZ < -90)
		ugaoKamereXZ = -90;

	PromeniPozicijuKamere();
}

void CGLRenderer::ZumirajPogled(float deltaUdaljenosti)
{
	udaljenostKamere += deltaUdaljenosti;

	if (udaljenostKamere < 2.0f)		// minimalni zoom
		udaljenostKamere = 2.0f;

	if (udaljenostKamere > 50.0f)		// maksimalni zoom
		udaljenostKamere = 50.0f;

	PromeniPozicijuKamere();
}

void CGLRenderer::PromeniPozicijuKamere()
{
	// na osnovu promenjenih parametara izracunavaju se potrebne koordinate za samu kameru

	okoKamereX = udaljenostKamere * cos(ugaoKamereXZ * TO_RAD) * sin(ugaoKamereXY * TO_RAD);
	okoKamereY = udaljenostKamere * sin(ugaoKamereXZ * TO_RAD);
	okoKamereZ = udaljenostKamere * cos(ugaoKamereXZ * TO_RAD) * cos(ugaoKamereXY * TO_RAD);
}

double CGLRenderer::R(double a, double alpha, int n, double h)
{
	// claude je generisao jer je ova funkcija trebala vec da bude u projektu data

	double period = a * (1.0 + alpha);
	double obim = n * period;
	return obim / (2.0 * M_PI);

	// PROBLEM JE U OVOJ GLUPOJ FUNKCIJI JER JE VLADAN NESTO IZMISLIO KO ZNA STA
	// I ONDA NI CLAUDE NE UME DA JE REKONSTRUISE I ONDA MI ZUPCI NE VALJAJU, NISU KAO NA SLICI
	// ALI LOGIKA U DRAWGEAR JE DOBRA SVE SAMO STO GA JEBE OVA FUNKCIJA
}

void CGLRenderer::NormCrossProd(double x1, double y1, double z1, double x2, double y2, double z2, double& x, double& y, double& z)
{
	// i ovo je claude generisao jer je i ovo trebalo da bude u projektu dato

	// fora je da raca normalan vektor na osnovu dva vektora koja su u ravni povrsine a onda se normalizuje da bi se dobio jedinicni vektor

	// vektorski proizvod v1 x v2
	x = y1 * z2 - z1 * y2;
	y = z1 * x2 - x1 * z2;
	z = x1 * y2 - y1 * x2;

	// normalizacija
	double len = sqrt(x * x + y * y + z * z);

	x /= len;
	y /= len;
	z /= len;
}

void CGLRenderer::DrawGear(double a, double alpha, int n, double h, double d)
{
	/*
			
				P1-----------P2
			   /               \
			  /                 \
			P0                   P3---------P4

	*/

	double spoljasnjiR = R(a, alpha, n, h);
	double unutrasnjiR = spoljasnjiR - h;

	double ugaoPerioda = 2.0 * M_PI / (double)n;								// ugao jednog perioda (zubac + udubljenje)
	double ugaoZupca = a / spoljasnjiR;											// ugao zupca na spoljasnjem poluprecniku
	double ugaoKosine = (ugaoPerioda - ugaoZupca) / (2.0 + alpha);									// ugao kosine (deo perioda koji nije zubac)
	double ugaoDna = ugaoPerioda - ugaoZupca - 2.0 * ugaoKosine;
	
	double prednjaStrana = d / 2.0;												// z koordinata prednje strane
	double zadnjaStrana = -d / 2.0;												// z koordinata zadnje strane

	double nx, ny, nz;															// komponente vektora

	double trenutniUgao = 0.0;

	for (int i = 0; i < n; i++)
	{
		// uglovi svih tacaka 
		double ugao0 = trenutniUgao;											// pocetak kosine 
		double ugao1 = trenutniUgao + ugaoKosine;								// pocetak zupca 
		double ugao2 = trenutniUgao + ugaoKosine + ugaoZupca;					// kraj zupca 
		double ugao3 = trenutniUgao + ugaoKosine + ugaoZupca + ugaoKosine;		// kraj kosine 
		double ugao4 = trenutniUgao + ugaoPerioda;								// kraj dna = pocetak sledeceg perioda 

		// x,y koordinate svih tacaka
		double x0 = unutrasnjiR * cos(ugao0), y0 = unutrasnjiR * sin(ugao0);	// levo dno kosine
		double x1 = spoljasnjiR * cos(ugao1), y1 = spoljasnjiR * sin(ugao1);	// levi deo zupca
		double x2 = spoljasnjiR * cos(ugao2), y2 = spoljasnjiR * sin(ugao2);	// desni deo zupca
		double x3 = unutrasnjiR * cos(ugao3), y3 = unutrasnjiR * sin(ugao3);	// desno dno kosine
		double x4 = unutrasnjiR * cos(ugao4), y4 = unutrasnjiR * sin(ugao4);	// kraj dna udubljenja

		// prednja strana - normala pokazuje u +z
		glNormal3f(0, 0, 1);
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(0, 0, prednjaStrana);										// centar
		glVertex3f(x0, y0, prednjaStrana);										// levo dno kosine
		glVertex3f(x1, y1, prednjaStrana);										// levi deo zupca
		glVertex3f(x2, y2, prednjaStrana);										// desni deo zupca
		glVertex3f(x3, y3, prednjaStrana);										// desno dno kosine
		glVertex3f(x4, y4, prednjaStrana);										// kraj dna udubljenja
		glEnd();

		// zadnja strana

		glNormal3f(0, 0, -1);
		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(0, 0, zadnjaStrana);
		glVertex3f(x4, y4, zadnjaStrana);  
		glVertex3f(x3, y3, zadnjaStrana);
		glVertex3f(x2, y2, zadnjaStrana);
		glVertex3f(x1, y1, zadnjaStrana);
		glVertex3f(x0, y0, zadnjaStrana);
		glEnd();

		// bocne strane - ovde ide malo seksa - normala se racuna preko "dobijene" funkcije

		// leva kosina P0 - P1
		NormCrossProd(x1 - x0, y1 - y0, 0, 0, 0, 1, nx, ny, nz);
		glBegin(GL_QUADS);
		glNormal3f(nx, ny, nz);
		glVertex3f(x0, y0, prednjaStrana);		
		glVertex3f(x0, y0, zadnjaStrana);
		glVertex3f(x1, y1, zadnjaStrana);
		glVertex3f(x1, y1, prednjaStrana);
		glEnd();

		// vrh zupca P1 - P2
		NormCrossProd(x2 - x1, y2 - y1, 0, 0, 0, 1, nx, ny, nz);
		glBegin(GL_QUADS);
		glNormal3f(nx, ny, nz);
		glVertex3f(x1, y1, prednjaStrana);
		glVertex3f(x1, y1, zadnjaStrana);
		glVertex3f(x2, y2, zadnjaStrana);
		glVertex3f(x2, y2, prednjaStrana);
		glEnd();

		// desna kosina P2 - P3
		NormCrossProd(x3 - x2, y3 - y2, 0, 0, 0, 1, nx, ny, nz);
		glBegin(GL_QUADS);
		glNormal3f(nx, ny, nz);
		glVertex3f(x2, y2, prednjaStrana);
		glVertex3f(x2, y2, zadnjaStrana);
		glVertex3f(x3, y3, zadnjaStrana);
		glVertex3f(x3, y3, prednjaStrana);
		glEnd();

		// dno udubljenja P3 - P4
		NormCrossProd(x4 - x3, y4 - y3, 0, 0, 0, 1, nx, ny, nz);
		glBegin(GL_QUADS);
		glNormal3f(nx, ny, nz);
		glVertex3f(x3, y3, prednjaStrana);
		glVertex3f(x3, y3, zadnjaStrana);
		glVertex3f(x4, y4, zadnjaStrana);
		glVertex3f(x4, y4, prednjaStrana);
		glEnd();

		trenutniUgao += ugaoPerioda;
	}

	// u sto jaka funkcija
}

void CGLRenderer::SetMetalicMaterial()
{
	float difuzna[] = { 0.0f, 0.0f, 0.5f, 1.0f };			// tamno plava difuziona komponenta
	float ambijentalna[] = { 0.0f, 0.0f, 0.2f, 1.0f };		// ambijentalna je malo tamnija od difuzne
	float spekularna[] = { 1.0f, 1.0f, 1.0f, 1.0f };		// beli odsjaj
	float sjaj = 128.0f;									// visoki sjaj

	glMaterialfv(GL_FRONT, GL_DIFFUSE, difuzna);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambijentalna);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spekularna);
	glMaterialf(GL_FRONT, GL_SHININESS, sjaj);
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

void CGLRenderer::NacrtajZvezdu(double r1, double r2, int n, double dubina)
{

	double korak = 2 * M_PI / (double)n;		// ugao izmedju dva zupca
	double polaKoraka = korak / 2;

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, dubina / 2);		

	double ugaoPret = 0;

	for (int i = 0; i <= n; i++)
	{
		double ugao = i * korak;
		glVertex3f(r1 * cos(ugao - polaKoraka), r1 * sin(ugao - polaKoraka), 0);
		glVertex3f(r2 * cos(ugao), r2 * sin(ugao), 0);
		ugaoPret = ugao;
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 0, -1);
	glVertex3f(0, 0, -dubina / 2);

	for (int i = 0; i <= n; i++)
	{
		double ugao = i * korak;
		glVertex3f(r1 * cos(ugao - polaKoraka), r1 * sin(ugao - polaKoraka), 0);
		glVertex3f(r2 * cos(ugao), r2 * sin(ugao), 0);
		ugaoPret = ugao;
	}
	glEnd();
}

void CGLRenderer::DrawWall(double a)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, teksturaOkoline);  // jedna tekstura za sve zidove
	glColor3f(1, 1, 1);

	// prednji zid 
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0); glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();

	// zadnji zid  
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(1, 1); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 1); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 0); glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	// levi zid
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0); glVertex3f(-a / 2, a / 2, -a / 2);
	glEnd();

	// desni zid 
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0); glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 1); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 0); glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	// nedostaje gornji i dodnji deo
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}
