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

	teksturaPauk = LoadTexture("spider.png");

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

	// DrawAxes();

	// kada god nesto poprimi drugu boju samo ovo i resava se sve 
	glColor3f(1, 1, 1);

	DrawSpider();

	glTranslatef(0, 51, 0);

	DrawEnvCuve(100);

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
	
	glDeleteTextures(1, &teksturaPauk);
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

void CGLRenderer::DrawEnvCuve(double a)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);

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

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{
	double dAng1 = M_PI / (double)nSeg;
	double dAng2 = 2 * M_PI / (double)nSeg;
	double ang1 = -M_PI / 2.0;
	double ang2;
	
	for (int i = 0; i < nSeg; i++)
	{
		ang2 = 0;

		glBegin(GL_QUAD_STRIP);

		for (int j = 0; j < nSeg + 1; j++)
		{
			double x1 = r * cos(ang1) * cos(ang2);
			double y1 = r * sin(ang1);
			double z1 = r * cos(ang1) * sin(ang2);

			double x2 = r * cos(ang1 + dAng1) * cos(ang2);
			double y2 = r * sin(ang1 + dAng1);
			double z2 = r * cos(ang1 + dAng1) * sin(ang2);

			double tx1 = texR * x1 / r + texU;
			double ty1 = texR * z1 / r + texV;

			double tx2 = texR * x2 / r + texU;
			double ty2 = texR * z2 / r + texV;

			glTexCoord2f(tx1, ty1);
			glVertex3f(x1, y1, z1);
			glTexCoord2f(tx2, ty2);
			glVertex3f(x2, y2, z2);

			ang2 += dAng2;
		}
		glEnd();
		ang1 += dAng1;
	}

	// a vole da izmisljaju uvek nesto sa ovim teksturama
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR)
{
	double dAng1 = 2 * M_PI / (double)nSeg;
	double ang1 = 0;

	glBegin(GL_TRIANGLE_FAN);

	glColor3f(1, 1, 1);
	glTexCoord2f(texU, texV);
	glVertex3d(0, h, 0);

	for (int i = 0; i <= nSeg; i++)
	{
		double x = r * cos(ang1);
		double y = 0;
		double z = r * sin(ang1);

		double tx = texR * x / r + texU;
		double ty = texR * z / r + texV;

		glTexCoord2f(tx, ty);
		glVertex3d(x, y, z);

		ang1 += dAng1;
	}
	glEnd();
}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg)
{
	glPushMatrix();

	glTranslatef(0, r, 0);
	DrawSphere(r, 2 * nSeg, 0.25, 0.25, 0.24);
	DrawCone(r, h, nSeg, 0.75, 0.75, 0.24);

	glPopMatrix;
}

void CGLRenderer::DrawLeg()
{
	glPushMatrix();

	DrawLegSegment(1, 10, 5);
	glTranslatef(0, 10, 0);
	glRotatef(85, 1, 0, 0);
	DrawLegSegment(1, 15, 5);

	glPopMatrix();
}

void CGLRenderer::DrawSpiderBody()
{
	glPushMatrix();

	glPushMatrix();

	glScalef(1, 0.5, 1);
	DrawSphere(3, 10, 0.25, 0.25, 0.24);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(6, 0, 0); // ne znam zasto ovde 6 ali ajde
	glScalef(1, 0.8, 1);
	DrawSphere(5, 10, 0.25, 0.25, 0.24);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(-4, 0, 0);
	glScalef(1, 0.5, 1);
	DrawSphere(2, 10, 0.75, 0.25, 0.24);
	
	glPopMatrix();

	glPopMatrix();
}

void CGLRenderer::DrawSpider()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, teksturaPauk);

	glPushMatrix();

	glTranslatef(0, 5, 0);

	DrawSpiderBody();

	glPushMatrix();


	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(i * 30 - 45, 0, 1, 0);
		glRotatef(45, 1, 0, 0);
		DrawLeg();
		glPopMatrix();
	}
	glPopMatrix();

	glPushMatrix();

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(i * 30 - 45 + 180, 0, 1, 0);
		glRotatef(45, 1, 0, 0);
		DrawLeg();
		glPopMatrix();
	}
	glPopMatrix();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}

// ovde sam sad iskoristio njihov prostiji nacin za kameru, ne radi se sa glLookAt funkcijom nego se svi objekti pomeraju sto je po malo glupo ali okej, bar je prostije
