#include "Editor.h"
#include "MonkudoxEngine.h"
#include "Stuff.h"
#include "Model.h"
#include "AudioManager.h"
#include "SkyBox.h"
#include "Postprocessing.h"
#include "Time.h"
#include "PhysicsWorld.h"
#include "Collider.h"
#include "RigidBody.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

struct WindowInfo
{
	int width;
	int height;
};

enum QualityLevel {
	LOW = 1,
	MEDIUM = 2,
	HIGH = 3
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

unsigned int samples;
unsigned int shadowMapWidth;
unsigned int shadowMapHeight;

void QualityChange(QualityLevel quality)
{
	switch (quality)
	{
	case LOW:
		samples = 4;
		shadowMapWidth = 1024;
		shadowMapHeight = 1024;
		break;
	case MEDIUM:
		samples = 9;
		shadowMapWidth = 2048;
		shadowMapHeight = 2048;
		break;
	case HIGH:
		samples = 16;
		shadowMapWidth = 4096;
		shadowMapHeight = 4096;
		break;
	default:
		std::cout << "FATAL ERROR QUALITY NOT LINKED";
		break;
	}
}

MonkudoxEngine::GameController game;

int main()
{
	game.Awake();
	QualityChange(LOW);

	// Init global de Jolt (solo una vez)
	JPH::Trace = [](const char* inFMT, ...) {
		va_list list;
		va_start(list, inFMT);
		vprintf(inFMT, list);
		va_end(list);
		};
	JPH::RegisterDefaultAllocator();
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

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
	glfwSwapInterval(0);
	gladLoadGL();
	glViewport(0, 0, windowInfo.width, windowInfo.height);

	GLFWimage icon;
	icon.pixels = stbi_load("./Assets/img/icon.png", &icon.width, &icon.height, nullptr, 4);
	glfwSetWindowIcon(window, 1, &icon);
    stbi_image_free(icon.pixels);

	PhysicsWorld physicsWorld;

	// ====================== SAHDERS & MESH ====================== // 

	Shader* shaderProgram;
	Shader lit("./Shaders/default.vert", "./Shaders/lit.frag", "./Shaders/default.geom");
	Shader ecatepec("./Shaders/default.vert", "./Shaders/toon.frag", "./Shaders/default.geom");

	shaderProgram = &ecatepec;

	Shader normalsShader("./Shaders/default.vert", "./Shaders/normals.frag", "./Shaders/normals.geom");
	Shader outlineProgram("./Shaders/outline.vert", "./Shaders/outline.frag");
	Shader framebufferProgram("./Shaders/framebuffer.vert", "./Shaders/framebuffer.frag");
	Shader skyboxShader("./Shaders/skybox.vert", "./Shaders/skybox.frag");
	Shader shadowMapProgram("./Shaders/shadowmap.vert", "./Shaders/shadowmap.frag");
	Shader shadowCubeMapProgram("./Shaders/shadowcubemap.vert", "./Shaders/shadowcubemap.frag", "./Shaders/shadowcubemap.geom");


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram->Activate();
	glUniform4f(glGetUniformLocation(shaderProgram->ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	framebufferProgram.Activate();
	glUniform1i(glGetUniformLocation(framebufferProgram.ID, "screenTexture"), 0);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// ====================== PRE SETUP ====================== // 

	Postprocessing postprocess(windowInfo.width, windowInfo.height, rectangleVertices, samples);

	Camera camera(windowInfo.width, windowInfo.height, glm::vec3(0.0f, 0.0f, 2.0f));

	SkyBox skybox(skyboxVertices, skyboxIndices, facesCubemap);
	float skyboxRotation = 0.0f;

	Time time;

	// ====================== SCENE SETUP ====================== //

	game.allGameObjects.emplace_back("Cirno", "./Assets/models/1/Cirno.glb", shaderProgram, camera);
	game.allGameObjects.emplace_back("Gooboo", "./Assets/models/2/Gooboo.glb", shaderProgram, camera);
	game.allGameObjects.emplace_back("Boogoo", "./Assets/models/3/Boogoo.glb", shaderProgram, camera);
	game.allGameObjects.emplace_back("Plane", "./Assets/models/Essentials/Plane.glb", shaderProgram, camera);

	GameObject& cirno = game.allGameObjects[0];
	cirno.transform.position = glm::vec3(25.0f, -1.0f, 25.0f);
	cirno.transform.scale = glm::vec3(0.5f);
	cirno.transform.rotation = glm::vec3(0.0f, 45.0f, 0.0f);
	cirno.transform.UpdateMatrix();

	GameObject& gooboo = game.allGameObjects[1];
	gooboo.transform.position = glm::vec3(0.0f);
	gooboo.transform.scale = glm::vec3(0.05f);
	gooboo.transform.rotation = glm::vec3(0.0f, 45.0f, 0.0f);
	gooboo.transform.UpdateMatrix();

	GameObject& boogoo = game.allGameObjects[2];
	boogoo.transform.position = glm::vec3(-25.0f, -1.0f, -25.0f);
	boogoo.transform.scale = glm::vec3(0.5f);
	boogoo.transform.rotation = glm::vec3(0.0f, 45.0f, 0.0f);
	boogoo.transform.UpdateMatrix();

	GameObject& plane = game.allGameObjects[3];
	plane.transform.position = glm::vec3(-50.0f, -25.0f, 0.0f);
	plane.transform.scale = glm::vec3(5.0f);
	plane.transform.rotation = glm::vec3(0.0f, 45.0f, 0.0f);
	plane.transform.UpdateMatrix();

	// ====================== PHYSICS ====================== //

	Collider goobooCollider(new JPH::BoxShape(JPH::Vec3(0.5f, 1.0f, 0.5f))); // ajusta tamaño según modelo
	RigidBody goobooBody(&physicsWorld, goobooCollider, JPH::Vec3(gooboo.transform.position.x, gooboo.transform.position.y, gooboo.transform.position.z), false);

	Collider planeCollider(new JPH::BoxShape(JPH::Vec3(100.0f, 1.0f, 100.0f)));
	RigidBody planeBody(&physicsWorld, planeCollider, JPH::Vec3(plane.transform.position.x, plane.transform.position.y, plane.transform.position.z), true);

	
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

	// ====================== SHADOWS ====================== // 

	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float farPlane = 150.0f;
	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, farPlane);
	glm::mat4 perspectiveProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightProjection = perspectiveProjection * lightView;


	shadowMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowMapProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	unsigned int pointShadowMapFBO;
	glGenFramebuffers(1, &pointShadowMapFBO);

	// Texture for Cubemap Shadow Map FBO
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Matrices needed for the light's perspective on all faces of the cubemap
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
	glm::mat4 shadowTransforms[] =
	{
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};
	// Export all matrices to shader
	shadowCubeMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[0]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[0]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[1]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[1]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[2]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[2]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[3]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[3]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[4]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[4]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[5]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[5]));
	glUniform3f(glGetUniformLocation(shadowCubeMapProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(shadowCubeMapProgram.ID, "farPlane"), farPlane);

	// ====================== ÑAM ÑAM ====================== // 

	float speed = 50.0f;

	audio.PlaySound("./Assets/audio/Click.wav", 1.0f);

	camera.BackfaceCulling(true, GL_CW);

	camera.isFreeLook = false;

	camera.renderShadows = false;

	Editor editor;
	editor.Init(window);

	game.Start();

	// ====================== GAME LOOP ====================== // 

	while (!glfwWindowShouldClose(window))
	{
		game.Update();

		time.UpdateClock();
		time.FPS_window(window);

		physicsWorld.Step(time.deltaTime);

		editor.StartGUILayout();

		if (editor.runtimePause)
		{
			skyboxRotation += 0.0025f * time.deltaTime;

			boogoo.transform.rotation += glm::vec3(2.0f, 0.0f, 2.0f);
			boogoo.transform.UpdateMatrix();
			
			physicsWorld.SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));

			JPH::BodyInterface& bodyInterface = physicsWorld.GetSystem().GetBodyInterface();
			JPH::BodyID goobooID = goobooBody.GetBodyID();

			// 1. Obtener vectores de dirección DESDE TU TRANSFORM
			// Usamos la rotación actual del personaje para saber hacia dónde es "adelante"
			glm::vec3 forward = gooboo.transform.GetForwardVector();
			// Proyectamos a XZ para que no intente volar si el modelo está algo inclinado
			forward.y = 0.0f;
			forward = glm::normalize(forward);

			// Calculamos el vector derecha (perpendicular al forward y al eje Y)
			glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

			// 2. Calcular dirección de movimiento según teclas
			glm::vec3 moveDir(0.0f);
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += forward;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= forward;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= right;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += right;

			if (glm::length(moveDir) > 0.0f)
				moveDir = glm::normalize(moveDir);

			// 3. Aplicar a Jolt (Velocidad constante, no posición manual)
			JPH::Vec3 currentVel = bodyInterface.GetLinearVelocity(goobooID);
			float movementSpeed = 15.0f;

			JPH::Vec3 newVel(moveDir.x * movementSpeed, currentVel.GetY(), moveDir.z * movementSpeed);

			// Lógica de Salto
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && std::abs(currentVel.GetY()) < 0.05f)
			{
				newVel.SetY(8.0f); // Fuerza de salto
			}

			bodyInterface.SetLinearVelocity(goobooID, newVel);
			bodyInterface.ActivateBody(goobooID);

			// 4. EL PASO CRUCIAL: Sincronizar Jolt -> Transform
			// Aquí es donde evitamos que el modelo "se pierda" o vaya a otra velocidad
			JPH::Vec3 physicsPos = bodyInterface.GetPosition(goobooID);
			gooboo.transform.position = glm::vec3(physicsPos.GetX(), physicsPos.GetY(), physicsPos.GetZ());

			// Actualizar matriz para el renderizado
			gooboo.transform.UpdateMatrix();
		}

		glEnable(GL_DEPTH_TEST);

		if(camera.renderShadows)
		{ 
			glViewport(0, 0, shadowMapWidth, shadowMapHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			for (auto& obj : game.allGameObjects)
			{
				if (obj.isActive && obj.meshRenderer.castShadows)
				{
					obj.model.Draw(shadowCubeMapProgram, camera, obj.transform.matrix);
				}
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, windowInfo.width, windowInfo.height);

		glBindFramebuffer(GL_FRAMEBUFFER, postprocess.FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, postprocess.postProcessingFBO);
		glBlitFramebuffer(0, 0, windowInfo.width, windowInfo.height, 0, 0, windowInfo.width, windowInfo.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	
		camera.Inputs(window, &gooboo.transform);
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		framebuffer_size_callback(window, fbWidth, fbHeight); 

		camera.updateMatrix(45.0f, 0.1f, farPlane);

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

		shaderProgram->Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
		glUniform1f(glGetUniformLocation(shaderProgram->ID, "farPlane"), farPlane);
		glUniform1i(glGetUniformLocation(shaderProgram->ID, "renderShadows"), camera.renderShadows);

		if (camera.renderShadows)
		{
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
			glUniform1i(glGetUniformLocation(shaderProgram->ID, "shadowCubeMap"), 2);
		}

		for (auto& obj : game.allGameObjects)
		{
			if (obj.isActive) 
		    {
				obj.transform.UpdateMatrix();
				obj.model.Draw(*shaderProgram, camera, obj.transform.matrix);
			}
		}

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		outlineProgram.Activate();
		glUniform1f(glGetUniformLocation(outlineProgram.ID, "outlineWidth"), 0.025f);  
		glUniform3f(glGetUniformLocation(outlineProgram.ID, "outlineColor"), 0.0f, 0.0f, 0.0f); // negro

		for (auto& obj : game.allGameObjects)
		{
			if (obj.isActive)
			{
				glm::mat4 outlineMat = obj.transform.matrix * glm::scale(glm::mat4(1.0f), glm::vec3(1.01f));
				obj.model.Draw(outlineProgram, camera, outlineMat);
			}
		}

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_FRAMEBUFFER_SRGB);
		framebufferProgram.Activate();
		glBindVertexArray(postprocess.rectVAO);
		glDisable(GL_DEPTH_TEST); 
		glBindTexture(GL_TEXTURE_2D, postprocess.framebufferTexture);
		glBindTexture(GL_TEXTURE_2D, postprocess.postProcessingTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		editor.CreateGUILayout(window, camera);
	}

	// ====================== CLEANUP ====================== // 

	//editor.SaveGUILayout();
	editor.Delete();
	
	//ma_sound_uninit(&backgroundMusic);
	shaderProgram->Delete();
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
