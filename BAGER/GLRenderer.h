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

	UINT teksturaBager;
	UINT teksturaOkolina[6];

	void DrawPolygon(POINTF* points, POINTF* textCoords, int n);
	void DrawExtrudedPolygon(POINTF* points, POINTF* textCoords, int n, float zh, float r, float g, float b);
	void DrawBase();
	void DrawBody();
	void DrawArm(double zh);
	void DrawFork();
	void DrawExcavator();

	float ugao1 = 0;
	float ugao2 = 90;
	float ugao3 = -90;
	float ugao4 = -90;

	void DrawEnvCube(double a);

	void UpdateCamera();

	double cameraDist = 0;
	double cameraBeta = 0;
	double cameraAlpha = 0;
};
