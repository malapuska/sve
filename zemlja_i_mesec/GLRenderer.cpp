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
	
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);	// bela boja pozadine
	glEnable(GL_DEPTH_TEST);				// ukljucivanje provere dubine (skrivanje povrsina koje nisu okrenute ka posmatracu)
	glCullFace(GL_BACK);					// cull back faces
	glEnable(GL_CULL_FACE);					// ukljucivanje cull face-a (skrivanje povrsina koje nisu okrenute ka posmatracu)

	zemlja[0] = LoadTexture("TSC0.jpg");
	zemlja[1] = LoadTexture("TSC1.jpg");
	zemlja[2] = LoadTexture("TSC2.jpg");
	zemlja[3] = LoadTexture("TSC3.jpg");
	zemlja[4] = LoadTexture("TSC4.jpg");
	zemlja[5] = LoadTexture("TSC5.jpg");

	svemir[0] = LoadTexture("S0.jpg");
	svemir[1] = LoadTexture("S1.jpg");
	svemir[2] = LoadTexture("S2.jpg");
	svemir[3] = LoadTexture("S3.jpg");
	svemir[4] = LoadTexture("S4.jpg");
	svemir[5] = LoadTexture("S5.jpg");

	mesec[0] = LoadTexture("M0.jpg");
	mesec[1] = LoadTexture("M1.jpg");
	mesec[2] = LoadTexture("M2.jpg");
	mesec[3] = LoadTexture("M3.jpg");
	mesec[4] = LoadTexture("M4.jpg");
	mesec[5] = LoadTexture("M5.jpg");

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// ovo treba u templatu da bi se kamera pravilno postavila u odnosu na promene koje su mogle nastati u prethodnom frejmu (rotacija, zumiranje)
	PromeniPozicijuKamere();

	// Postavljanje kamere
	gluLookAt(
		okoKamereX, okoKamereY, okoKamereZ,	// gde je oko
		0.0, 4.0, 0.0,						// gde gleda (centar figure, malo podignut)
		0.0, 1.0, 0.0						// up vektor
	);

	// ovde se dalje radi sve ostalo iscrtavanje scene ----------

	// ovo je dodato u odnosu na resenje
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);		// iskljucivanje provere dubine za iscrtavanje pozadine (ne treba da se proverava da li je pozadina zaklonjena nekim objektom)
	glDisable(GL_LIGHTING);			// iskljucivanje osvetljenja za iscrtavanje pozadine (ne treba da se osvetljava)
	
	// i ovo je trebalo ovako
	glDisable(GL_CULL_FACE);		
	DrawSpace(200.0, 20);
	glEnable(GL_CULL_FACE);			// ponovo ukljucivanje cull face-a za iscrtavanje Zemlje i Meseca (treba da se proverava da li su zaklonjeni nekim objektom)	

	if (svetlo)
	{
		glEnable(GL_LIGHTING);			// ukljucivanje osvetljenja za iscrtavanje Zemlje i Meseca
		glEnable(GL_LIGHT0);			// ukljucivanje prvog izvora svetlosti (direktno osvetljenje)
	}

	float light_ambient[] = { 0,0,0,1 };
	float light_diffuse[] = { 1,1,1,1 };
	float light_specular[] = { 1,1,1,1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };	// pozicija svetlosti 

	glEnable(GL_DEPTH_TEST);				// ponovo ukljucivanje provere dubine za iscrtavanje Zemlje i Meseca (treba da se proverava da li su zaklonjeni nekim objektom)

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	DrawEarth(3, 20);

	glTranslated(-50, 0, 0);				// pomeranje na stranu da se ne bi preklapali
	glRotated(ugaoMeseca, 0, 1, 0);			// rotacija da se vidi druga strana Meseca
	DrawMoon(1, 20);

	// ----------------------------------------------------------

	glFlush();
	SwapBuffers(pDC->m_hDC);	// prikazivanje nacrtane scene

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);				// podesavanje viewporta na celu povrsinu prozora
	glMatrixMode(GL_PROJECTION);							// prelazak u projekcijsku matricu
	glLoadIdentity();										// resetovanje projekcijske matrice
	gluPerspective(40, (double)w / (double)h, 0.1f, 2000);	// perspektivna projekcija
	glMatrixMode(GL_MODELVIEW);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ---------------------------

	glDeleteTextures(6, zemlja);
	glDeleteTextures(6, svemir);
	glDeleteTextures(6, mesec);

	// ---------------------------
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

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);									// poravnanje texel-a u bajtovima (4 znaci da je svaki texel poravnat na 4 bajta, sto je slucaj sa RGB i RGBA formatima)
	glGenTextures(1, &idTeksture);											// generisanje ID-a za teksturu

	glBindTexture(GL_TEXTURE_2D, idTeksture);								// vezivanje teksture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);			// podesavanje wrap moda (ponavljanje teksture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);			// podesavanje wrap moda (ponavljanje teksture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// podesavanje filtera za uvecanje teksture (linearno filtriranje)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		// podesavanje filtera za umanjenje teksture (linearno filtriranje sa mipmapama)
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 			// podesavanje nacina kombinovanja boje teksture i boje materijala (modulacija)

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tekstura.Width(), tekstura.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, tekstura.GetDIBBits());	// kreiranje mipmapa i uploadovanje texel podataka u OpenGL
	return idTeksture;
}

void CGLRenderer::DrawPatch(double R, int n)
{
	double delta = 2 / (double)n;	// korak u x i y smeru
	double y = 1.0;					// pocetna y koordinata (gore)

	for (int i = 0; i < n; i++)
	{
		glBegin(GL_TRIANGLE_STRIP);		// pocetak crtanja tristripa
		double x = -1.0;				// pocetna x koordinata (levo)

		for (int j = 0; j < n + 1; j++)
		{
			double phi, theta;
			inverseTSC(x, y, phi, theta);			// dobijanje uglova phi i theta na osnovu x i y koordinata

			double xd, yd, zd;
			xd = R * cos(theta) * sin(phi);			// izracunavanje x koordinate tacke na povrsini lopte
			yd = R * sin(theta);					// izracunavanje y koordinate tacke na povrsini lopte
			zd = R * cos(theta) * cos(phi);			// izracunavanje z koordinate tacke na povrsini lopte

			glNormal3f(xd / R, yd / R, zd / R);		// normalni vektor (jedinicni vektor od centra lopte do tacke na povrsini)
			glTexCoord2f((x + 1.0) / 2.0, (-y + 1.0) / 2.0); 	// teksturne koordinate se dobijaju skaliranjem x i y koordinata iz intervala [-1, 1] u interval [0, 1]
			glVertex3f(xd, yd, zd);					// specificiranje vrha

			inverseTSC(x, y - delta, phi, theta);	// dobijanje uglova phi i theta na osnovu x i y koordinata

			xd = R * cos(theta) * sin(phi);			// izracunavanje x koordinate tacke na povrsini lopte
			yd = R * sin(theta);					// izracunavanje y koordinate tacke na povrsini lopte
			zd = R * cos(theta) * cos(phi);			// izracunavanje z koordinate tacke na povrsini lopte

			glNormal3f(xd / R, yd / R, zd / R);		// normalni vektor (jedinicni vektor od centra lopte do tacke na povrsini)
			glTexCoord2f((x + 1.0) / 2.0, (-y + delta + 1.0) / 2.0); 	// teksturne koordinate se dobijaju skaliranjem x i y koordinata iz intervala [-1, 1] u interval [0, 1]
			glVertex3f(xd, yd, zd);					// specificiranje vrha
			x += delta;							// povecavanje x koordinate za korak
		}

		glEnd();					// kraj crtanja tristripa
		y -= delta;					// smanjivanje y koordinate za korak
	}
}

void CGLRenderer::inverseTSC(double x, double y, double& phi, double& theta)
{
	phi = atan(x);
	theta = atan(y * cos(phi));
}

void CGLRenderer::DrawEarth(double R, int tes)
{
	glPushMatrix();
	for (int i = 0; i < 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, zemlja[i]);	// vezivanje odgovarajuce teksture za crtanje dela lopte
		DrawPatch(R, tes);							// crtanje dela lopte
		glRotated(90, 0, 1, 0);						// rotacija oko Y ose za 90 stepeni da bi se nacrtao sledeci deo lopte
	}
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1, 0, 0);						// rotacija oko X ose za 90 stepeni da bi se nacrtao deo lopte koji je na suprotnoj strani od pocetne pozicije
	glBindTexture(GL_TEXTURE_2D, zemlja[4]);		// vezivanje odgovarajuce teksture za crtanje dela lopte
	DrawPatch(R, tes);								// crtanje dela lopte
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1, 0, 0);							// rotacija oko X ose za 90 stepeni da bi se nacrtao deo lopte koji je na suprotnoj strani od pocetne pozicije
	glBindTexture(GL_TEXTURE_2D, zemlja[5]);		// vezivanje odgovarajuce teksture za crtanje dela lopte
	DrawPatch(R, tes);								// crtanje dela lopte
	glPopMatrix();
}

void CGLRenderer::DrawMoon(double R, int tes)
{
	glPushMatrix();
	for (int i = 0; i < 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, mesec[i]);	
		DrawPatch(R, tes);							
		glRotated(90, 0, 1, 0);						
	}
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1, 0, 0);						
	glBindTexture(GL_TEXTURE_2D, mesec[4]);		
	DrawPatch(R, tes);								
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1, 0, 0);							
	glBindTexture(GL_TEXTURE_2D, mesec[5]);		
	DrawPatch(R, tes);								
	glPopMatrix();
}

void CGLRenderer::DrawSpace(double R, int tes)
{
	glPushMatrix();
	for (int i = 0; i < 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, svemir[i]);
		DrawPatch(R, tes);
		glRotated(90, 0, 1, 0);
	}
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, svemir[4]);
	DrawPatch(R, tes);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, svemir[5]);
	DrawPatch(R, tes);
	glPopMatrix();
}

// ovo je sve moglo u jednu funkciju da se spakuje realno ali prvo dali su takav tekst da se pisu posebne funkcije
// a drugo ovo je njihovo resenje tako da sam iznenadjen ne bas prijatno
