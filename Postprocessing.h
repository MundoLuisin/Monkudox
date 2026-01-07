#pragma once 

class Postprocessing
{
public:
	unsigned int rectVAO, rectVBO;
	unsigned int FBO;
	unsigned int framebufferTexture;
	unsigned int RBO;

	Postprocessing(int widowWidth, int windowHeight, float rectangleVertices[]);

	void Delete();
};