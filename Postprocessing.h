#pragma once 

class Postprocessing
{
public:
	unsigned int rectVAO, rectVBO;
	unsigned int FBO;
	unsigned int postProcessingFBO;
	unsigned int framebufferTexture;
	unsigned int postProcessingTexture;
	unsigned int RBO;

	Postprocessing(int widowWidth, int windowHeight, float rectangleVertices[], unsigned int samples);

	void FrameBufferTexture(int& widowWidth, int& windowHeight, float rectangleVertices[], unsigned int& samples);
	void PostProcessingTexture(int& widowWidth, int& windowHeight, float rectangleVertices[]);

	void Delete();

private:

};