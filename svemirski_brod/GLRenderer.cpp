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

	brodTekstura = LoadTexture("ShipT1.png");
	svemirTekstura[0] = LoadTexture("front.jpg");
	svemirTekstura[2] = LoadTexture("left.jpg");
	svemirTekstura[3] = LoadTexture("right.jpg");
	svemirTekstura[1] = LoadTexture("back.jpg");
	svemirTekstura[4] = LoadTexture("top.jpg");
	svemirTekstura[5] = LoadTexture("bot.jpg");
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	PromeniPozicijuKamere();

	// Postavljanje kamere
	gluLookAt(
		okoKamereX, okoKamereY, okoKamereZ,	// gde je oko
		0.0, 0.0, 0.0,						// gde gleda (centar figure, malo podignut)
		0.0, 1.0, 0.0						// up vektor
	);

	// ovde se dalje radi sve ostalo iscrtavanje scene ----------

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);

	glPushMatrix();
	DrawSpaceCube(200); 
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 10, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 10);
	glEnd();

	glEnable(GL_TEXTURE_2D);

	if (svetlo)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}

	float light_ambient[] = { 0,0,0,1 };
	float light_diffuse[] = { 1,1,1,1 };
	float light_specular[] = { 1,1,1,1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_DEPTH_TEST);

	double d = sqrt(5.8 * 5.8 + 2.15 * 2.15);
	glTranslatef(-d / 2, 0, 0);
	glRotatef(90, 1, 0, 0);
	DrawShip();


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
	
	glDeleteTextures(1, &brodTekstura);
	glDeleteTextures(6, svemirTekstura);

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

void CGLRenderer::DrawTriangle(float d1, float d2, float rep)
{
	double a1 = atan2(d2, d1);
	double d3 = sqrt(pow(d1, 2) + pow(d2, 2));

	double y = d1 * cos(a1) / d3;
	double x = d1 * sin(a1) / d3;

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);

	glNormal3f(0, 0, 1);

	glTexCoord2f(0.5 * rep, 0.0);
	glVertex3f(0, 0, 0);

	glTexCoord2f((0.5 + x) * rep, y * rep);
	glVertex3f(d1, 0, 0);

	glTexCoord2f(0.5 * rep, 1 * rep);
	glVertex3f(d1, d2, 0);

	glEnd();

	// ovo ako je nekom jasno da ga jebem onda
}

void CGLRenderer::DrawShip()
{
	glBindTexture(GL_TEXTURE_2D, brodTekstura);
	double a1 = atan2(2.15, 5.8);
	float rep = 3;

	glPushMatrix();
	glRotatef(-4.47, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-a1 * TO_DEG, 0, 0, 1);
	DrawTriangle(5.8, 2.15, rep);
	glPopMatrix();

	glPushMatrix();
	glScalef(1, -1, 1);
	glRotatef(-4.47, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-a1 * TO_DEG, 0, 0, 1);
	DrawTriangle(5.8, 2.15, rep);
	glPopMatrix();

	glPushMatrix();
	glRotatef(180, 1, 0, 0);

	glPushMatrix();
	glRotatef(-4.47, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-a1 * TO_DEG, 0, 0, 1);
	DrawTriangle(5.8, 2.15, rep);
	glPopMatrix();

	glPushMatrix();
	glScalef(1, -1, 1);
	glRotatef(-4.47, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-a1 * TO_DEG, 0, 0, 1);
	DrawTriangle(5.8, 2.15, rep);
	glPopMatrix();

	glPopMatrix();

	// ovo mi je kao i jasno
}

void CGLRenderer::DrawSpaceCube(double a)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);

	// prednji deo
	glBindTexture(GL_TEXTURE_2D, svemirTekstura[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0); glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();

	// levi deo
	glBindTexture(GL_TEXTURE_2D, svemirTekstura[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0); glVertex3f(-a / 2, a / 2, -a / 2);
	glEnd();

	// desni deo
	glBindTexture(GL_TEXTURE_2D, svemirTekstura[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 1); glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0); glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	// zadnji deo
	glBindTexture(GL_TEXTURE_2D, svemirTekstura[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(0, 1); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0); glVertex3f(-a / 2, a / 2, a / 2);
	glEnd();

	// gornji deo (top)
	glBindTexture(GL_TEXTURE_2D, svemirTekstura[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(1, 0); glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(0, 1); glVertex3f(a / 2, a / 2, a / 2);
	glEnd();

	// donji deo (bot)
	glBindTexture(GL_TEXTURE_2D, svemirTekstura[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 0); glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 0); glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1); glVertex3f(a / 2, -a / 2, -a / 2);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
