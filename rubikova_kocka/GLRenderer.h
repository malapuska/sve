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

	UINT LoadTexture(char* fileName);

	UINT tekstura;

	void DrawCube(float a, float tx0, float ty0, float tx1, float ty1);
	void DrawRubikCube(double a, int count);
	void SetWhiteLight();
	void SetMaterial(float r, float g, float b);

	float ugao[3] = { 0, 0, 0 };

	float cameraDist = 20;
	float cameraBeta = 0;
	float cameraAlpha = 0;

	void UpdateCamera();
};
