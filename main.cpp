#include"Model.h"
#include"AudioManager.h"
#include <format>

struct WindowInfo
{
	int width;
	int height;
};

float rectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	// INIT //
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	WindowInfo windowInfo = { windowInfo.width = 1280, windowInfo.height = 720 };
	GLFWwindow* window = glfwCreateWindow(windowInfo.width, windowInfo.height, "Monkudox", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "FATAL ERROR: Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, windowInfo.width, windowInfo.height);
	///////////////////////////////////////////////////////

	// SHADER & MESH CREATION //
	Shader shaderProgram("default.vert", "default.frag");
	Shader outlineProgram("outline.vert", "outline.frag");
	Shader framebufferProgram("framebuffer.vert", "framebuffer.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	framebufferProgram.Activate();
	glUniform1i(glGetUniformLocation(framebufferProgram.ID, "screenTexture"), 0);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

	///////////////////////////////////////////////////////

	// PRE SETUP //
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	///////////////////////////////////////////////////////

	////// Prepare framebuffer rectangle VBO and VAO ///////////////
	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int framebufferTexture;
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowInfo.width, windowInfo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowInfo.width, windowInfo.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: 0x" << std::hex << fboStatus << std::endl;



	// Use this to disable VSync (not advized)
	//glfwSwapInterval(0);


	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		"./Assets/img/skybox/px.png",  // Front  (+X)
	"./Assets/img/skybox/nx.png",  // Back   (-X)
	"./Assets/img/skybox/py.png",  // Up     (+Y)
	"./Assets/img/skybox/ny.png",  // Down   (-Y)
	"./Assets/img/skybox/pz.png",  // Right  (+Z)
	"./Assets/img/skybox/nz.png"   // Left   (-Z)
	};

	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	/////////////////////////////////////////////////////////////////////

	Camera camera(windowInfo.width, windowInfo.height, glm::vec3(0.0f, 0.0f, 2.0f));

	Model model1("./Assets/models/1/Cirno.glb");
	Model model2("./Assets/models/2/Gooboo.glb");
	Model model3("./Assets/models/3/Boogoo.glb");



	double prevTime = 0.0;
	double crntTime = 0.0;
	double deltaTime = 0.0;
	unsigned int counter = 0;

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Ajusta estos valores según el modelo
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.0f, 0.0f)); // bajar para que no flote
	modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotar si hace falta
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f)); // ¡ESCALA AQUÍ! 0.5f = mitad de tamaño

	glm::mat4 modelMatrix2 = glm::mat4(1.0f);
	modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(10.0f, -1.0f, 10.0f));
	modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(0.25f));

	glm::mat4 modelMatrix3 = glm::mat4(1.0f);
	modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(-10.0f, -1.0f, -10.0f));
	modelMatrix3 = glm::rotate(modelMatrix3, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix3 = glm::scale(modelMatrix3, glm::vec3(0.25f));

	
	// AUDIO //
	AudioManager audio;
	if (!audio.Init()) {
		std::cout << "FALLO CRÍTICO: No se pudo iniciar miniaudio" << std::endl;
	}

	// Música de fondo (en bucle, volumen bajito)
	/*ma_sound backgroundMusic;  // ← Declarar como variable local, NO puntero dinámico
	if (ma_sound_init_from_file(&audio.engine, "./Assets/audio/Background.mp3", 0, NULL, NULL, &backgroundMusic) == MA_SUCCESS) {
		ma_sound_set_volume(&backgroundMusic, 0.75f);     // volumen cómodo
		ma_sound_set_looping(&backgroundMusic, false);   // bucle infinito
		ma_sound_start(&backgroundMusic);
	}
	else {
		std::cout << "ERROR: No se pudo cargar" << std::endl;
		std::cout << "   → Comprueba que el archivo existe y es  válido" << std::endl;
	}*/

	// Sonido de click (uno solo)
	audio.PlaySound("./Assets/audio/Click.wav", 1.0f);
	///////////////////////////////////////////////////////

	// GAME LOOP /
	while (!glfwWindowShouldClose(window))
	{
		crntTime = glfwGetTime();
		deltaTime = crntTime - prevTime;
		counter++;
		if (deltaTime >= 1.0 / 30.0)
		{
			float fpsValue = (1.0f / deltaTime) * counter;
			std::string newTitle = std::format("Monkudox {:.1f} FPS", fpsValue);
			glfwSetWindowTitle(window, newTitle.c_str());

			prevTime = crntTime;
			counter = 0;
		}

		modelMatrix = glm::rotate(modelMatrix, glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		modelMatrix3 = glm::rotate(modelMatrix3, glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 1.0f));

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	
		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		glDepthFunc(GL_LEQUAL);
		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)windowInfo.width / windowInfo.height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		model1.Draw(shaderProgram, camera, modelMatrix);
		model2.Draw(shaderProgram, camera, modelMatrix2);
		model3.Draw(shaderProgram, camera, modelMatrix3);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		outlineProgram.Activate();
		glUniform1f(glGetUniformLocation(outlineProgram.ID, "outlineWidth"), 0.025f);  // prueba valores: 0.02f a 0.1f
		glUniform3f(glGetUniformLocation(outlineProgram.ID, "outlineColor"), 0.0f, 0.0f, 0.0f); // negro
		glm::mat4 outlineMatrix = glm::scale(modelMatrix, glm::vec3(1.01f));
		glm::mat4 outlineMatrix2 = glm::scale(modelMatrix2, glm::vec3(1.01f));
		glm::mat4 outlineMatrix3 = glm::scale(modelMatrix3, glm::vec3(1.01f));
		model1.Draw(outlineProgram, camera, outlineMatrix);
		model2.Draw(outlineProgram, camera, outlineMatrix2);
		model3.Draw(outlineProgram, camera, outlineMatrix3);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Draw the framebuffer rectangle
		framebufferProgram.Activate();
		glBindVertexArray(rectVAO);
		glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	///////////////////////////////////////////////////////

	// DESTROY STUFF & MEMORY RELEASE //
	//ma_sound_uninit(&backgroundMusic);
	shaderProgram.Delete();
	outlineProgram.Delete();
	framebufferProgram.Delete();
	skyboxShader.Delete();
	glDeleteFramebuffers(1, &FBO);
	audio.Shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
	///////////////////////////////////////////////////////
}