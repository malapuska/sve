#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")

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

	tekstura = LoadTexture("OpenGL.bmp");

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
	SetWhiteLight();

	// LIGHT1 - spot koji prati posmatraca
	// postavljamo PRE kamere da ostane na mestu posmatraca
	// pozicija (0,0,0) u prostoru kamere = mesto posmatraca
	float pozLight1[] = { 0.0f, 0.0f,  0.0f, 1.0f };
	// pravac (0,0,-1) u prostoru kamere = pravac pogleda
	float dirLight1[] = { 0.0f, 0.0f, -1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, pozLight1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dirLight1);

	// kamera
	UpdateCamera();

	// LIGHT0 - fiksno na (5, 20, 0) u svetskom koordinatnom sistemu
	// postavljamo POSLE kamere
	float pozLight0[] = { 5.0f, 20.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, pozLight0);

	// kompozitna kocka 3x3x3, elementarna kocka stranice 5
	DrawRubikCube(3, 3);

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
	gluPerspective(40, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &tekstura);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
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

void CGLRenderer::DrawCube(float a, float tx0, float ty0, float tx1, float ty1)
{
	float h = a / 2.0f;

	// prednja strana - crvena
	SetMaterial(1.0f, 0.0f, 0.0f);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(tx0, ty1); glVertex3f(-h, -h, h);  // donji levi
	glTexCoord2f(tx1, ty1); glVertex3f(h, -h, h);  // donji desni
	glTexCoord2f(tx1, ty0); glVertex3f(h, h, h);  // gornji desni
	glTexCoord2f(tx0, ty0); glVertex3f(-h, h, h);  // gornji levi
	glEnd();

	// zadnja strana - narandzasta
	SetMaterial(1.0f, 0.5f, 0.0f);
	glNormal3f(0, 0, -1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(h, -h, -h);
	glTexCoord2f(1, 1); glVertex3f(-h, -h, -h);
	glTexCoord2f(1, 0); glVertex3f(-h, h, -h);
	glTexCoord2f(0, 0); glVertex3f(h, h, -h);
	glEnd();

	// desna strana - plava
	SetMaterial(0.0f, 0.0f, 1.0f);
	glNormal3f(1, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(h, -h, h);
	glTexCoord2f(1, 1); glVertex3f(h, -h, -h);
	glTexCoord2f(1, 0); glVertex3f(h, h, -h);
	glTexCoord2f(0, 0); glVertex3f(h, h, h);
	glEnd();

	// leva strana - zelena
	SetMaterial(0.0f, 1.0f, 0.0f);
	glNormal3f(-1, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-h, -h, -h);
	glTexCoord2f(1, 1); glVertex3f(-h, -h, h);
	glTexCoord2f(1, 0); glVertex3f(-h, h, h);
	glTexCoord2f(0, 0); glVertex3f(-h, h, -h);
	glEnd();

	// gornja strana - bela
	SetMaterial(1.0f, 1.0f, 1.0f);
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-h, h, h);
	glTexCoord2f(1, 1); glVertex3f(h, h, h);
	glTexCoord2f(1, 0); glVertex3f(h, h, -h);
	glTexCoord2f(0, 0); glVertex3f(-h, h, -h);
	glEnd();

	// donja strana - zuta
	SetMaterial(1.0f, 1.0f, 0.0f);
	glNormal3f(0, -1, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-h, -h, -h);
	glTexCoord2f(1, 1); glVertex3f(h, -h, -h);
	glTexCoord2f(1, 0); glVertex3f(h, -h, h);
	glTexCoord2f(0, 0); glVertex3f(-h, -h, h);
	glEnd();
}

void CGLRenderer::DrawRubikCube(double a, int count)
{
	float razmak = a * 1.05;						// koliko kockica zauzima sa razmakom
	float ukupno = count * razmak;					// sirina cele kocke
	float offset = ukupno / 2.0 - razmak / 2.0;		// ovo da bi centar rubikove kocke bio u 0,0,0 koordinatnog pocetka da bi transformacije bile lakse
													// centar kocke - centar centralne kockice
													// pa zato oduzimamo ukupno / 2 i pola od jedne kockice da bi bili u centar kockice a ne u njen ugao

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tekstura);

	for (int x = 0; x < count; x++)
	{
		glPushMatrix();
		glRotatef(ugao[x], 1, 0, 0);

		for (int y = 0; y < count; y++)
		{
			for (int z = 0; z < count; z++)
			{
				float px = x * razmak - offset;
				float py = y * razmak - offset;
				float pz = z * razmak - offset;

				// tx ide s leva na desno normalno
				float tx0 = (float)x / count;
				float tx1 = (float)(x + 1) / count;

				float ty0 = (float)(count - 1 - y) / count;  // gornji deo teksture
				float ty1 = (float)(count - y) / count;      // donji deo teksture

				glPushMatrix();
				glTranslatef(px, py, pz);
				DrawCube(a, tx0, ty0, tx1, ty1);
				glPopMatrix();
			}
		}
		glPopMatrix();
	}

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::SetWhiteLight()
{
	// globalno ambijentalno osvetljenje povecano na 0.5
	float globalAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	// lokalni posmatrač - tacniji odsjaj
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	float belaSvetlost[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float nizAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	// ================================================
	// LIGHT0 - tackasti izvor
	// pozicija se postavlja u DrawScene posle kamere
	// w=1 znaci poziciono (tackasto) svetlo
	// ================================================
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, nizAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, belaSvetlost);
	glLightfv(GL_LIGHT0, GL_SPECULAR, belaSvetlost);

	// ================================================
	// LIGHT1 - spot (usmereni) izvor
	// pozicija i pravac se postavljaju u DrawScene
	// ================================================
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, nizAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, belaSvetlost);
	glLightfv(GL_LIGHT1, GL_SPECULAR, belaSvetlost);

	// ugao konusa spot svetla - 13 stepeni
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 13.0f);
	// koncentracija svetlosti unutar konusa - veci broj = uzi snop
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f);
}

void CGLRenderer::SetMaterial(float r, float g, float b)
{
	// difuzna i spekularna = prosledjena boja
	float difuzna[] = { r, g, b, 1.0f };
	float spekularna[] = { r, g, b, 1.0f };

	// ambijentalna = 20% difuzne
	float ambijentalna[] = { r * 0.2f, g * 0.2f, b * 0.2f, 1.0f };

	// nema emisione komponente
	float emisiona[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, difuzna);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spekularna);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambijentalna);
	glMaterialfv(GL_FRONT, GL_EMISSION, emisiona);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0f);
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}
