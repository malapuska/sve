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

	teksturaLampa = LoadTexture("lamp.jpg");

	teksturaOkolina[0] = LoadTexture("front.jpg");
	teksturaOkolina[1] = LoadTexture("back.jpg");
	teksturaOkolina[2] = LoadTexture("left.jpg");
	teksturaOkolina[3] = LoadTexture("right.jpg");
	teksturaOkolina[4] = LoadTexture("top.jpg");
	teksturaOkolina[5] = LoadTexture("bot.jpg");

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

	glColor3f(1, 1, 1);

	glPushMatrix();

	glTranslatef(0, -10, 0);

	DrawAxes();

	glColor3f(1, 1, 1);

	DrawLamp();

	glTranslatef(0, 50, 0);

	DrawEnvCube(100);

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
	
	glDeleteTextures(1, &teksturaLampa);
	glDeleteTextures(6, teksturaOkolina);

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

	glColor3f(1, 0, 0);	// x - crvena osa
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 1, 0);	// y - zelena osa
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);

	glColor3f(0, 0, 1);	// z - plava osa
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);

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

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);

	// prednji deo
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0); glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();

	// levi deo
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0); glVertex3f(-a / 2, a / 2, -a / 2);
	glEnd();

	// desni deo
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0); glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	// zadnji deo
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(0, 1); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0); glVertex3f(-a / 2, a / 2, a / 2);
	glEnd();

	// gornji deo (top)
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 0); glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	// donji deo (bot)
	glBindTexture(GL_TEXTURE_2D, teksturaOkolina[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, -a / 2, -a / 2);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, teksturaLampa);
	glColor3f(1, 1, 1);

	float teksturaDole, teksturaGore;
	if (texMode == 0)
	{
		teksturaDole = 0.0f;
		teksturaGore = 0.5f;
	}
	else
	{
		teksturaDole = 0.5f;
		teksturaGore = 1.0f;
	}

	double korak = 2 * M_PI / nSeg;

	// omotac
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++)
	{
		double ugao = i * korak;
		double x = cos(ugao);
		double z = sin(ugao);
		float u = (float)i / (float)nSeg; // horizontalna koordinata teksture

		glTexCoord2f(u, teksturaGore);
		glVertex3d(r2 * x, h, r2 * z);

		glTexCoord2f(u, teksturaDole);
		glVertex3d(r1 * x, 0, r1 * z);
	}
	glEnd();

	// baze ukoliko nisu otvorene
	if (!bIsOpen)
	{
		// donja baza
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5f, 0.25f); // centar donjeg dela teksture
		glVertex3d(0, 0, 0);
		for (int i =0; i<= nSeg; i++)
		{
			double ugao = i * korak;
			double x = cos(ugao);
			double z = sin(ugao);
			float u = 0.5f + 0.5 * x;		// horizontalna koordinata teksture
			float v = 0.25f + 0.25 * z;	// vertikalna koordinata teksture
			glTexCoord2f(u, v);
			glVertex3d(r1 * x, 0, r1 * z);
		}
		glEnd();

		// gornja baza
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5f, 0.25f); 
		glVertex3d(0, h, 0);
		for (int i = 0; i <= nSeg; i++)
		{
			double ugao = i * korak;
			double x = cos(ugao);
			double z = sin(ugao);
			float u = 0.5f + 0.5f * x;		// horizontalna koordinata teksture
			float v = 0.25f + 0.25f * z;	// vertikalna koordinata teksture
			glTexCoord2f(u, v);
			glVertex3d(r2 * x, h, r2 * z);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawLampBase()
{
	DrawCylinder(8, 7, 2, 10, 1, false);
}

void CGLRenderer::DrawLampArm()
{
	glPushMatrix();

	DrawCylinder(3, 3, 2, 10, 1, false);
	glRotated(90, 1, 0, 0);
	glTranslated(0, 3, -1);
	DrawCylinder(1, 1, 15, 10, 1, false);

	glPopMatrix();
}

void CGLRenderer::DrawLampHead()
{
	glPushMatrix();

	DrawCylinder(2, 2, 2, 10, 1, false);

	glTranslatef(-1.5, 0, 0);
	glRotatef(90, 1, 0, 0);
	glTranslated(0, -2.5, -1);
	DrawCylinder(3, 3, 5, 10, 1, false);

	glTranslated(0, 5, 0);
	DrawCylinder(3, 6, 5, 10, 0, true);

	glTranslated(0, -6, 0);
	DrawCylinder(2, 3, 1, 10, 1, false);

	glPopMatrix();
}

void CGLRenderer::DrawLamp()
{
	glPushMatrix();

	DrawLampBase();
	
	glRotated(90, 1, 0, 0);
	glRotated(ugao1, 0, 1, 0);
	DrawLampArm();

	glTranslated(0, 0, 18);
	glRotated(ugao2, 0, 1, 0);
	DrawLampArm();

	glTranslated(0, 0, 18);
	glRotated(ugao3, 0, 1, 0);
	DrawLampHead();

	glPopMatrix();
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}
