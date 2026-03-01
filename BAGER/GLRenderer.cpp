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

	teksturaBager = LoadTexture("excavator.png");

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

	DrawAxes();

	glTranslatef(-4, 0, -2.5);

	DrawExcavator();

	glTranslatef(4, 50, 2.5);

	DrawEnvCube(100);

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
	gluPerspective(55, (double)w / (double)h, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &teksturaBager);
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

	glColor3f(0, 0, 1);	// x - crvena osa
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(1, 0, 0);	// y - zelena osa
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);

	glColor3f(0, 1, 0);	// z - plava osa
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

void CGLRenderer::DrawPolygon(POINTF* points, POINTF* textCoords, int n)
{
	glBegin(GL_TRIANGLE_FAN);

	for (int i = 0; i < n; i++) 
	{
		glTexCoord2f(textCoords[i].x, textCoords[i].y);
		glVertex2f(points[i].x, points[i].y);
	}

	glEnd();
}

void CGLRenderer::DrawExtrudedPolygon(POINTF* points, POINTF* textCoords, int n, float zh, float r, float g, float b)
{
	glEnable(GL_TEXTURE_2D);

	// donja osnova
	glColor3f(1, 1, 1);
	DrawPolygon(points, textCoords, n);

	// gornja osnova
	glPushMatrix();
	glTranslatef(0, 0, zh);
	DrawPolygon(points, textCoords, n);
	glPopMatrix();

	// omotac
	glDisable(GL_TEXTURE_2D);
	glColor3f(r, g, b);

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= n; i++)
	{
		int indeks = i % n; // ovo sluzi da bi zatvorili sa poslednjom iteracijom gde ce indeks da bude 0
		glVertex3f(points[indeks].x, points[indeks].y, 0);
		glVertex3f(points[indeks].x, points[indeks].y, zh);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
}

void CGLRenderer::DrawBase()
{
	// prostorne koordinate se dobijaju iz grid koordinata (16x16 grid, 1 podeok = 0.5 jedinice)
	// prostorni_x = (grid_x - min_grid_x) * 0.5
	// prostorni_y = (max_grid_y - grid_y) * 0.5  -- y se invertuje jer u opengl-u y ide nagore, a u teksturi nadole
	// min_grid_x = 0, max_grid_y = 16
	
	// grid koordinate temena (u smeru kazaljke na satu):
	// (0,13), (1,12), (15,12), (16,13), (16,15), (15,16), (1,16), (0,15)

	POINTF points[8] = {
		{0.0f, 1.5f},						// (0,13)  -> x=(0-0)*0.5=0.0,  y=(16-13)*0.5=1.5
		{0.5f, 2.0f},						// (1,12)  -> x=(1-0)*0.5=0.5,  y=(16-12)*0.5=2.0
		{7.5f, 2.0f},						// (15,12) -> x=(15-0)*0.5=7.5, y=(16-12)*0.5=2.0
		{8.0f, 1.5f},						// (16,13) -> x=(16-0)*0.5=8.0, y=(16-13)*0.5=1.5
		{8.0f, 0.5f},						// (16,15) -> x=(16-0)*0.5=8.0, y=(16-15)*0.5=0.5
		{7.5f, 0.0f},						// (15,16) -> x=(15-0)*0.5=7.5, y=(16-16)*0.5=0.0
		{0.5f, 0.0f},						// (1,16)  -> x=(1-0)*0.5=0.5,  y=(16-16)*0.5=0.0
		{0.0f, 0.5f}						// (0,15)  -> x=(0-0)*0.5=0.0,  y=(16-15)*0.5=0.5

	};

	// teksturne koordinate se dobijaju direktno iz grid koordinata:
	// tex_x = grid_x / 16.0
	// tex_y = grid_y / 16.0

	POINTF texCoords[8] = {
		{0 / 16.0f,  13 / 16.0f},			// (0,13)
		{1 / 16.0f,  12 / 16.0f},			// (1,12)
		{15 / 16.0f, 12 / 16.0f},			// (15,12)
		{16 / 16.0f, 13 / 16.0f},			// (16,13)
		{16 / 16.0f, 15 / 16.0f},			// (16,15)
		{15 / 16.0f, 16 / 16.0f},			// (15,16)
		{1 / 16.0f,  16 / 16.0f},			// (1,16)
		{0 / 16.0f,  15 / 16.0f}			// (0,15)
	};

	glBindTexture(GL_TEXTURE_2D, teksturaBager);
	DrawExtrudedPolygon(points, texCoords, 8, 5.0f, 0, 0, 0);
}

void CGLRenderer::DrawBody()
{
	// prostorne koordinate se dobijaju iz grid koordinata (16x16 grid, 1 podeok = 0.5 jedinice)
	// prostorni_x = (grid_x - min_grid_x) * 0.5
	// prostorni_y = (max_grid_y - grid_y) * 0.5  -- y se invertuje jer u opengl-u y ide nagore, a u teksturi nadole
	// min_grid_x = 8, max_grid_y = 8
	
	// grid koordinate temena (u smeru kazaljke na satu):
	// (8,0), (12,0), (16,4), (16,8), (8,8)

	POINTF points[5] = {
		{0.0f, 4.0f},						// (8,0)  -> x=(8-8)*0.5=0.0,  y=(8-0)*0.5=4.0
		{2.0f, 4.0f},						// (12,0) -> x=(12-8)*0.5=2.0, y=(8-0)*0.5=4.0
		{4.0f, 2.0f},						// (16,4) -> x=(16-8)*0.5=4.0, y=(8-4)*0.5=2.0
		{4.0f, 0.0f},						// (16,8) -> x=(16-8)*0.5=4.0, y=(8-8)*0.5=0.0
		{0.0f, 0.0f}						// (8,8)  -> x=(8-8)*0.5=0.0,  y=(8-8)*0.5=0.0
	};

	// teksturne koordinate se dobijaju direktno iz grid koordinata:
	// tex_x = grid_x / 16.0
	// tex_y = grid_y / 16.0

	POINTF texCoords[5] = {
		{8 / 16.0f,  0 / 16.0f},			// (8,0)
		{12 / 16.0f, 0 / 16.0f},			// (12,0)
		{16 / 16.0f, 4 / 16.0f},			// (16,4)
		{16 / 16.0f, 8 / 16.0f},			// (16,8)
		{8 / 16.0f,  8 / 16.0f}				// (8,8)
	};

	glBindTexture(GL_TEXTURE_2D, teksturaBager);
	DrawExtrudedPolygon(points, texCoords, 5, 4.0f, 0.96f, 0.5f, 0.12f);
}

void CGLRenderer::DrawArm(double zh)
{
	// prostorne koordinate se dobijaju iz grid koordinata (16x16 grid, 1 podeok = 0.5 jedinice)
	// prostorni_x = (grid_x - min_grid_x) * 0.5
	// prostorni_y = (max_grid_y - grid_y) * 0.5  -- y se invertuje jer u opengl-u y ide nagore, a u teksturi nadole
	// min_grid_x = 0, max_grid_y = 12
	
	// grid koordinate temena (u smeru kazaljke na satu):
	// (1,8), (16,9), (16,11), (1,12), (0,11), (0,9)

	POINTF points[6] = {
		{0.5f, 2.0f},						// (1,8)  -> x=(1-0)*0.5=0.5,  y=(12-8)*0.5=2.0
		{8.0f, 1.5f},						// (16,9) -> x=(16-0)*0.5=8.0, y=(12-9)*0.5=1.5
		{8.0f, 0.5f},						// (16,11)-> x=(16-0)*0.5=8.0, y=(12-11)*0.5=0.5
		{0.5f, 0.0f},						// (1,12) -> x=(1-0)*0.5=0.5,  y=(12-12)*0.5=0.0
		{0.0f, 0.5f},						// (0,11) -> x=(0-0)*0.5=0.0,  y=(12-11)*0.5=0.5
		{0.0f, 1.5f}						// (0,9)  -> x=(0-0)*0.5=0.0,  y=(12-9)*0.5=1.5 y=(16-9)*0.5=3.5
	};

	// teksturne koordinate se dobijaju direktno iz grid koordinata:
	// tex_x = grid_x / 16.0
	// tex_y = grid_y / 16.0
	POINTF texCoords[6] = {
		{1 / 16.0f,  8 / 16.0f},			// (1,8)
		{16 / 16.0f, 9 / 16.0f},			// (16,9)
		{16 / 16.0f, 11 / 16.0f},			// (16,11)
		{1 / 16.0f,  12 / 16.0f},			// (1,12)
		{0 / 16.0f,  11 / 16.0f},			// (0,11)
		{0 / 16.0f,  9 / 16.0f}				// (0,9)
	};

	glBindTexture(GL_TEXTURE_2D, teksturaBager);
	DrawExtrudedPolygon(points, texCoords, 6, (float)zh, 0.96f, 0.5f, 0.12f);
}

void CGLRenderer::DrawFork()
{
	// prostorne koordinate se dobijaju iz grid koordinata (16x16 grid, 1 podeok = 0.5 jedinice)
	// prostorni_x = (grid_x - min_grid_x) * 0.5
	// prostorni_y = (max_grid_y - grid_y) * 0.5  -- y se invertuje jer u opengl-u y ide nagore, a u teksturi nadole
	// min_grid_x = 0, max_grid_y = 6
	
	// grid koordinate temena (u smeru kazaljke na satu):
	// (1,0), (7,0), (8,1), (8,6), (0,6), (0,1)

	POINTF points[6] = {
		{0.5f, 3.0f},						// (1,0) -> x=(1-0)*0.5=0.5,  y=(6-0)*0.5=3.0
		{3.5f, 3.0f},						// (7,0) -> x=(7-0)*0.5=3.5,  y=(6-0)*0.5=3.0
		{4.0f, 2.5f},						// (8,1) -> x=(8-0)*0.5=4.0,  y=(6-1)*0.5=2.5
		{4.0f, 0.0f},						// (8,6) -> x=(8-0)*0.5=4.0,  y=(6-6)*0.5=0.0
		{0.0f, 0.0f},						// (0,6) -> x=(0-0)*0.5=0.0,  y=(6-6)*0.5=0.0
		{0.0f, 2.5f}						// (0,1) -> x=(0-0)*0.5=0.0,  y=(6-1)*0.5=2.5
	};

	// teksturne koordinate se dobijaju direktno iz grid koordinata:
	// tex_x = grid_x / 16.0
	// tex_y = grid_y / 16.0

	POINTF texCoords[6] = {
		{1 / 16.0f, 0 / 16.0f},				// (1,0)
		{7 / 16.0f, 0 / 16.0f},				// (7,0)
		{8 / 16.0f, 1 / 16.0f},				// (8,1)
		{8 / 16.0f, 6 / 16.0f},				// (8,6)
		{0 / 16.0f, 6 / 16.0f},				// (0,6)
		{0 / 16.0f, 1 / 16.0f}				// (0,1)
	};

	glBindTexture(GL_TEXTURE_2D, teksturaBager);
	DrawExtrudedPolygon(points, texCoords, 6, 1.0f, 0.7f, 0.7f, 0.7f);
}

void CGLRenderer::DrawExcavator()
{
	glPushMatrix();

	// gusenice
	DrawBase();

	// kabina
	glTranslatef(2, 2, 0.5);

	glTranslatef(2, 0, 2);
	glRotatef(ugao1, 0, 1, 0);
	glTranslatef(-2, 0, -2);
	DrawBody();

	// ruka1
	glTranslatef(4, 0, 1.5);

	glTranslatef(1, 1, 0);
	glRotatef(ugao2, 0, 0, 1);
	glTranslatef(-1, -1, 0);
	DrawArm(1);

	// ruka2
	glTranslatef(6.5, 0, -0.25);

	glTranslatef(1, 1, 0);
	glRotatef(ugao3, 0, 0, 1);
	glTranslatef(-1, -1, 0);
	DrawArm(1.5);

	// viljuska
	glTranslatef(6.5, 0, 0.25);

	glTranslatef(1, 1, 0);
	glRotatef(ugao4, 0, 0, 1);
	glTranslatef(-1, -1, 0);
	DrawFork();

	glPopMatrix();
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

void CGLRenderer::UpdateCamera()
{
	glTranslatef(0, 0, -cameraDist);
	glRotatef(cameraBeta * TO_DEG, 1, 0, 0);
	glRotatef(cameraAlpha * TO_DEG, 0, 1, 0);
}
