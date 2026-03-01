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

	cigle = LoadTexture("brick.png");
	okolina = LoadTexture("env.png");

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	UpdateCamera();

	// ovde se dalje radi sve ostalo iscrtavanje scene ----------

	glEnable(GL_LIGHTING);

	GLfloat lightPos[] = { 1.0f, 1.0f, 0.0f, 0.0f };  
	GLfloat lightWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightWhite);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightWhite);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glEnable(GL_LIGHT0);

	GLfloat matWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matWhite);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matWhite);

	glColor3f(1, 1, 1);

	glTranslated(0, -10, 0);

	DrawTube(2.5, 3.5, 10, 32);
	glTranslated(0, 10, 0);
	DrawCone(3.8, 2, 32);

	glPushMatrix();

	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 3.8, 0);
	glRotatef(ugaoRotacije, 0, 1, 0);

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(i * 90, 0, 1, 0);
		glRotatef(90, 1, 0, 0);
		DrawPaddle(8, 1.5);
		glPopMatrix();
	}

	glPopMatrix();

	glDisable(GL_LIGHTING);
	glTranslated(0, 20, 0);
	DrawCube(60);

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
	
	glDeleteTextures(1, &cigle);
	glDeleteTextures(1, &okolina);

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

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tekstura.Width(), tekstura.Height(), GL_RGBA, GL_UNSIGNED_BYTE, tekstura.GetDIBBits());
	return idTeksture;
}

void CGLRenderer::DrawCube(double a)
{
	double h = a / 2.0;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, okolina);
	glColor3f(1, 1, 1);


	// --- PREDNJA strana (FRONT) --- col=1, row=0
	// U: [1/4, 2/4], V: [0, 1/3]
	glBegin(GL_QUADS);
	glTexCoord2f(1.0 / 4, 0.0);			glVertex3f(-h, h, -h);
	glTexCoord2f(1.0 / 4, 1.0 / 3);     glVertex3f(-h, -h, -h);
	glTexCoord2f(2.0 / 4, 1.0 / 3);     glVertex3f(h, -h, -h);
	glTexCoord2f(2.0 / 4, 0.0);			glVertex3f(h, h, -h);
	glEnd();

	// --- DESNA strana (RIGHT) --- col=2, row=1
	// U: [2/4, 3/4], V: [1/3, 2/3]
	glBegin(GL_QUADS);
	glTexCoord2f(2.0 / 4, 1.0 / 3);     glVertex3f(h, -h, -h);
	glTexCoord2f(2.0 / 4, 2.0 / 3);     glVertex3f(h, -h, h);
	glTexCoord2f(3.0 / 4, 2.0 / 3);     glVertex3f(h, h, h);
	glTexCoord2f(3.0 / 4, 1.0 / 3);     glVertex3f(h, h, -h);
	glEnd();

	// --- ZADNJA strana (BACK) --- col=1, row=3
	// U: [1/4, 2/4], V: [2/3, 3/3]
	glBegin(GL_QUADS);
	glTexCoord2f(1.0 / 4, 2.0 / 3);     glVertex3f(-h, -h, h);
	glTexCoord2f(1.0 / 4, 3.0 / 3);     glVertex3f(-h, h, h);
	glTexCoord2f(2.0 / 4, 3.0 / 3);     glVertex3f(h, h, h);
	glTexCoord2f(2.0 / 4, 2.0 / 3);     glVertex3f(h, -h, h);
	glEnd();

	// --- LEVA strana (LEFT) --- col=0, row=1
	// U: [0, 1/4], V: [1/3, 2/3]
	glBegin(GL_QUADS);
	glTexCoord2f(1.0 / 4, 1.0 / 3);		glVertex3f(-h, -h, -h);
	glTexCoord2f(1.0 / 4, 2.0 / 3);		glVertex3f(-h, -h, h);
	glTexCoord2f(0.0, 2.0 / 3);			glVertex3f(-h, h, h);
	glTexCoord2f(0.0, 1.0 / 3);			glVertex3f(-h, h, -h);
	glEnd();

	// --- GORNJA strana (TOP) --- col=4, row=1
	// U: [3/4, 4/4], V: [1/3, 2/3]
	glBegin(GL_QUADS);
	glTexCoord2f(4.0 / 4, 1.0 / 3);     glVertex3f(-h, h, -h);
	glTexCoord2f(4.0 / 4, 2.0 / 3);     glVertex3f(-h, h, h);
	glTexCoord2f(3.0 / 4, 2.0 / 3);     glVertex3f(h, h, h);
	glTexCoord2f(3.0 / 4, 1.0 / 3);     glVertex3f(h, h, -h);
	glEnd();

	// --- DONJA strana (BOTTOM) --- col=1, row=1
	// U: [1/4, 2/4], V: [1/3, 2/3]
	glBegin(GL_QUADS);
	glTexCoord2f(1.0 / 4, 1.0 / 3);     glVertex3f(-h, -h, -h);
	glTexCoord2f(1.0 / 4, 2.0 / 3);     glVertex3f(-h, -h, h);
	glTexCoord2f(2.0 / 4, 2.0 / 3);     glVertex3f(h, -h, h);
	glTexCoord2f(2.0 / 4, 1.0 / 3);     glVertex3f(h, -h, -h);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawTube(double r1, double r2, double h, int n)
{
	double ugaoKorak = M_PI * 2 / n;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, cigle);
	glColor3f(1, 1, 1);

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n; i++)
	{
		double ugao = i * ugaoKorak;

		double x1 = r1 * cos(ugao);	
		double z1 = r1 * sin(ugao);

		double x2 = r2 * cos(ugao);
		double z2 = r2 * sin(ugao);

		double nx = cos(ugao);
		double nz = sin(ugao);

		double u = (double)i / n;

		glNormal3f(nx, 0, nz);

		glTexCoord2f(u, 1.0);
		glVertex3f(x1, h, z1);

		glTexCoord2f(u, 0.0);
		glVertex3f(x2, 0, z2);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawCone(double r, double h, int n)
{
	double ugaoKorak = M_PI * 2 / n;
	
	double normalY = r / sqrt(pow(r, 2) + pow(h, 2));
	double normalXZ = h / sqrt(pow(r, 2) + pow(h, 2));

	glBegin(GL_TRIANGLE_FAN);

	glNormal3f(0, 1, 0);
	glVertex3f(0, h, 0);

	for (int i = 0; i <= n; i++)
	{
		double ugao = i * ugaoKorak;

		double x = r * cos(ugao);
		double z = r * sin(ugao);

		double nx = cos(ugao) * normalXZ;
		double nz = sin(ugao) * normalXZ;

		glNormal3f(nx, normalY, nz);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, -1, 0);
	glVertex3f(0, 0, 0);

	for (int i = n; i >= 0; i--)
	{
		double angle = i * ugaoKorak;
		glVertex3f(r * cos(angle), 0, r * sin(angle));
	}
	glEnd();
}

void CGLRenderer::DrawPaddle(double lenght, double width)
{
	double sw = width / 8.0;

	double x0 = 0;
	double x1 = lenght / 8.0;
	double x2 = lenght / 4.0;
	double x3 = lenght;

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);

	// uzi deo
	glVertex3f(x0, 0, 0);
	glVertex3f(x1, 0, 0);
	glVertex3f(x1, sw, 0);
	glVertex3f(x0, sw, 0);

	// prelazni deo
	glVertex3f(x1, 0, 0);
	glVertex3f(x2, 0, 0);
	glVertex3f(x2, width, 0);
	glVertex3f(x1, sw, 0);

	// siroki deo
	glVertex3f(x2, 0, 0);
	glVertex3f(x3, 0, 0);
	glVertex3f(x3, width, 0);
	glVertex3f(x2, width, 0);

	glEnd();

	// sad treba ponavljanje koda za zadnju stranu

	// zadnja strana
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);

	glVertex3f(x0, sw, 0);
	glVertex3f(x1, sw, 0);
	glVertex3f(x1, 0, 0);
	glVertex3f(x0, 0, 0);

	glVertex3f(x1, sw, 0);
	glVertex3f(x2, width, 0);
	glVertex3f(x2, 0, 0);
	glVertex3f(x1, 0, 0);

	glVertex3f(x2, width, 0);
	glVertex3f(x3, width, 0);
	glVertex3f(x3, 0, 0);
	glVertex3f(x2, 0, 0);

	glEnd();
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}
