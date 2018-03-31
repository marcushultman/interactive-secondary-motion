/*
*	Interactive Secondary Motion for Virtual Faces implementation.
*
* Copyright © 2015, Marcus Hultman
*/

#include <stdio.h>
#include <string>

#include <vector>
#include <map>
#include <set>
#include <queue>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <AntTweakBar.h>
#include "TwVars.h"

#include "FFMPEGRecorder.h"

#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "CoordGrid.h"
#include "RigidSphere.h"

#pragma region Fields

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f
#define FOV 0.75f

static Camera		s_camera;
static glm::mat4	s_proj;

static CoordGrid*	s_grid;
static Model*		s_model;

static double		s_prevXPos, s_prevYPos;

// gloBalls
RigidSphere*		g_ball;

// Drag vertex <vertex index, target position>
std::pair<unsigned int, glm::vec3>* g_drag = NULL;

#pragma endregion

// AntTweakBar
TwBar *g_bar;
TwVars g_twVar;

#pragma region Event handlers

static void rayCast(glm::vec3 ray)
{
	// Drag vertex
	for (unsigned int i = 0; i < s_model->getNumMeshes(); i++){
		const Mesh* mesh = s_model->getMesh(i);
		int index = mesh->raycastVertex(s_camera.getPosition(), ray);
		if (index >= 0){
			glm::vec3 position = mesh->getVertexPosition(index);
			g_drag = new std::pair<unsigned int, glm::vec3>(index, position);
		}
	}
	
	// TODO: Shoot ball on impact point
}


static void onError(int error, const char* description)
{
	printf("Error: %s", description);
}

static void onMouseDown(GLFWwindow* window, int button, int action, int mods)
{
	// AntTweakBar event
	if (TwEventMouseButtonGLFW(button, action))
		return;

	// Scroll wheel press
	if (button == GLFW_MOUSE_BUTTON_1){
		if (action == GLFW_PRESS){
			s_camera.setMode(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == 
				GLFW_PRESS ? CameraMode::ARC : CameraMode::PAN);
		}
		else if (action == GLFW_RELEASE){
			s_camera.setMode(CameraMode::NONE);
		}
	}

	// Generate selection ray
	if (button == GLFW_MOUSE_BUTTON_2){
		if (action == GLFW_PRESS){
			// Get mouse pos
			double mouse_x, mouse_y;
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			// Calc selection ray
			int screenWidth, screenHeight;
			glfwGetWindowSize(window, &screenWidth, &screenHeight);
			double x = (2.0f * mouse_x) / double(screenWidth) - 1.0f;
			double y = 1.0f - (2.0f * mouse_y) / double(screenHeight);
			glm::vec4 ray_clip = glm::vec4(x, y, -1.0, 1.0);
			glm::vec4 ray_eye = glm::inverse(s_proj) * ray_clip;
			ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
			glm::vec3 ray_wor = glm::normalize(glm::vec3(inverse(s_camera.getView()) * ray_eye));
			// Ray cast event
			rayCast(ray_wor);
		}
		else if (action == GLFW_RELEASE) {
			delete g_drag;
			g_drag = NULL;
		}
	}
}
static void onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	// AntTweakBar event
	if (TwEventMousePosGLFW(int(xpos), int(ypos)))
		return;

	glm::vec2 dpos(xpos - s_prevXPos, s_prevYPos - ypos);

	// Drag mesh
	if (g_drag){
		g_drag->second += 0.01f * glm::vec3(dpos, 0);
	}

	// Camera control
	if (s_camera.getMode() == CameraMode::ARC){
		s_camera.rotate(dpos);
	}
	else if (s_camera.getMode() == CameraMode::PAN){
		s_camera.pan(dpos);
	}	

	s_prevXPos = xpos;
	s_prevYPos = ypos;
}
static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	// AntTweakBar event
	if (TwEventMouseWheelGLFW(int(yoffset)))
		return;

	s_camera.zoom((float) yoffset);
}

static void onKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// AntTweakBar event
	if (TwEventKeyGLFW(key, action))
		return;

	if (key == GLFW_KEY_N && action == GLFW_PRESS){
		g_twVar.sim_step = true;
	}	

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GL_TRUE);
	}	
}

static void onCharCallback(GLFWwindow* window, unsigned int codepoint)
{
	// AntTweakBar event
	if (TwEventCharGLFW(codepoint, GLFW_PRESS))
		return;
}

static void onFramebufferSizeChanged(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
	s_proj = glm::perspective(FOV, float(width) / float(height),
		NEAR_PLANE, FAR_PLANE);

	// AntTweakBar
	TwWindowSize(width, height);
}

#pragma endregion

static void setupAntTweakBar()
{
	g_bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='320 640' "); // resize bar
	TwDefine(" GLOBAL help='Simulation control.' "); // Message added to the help bar.
	
	/*TwAddVarRW(g_bar, "TD", TW_TYPE_FLOAT, &var_TIME_DELTA,
		" label='Time delta' precision=5 min=0 max=1 step=0.01 ");*/
	
	TwAddVarRW(g_bar, "FPS", TW_TYPE_DOUBLE, &g_twVar.info_fps,
		" group=INFO label='Frame rate:' precision=1 ");

	TwAddSeparator(g_bar, "SEP0", 0);

	/*TwAddVarRW(g_bar, "ANIMATION_SPEED", TW_TYPE_FLOAT, &var_ANIMATION_SPEED,
	" group=ANIMATION label='Speed' precision=1 min=0 max=10 step=0.1 ");*/

	TwAddVarRW(g_bar, "GRAVITY", TW_TYPE_FLOAT, &g_twVar.sim_gravity,
		" group=SIMULATION label='Gravity' precision=10 min=-1 max=1 step=0.0001 ");
	
	TwAddVarRW(g_bar, "SIMULATE", TW_TYPE_BOOL8, &g_twVar.sim_enable,
		" group=SIMULATION label='Run simulation' ");

	TwAddButton(g_bar, "SIMULATION_STEP", [](void*) { g_twVar.sim_step = true; }, NULL,
		" group=SIMULATION  label='Step simulation' key=N ");

	TwAddButton(g_bar, "SIMULATION_RESET", [](void*) { g_ball->reset(false); s_model->resetAnimation(); }, NULL,
		" group=SIMULATION  label='Reset' ");

	TwAddSeparator(g_bar, "SEP1", 0);

	TwAddVarRW(g_bar, "BLEND_VERT_STIFF", TW_TYPE_BOOL8, &g_twVar.blend_useWeights,
		" group=BLEND label='Use skin weights' ");
	TwAddVarRW(g_bar, "BLEND_STIFF_MULT", TW_TYPE_FLOAT, &g_twVar.blend_multiplier,
		" group=BLEND label='Multiplier' precision=4 min=0 max=1 step=0.0001 ");

	TwAddVarRW(g_bar, "BLEND_CONST_STIFF", TW_TYPE_FLOAT, &g_twVar.constraint_stiffness,
		" group=BLEND label='Stiffness k' precision=4 min=0 max=1 step=0.0001 ");

	TwAddSeparator(g_bar, "SEP2", 0);

	TwAddVarRW(g_bar, "RENDER_FILL", TW_TYPE_BOOL8, &g_twVar.render_fill,
		" group=RENDER label='Fill' ");
	TwAddVarRW(g_bar, "RENDER_WIREFRAME", TW_TYPE_BOOL8, &g_twVar.render_wireframe,
		" group=RENDER label='Wireframe' ");
	TwAddVarRW(g_bar, "RENDER_DISTMAG", TW_TYPE_BOOL8, &g_twVar.render_distMag,
		" group=RENDER label='Distortion magnitude' ");

	TwAddVarRW(g_bar, "rec_record", TW_TYPE_BOOL8, ffmpegRec(),
		" group=RECORD label='Record' ");

	TwAddSeparator(g_bar, "SEP3", 0);

	TwAddButton(g_bar, "BALL_RESET", [](void*) { g_ball->reset(); }, NULL, " label='Throw ball' ");
	
}

static void initialize(GLFWwindow* window)
{
	// Hook up GLFW events
	glfwSetMouseButtonCallback(window, onMouseDown);
	glfwSetScrollCallback(window, onMouseScroll);
	glfwSetCursorPosCallback(window, onMouseMove);

	glfwSetKeyCallback(window, onKeyDown);
	glfwSetCharCallback(window, onCharCallback);

	glfwSetFramebufferSizeCallback(window, onFramebufferSizeChanged);

	// Set up framebuffer (and projection matrix)
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	onFramebufferSizeChanged(window, width, height);

	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);
	setupAntTweakBar();

	// Init ffmpeg screen recording
	ffmpegSetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	ffmpegSetOutput("output.mp4");
	ffmpegOpen();


	s_grid = new CoordGrid();
	g_ball = new RigidSphere();

	//N : 7366    M : 43776
	s_model = new Model("resource/models/mesh1/head.dae");
	//N : 43922   M : 262656
	//s_model = new Model("resource/models/mesh1/head2.dae");
	//N : 175402  M : 1050624
	//s_model = new Model("resource/models/mesh1/head3.dae");
	
	//N : 1902    M : 11094
	//s_model = new Model("resource/models/mesh2/head.dae");
	//N : 7514    M : 44412
	//s_model = new Model("resource/models/mesh2/head2.dae");
	//N : 29838   M : 177648
	//s_model = new Model("resource/models/mesh2/head3.dae");

	// Some rendering settings
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(1.0, 2);
}

static void update(double elapsed_time, GLFWwindow* window)
{
	if (g_twVar.sim_enable || g_twVar.sim_step){
		g_ball->update(elapsed_time);
		s_model->update(elapsed_time);
		g_twVar.sim_step = false;
	}
}

static void draw(GLFWwindow* window)
{
	// Clear the buffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw simulated head
	glm::vec3 cameraPosition = s_camera.getPosition();
	glm::mat4 view = s_camera.getView();

	s_grid->draw(view, s_proj);
	g_ball->draw(view, s_proj);
	s_model->draw(view, s_proj, cameraPosition); // , false); // LBS only

	// AntTweakBar
	TwDraw();
}

static void unload(GLFWwindow* window)
{
	// AntTweakBar
	TwTerminate();

	delete s_grid;
	delete s_model;

	delete g_ball;

	ffmpegClose();
}

#pragma region Main Loop

void mainLoop(GLFWwindow* window)
{
	double old_time = glfwGetTime();

	double second = 0;
	std::queue<double> fpsLog;

	while (!glfwWindowShouldClose(window))
	{
		// Handles events and returns immediately
		glfwPollEvents();

		// Calculate elapsed time
		double current_time = glfwGetTime(),
			elapsed_time = (current_time - old_time);
		old_time = current_time;

		// Update avg. fps, display it every 2 seconds
		fpsLog.push(1.0 / elapsed_time);
		if (second < current_time){
			double fpsSum = 0, logSize = fpsLog.size();
			while (!fpsLog.empty()){
				fpsSum += fpsLog.front();
				fpsLog.pop();
			}
			g_twVar.info_fps = fpsSum / logSize;
			second += 2.0;
		}

		update(elapsed_time, window);
		draw(window);
		glfwSwapBuffers(window);

		ffmpegUpdate();
	}
}

#pragma endregion


#pragma region Entry point

// Very complex stuff, impossible to explain.
int main(void)
{
	// Set the error callback
	glfwSetErrorCallback(onError);

	// Initialize GLFW
	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}	

	// Create a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 
		"Interactive secondary motion for virtual faces",
		nullptr, //glfwGetPrimaryMonitor(), // for full screen
		nullptr);
	// Make the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);
	
	// Init glew
	glewInit();

	// Initialize program
	initialize(window);

	// Main loop
	mainLoop(window);

	unload(window);

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

#pragma endregion