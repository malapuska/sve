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

	double R(double a, double alpha, int n, double h);
	void NormCrossProd(double x1, double y1, double z1, double x2, double y2, double z2, double& x, double& y, double& z);
	void DrawGear(double a, double alpha, int n, double h, double d);
	void SetMetalicMaterial();
	UINT LoadTexture(char* fileName);

	void NacrtajZvezdu(double r1, double r2, int n, double dubina);

	UINT teksturaOkoline;

	void DrawWall(double a);

	float ugao = 0.0f;		

	void UpdateCamera();

	double cameraDist = 0;
	double cameraBeta = 0;
	double cameraAlpha = 0;
};
