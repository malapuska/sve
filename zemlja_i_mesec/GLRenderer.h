#pragma once
#include "DImage.h"

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

public:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	// moje promenljive i funkcije ----------------------------------

	float udaljenostKamere = 15.0f;		// udaljenost kamere od koordinatnog pocetka
	float ugaoKamereXY = 0.0f;			// ugao rotacije kamere oko Y ose
	float ugaoKamereXZ = 0.0f;			// ugao rotacije kamere oko X ose

	double okoKamereX = 0.0;			// koordinate tacke sa koje kamera gleda
	double okoKamereY = 0.0;
	double okoKamereZ = 0.0;

	void RotirajPogled(float deltaXY, float deltaXZ);
	void ZumirajPogled(float deltaUdaljenosti);
	void PromeniPozicijuKamere();

	UINT LoadTexture(char* fileName);

	UINT zemlja[6];
	UINT svemir[6];
	UINT mesec[6];

	void DrawPatch(double R, int n);
	void inverseTSC(double x, double y, double& phi, double& theta);
	void DrawEarth(double R, int tes);
	void DrawMoon(double R, int tes);
	void DrawSpace(double R, int tes);

	bool svetlo = true;
	float ugaoMeseca = 0.0f;

};
