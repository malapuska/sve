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

	void DrawAxes();
	UINT LoadTexture(char* fileName);

	UINT teksturaPauk;
	UINT teksturaOkolina[6];

	void DrawEnvCuve(double a);
	void DrawSphere(double r, int nSeg, double texU, double texV, double texR);
	void DrawCone(double r, double h, int nSeg, double texU, double texV, double texR);
	void DrawLegSegment(double r, double h, int nSeg);
	void DrawLeg();
	void DrawSpiderBody();
	void DrawSpider();

	void UpdateCamera();

	double cameraDist = 0;
	double cameraBeta = 0;
	double cameraAlpha = 0;
};
