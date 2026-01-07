#include "Editor.h"
// THIS INCLUDES ARE DANGEROUS AND SHOULD BE CHANGED !!!
#include "Monkudox.h"
#include "Stuff.h"

struct WindowInfo
{
	int width;
	int height;
};


std::string facesCubemap[6] =
{
	"./Assets/img/skybox/px.png",  // Front  (+X)
    "./Assets/img/skybox/nx.png",  // Back   (-X)
    "./Assets/img/skybox/py.png",  // Up     (+Y)
    "./Assets/img/skybox/ny.png",  // Down   (-Y)
    "./Assets/img/skybox/pz.png",  // Right  (+Z)
    "./Assets/img/skybox/nz.png"   // Left   (-Z)
};

WindowInfo windowInfo = { windowInfo.width = 1280, windowInfo.height = 720 };
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	windowInfo.width = width;
	windowInfo.height = height;
}

int main()
{
	// ====================== INIT ====================== // 
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

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

	// ====================== SAHDERS & MESH ====================== // 

	Shader shaderProgram("./Shaders/default.vert", "./Shaders/default.frag", "./Shaders/default.geom");
	Shader normalsShader("./Shaders/default.vert", "./Shaders/normals.frag", "./Shaders/normals.geom");
	Shader outlineProgram("./Shaders/outline.vert", "./Shaders/outline.frag");
	Shader framebufferProgram("./Shaders/framebuffer.vert", "./Shaders/framebuffer.frag");
	Shader skyboxShader("./Shaders/skybox.vert", "./Shaders/skybox.frag");


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


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// ====================== PRE SETUP ====================== // 

	Postprocessing postprocess(windowInfo.width, windowInfo.height, rectangleVertices);

	Camera camera(windowInfo.width, windowInfo.height, glm::vec3(0.0f, 0.0f, 2.0f));

	SkyBox skybox(skyboxVertices, skyboxIndices, facesCubemap);
	float skyboxRotation = 0.0f;

	Time time;

	Model model1("./Assets/models/1/Cirno.glb");
	Model model2("./Assets/models/2/Gooboo.glb");
	Model model3("./Assets/models/3/Boogoo.glb");
	Model model5("./Assets/models/5/Plane.glb");



	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

	glm::mat4 modelMatrix5 = glm::mat4(1.0f);
	modelMatrix5 = glm::translate(modelMatrix5, glm::vec3(-50.0f, -25.0f, 25.0f));
	modelMatrix5 = glm::rotate(modelMatrix5, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix5 = glm::scale(modelMatrix5, glm::vec3(5.0f));

	
	// ====================== AUDIO ====================== // 

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

	camera.BackfaceCulling(true, GL_CW);

	Editor editor;
	editor.Init(window);

	// ====================== GAME LOOP ====================== // 

	while (!glfwWindowShouldClose(window))
	{
		time.UpdateClock();
		time.FPS_window(window);

		editor.StartGUILayout();

		if (editor.runtimePause)
		{
			skyboxRotation += 0.0025f * time.deltaTime;

			modelMatrix2 = glm::rotate(modelMatrix2, glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 1.0f));
			modelMatrix3 = glm::rotate(modelMatrix3, glm::radians(2.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		}


		glBindFramebuffer(GL_FRAMEBUFFER, postprocess.FBO);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	
		camera.Inputs(window);
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		framebuffer_size_callback(window, fbWidth, fbHeight); 

		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		glDepthFunc(GL_LEQUAL);
		skyboxShader.Activate();

		glm::mat4 skyboxView = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		skyboxView = glm::rotate(skyboxView, skyboxRotation, glm::vec3(0.0f, 1.0f, 0.0f));  
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowInfo.width / windowInfo.height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		skybox.Draw(skyboxShader);

		glDepthFunc(GL_LESS);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		model1.Draw(shaderProgram, camera, modelMatrix);
		model1.Draw(normalsShader, camera, modelMatrix);
		model2.Draw(shaderProgram, camera, modelMatrix2);
		model3.Draw(shaderProgram, camera, modelMatrix3);
		model5.Draw(shaderProgram, camera, modelMatrix5);

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
		framebufferProgram.Activate();
		glBindVertexArray(postprocess.rectVAO);
		glDisable(GL_DEPTH_TEST); 
		glBindTexture(GL_TEXTURE_2D, postprocess.framebufferTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		editor.CreateGUILayout(window);
	}

	// ====================== CLEANUP ====================== // 

	//editor.SaveGUILayout();
	editor.Delete();
	
	//ma_sound_uninit(&backgroundMusic);

	shaderProgram.Delete(); 
	normalsShader.Delete();
	outlineProgram.Delete();
	framebufferProgram.Delete();
	skyboxShader.Delete();

	postprocess.Delete();

	audio.Shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
