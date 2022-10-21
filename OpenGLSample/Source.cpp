#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sphere.h"

#include "shader.h"

#include <iostream>

#include "cylinder.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path); //function that loads textures

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float speedMultiplier = 2.5f; //variable used to adjust the speed

//switch between 2d and 3d
bool threeDview = true;


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");
	Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs"); //applied to material
	Shader lightingCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs"); //what gives off the light


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		
	};

	float planeVertices[] = {
		-3.0f, -1.0f, -3.0f,  0.0f, 0.0f,
		 3.0f, -1.0f, -3.0f,  1.0f, 0.0f,
		 3.0f, -1.0f,  3.0f,  1.0f, 1.0f,
		 3.0f, -1.0f,  3.0f,  1.0f, 1.0f,
		-3.0f, -1.0f,  3.0f,  0.0f, 1.0f,
		-3.0f, -1.0f, -3.0f,  0.0f, 0.0f
	};

	unsigned int VBO, VAO;   //soda bottle base 
	unsigned int VBO2, VAO2; //soda bottle middle
	unsigned int VBO3, VAO3; //soda bottle top
	unsigned int VBO4, VAO4; //soda bottle rounded cap base
	unsigned int VBO5, VAO5; //soda bottle cap

	unsigned int VBO6, VAO6; //plane that bottle sits on
	unsigned int lightCubeVBO, lightCubeVAO;
	unsigned int lightVBO, lightVAO;

	unsigned int diceVBO, diceVAO;
	unsigned int blackMarkerBodyVBO, blackMarkerBodyVAO;
	unsigned int blackMarkerTopVBO, blackMarkerTopVAO;
	unsigned int greenMarkerBodyVBO, greenMarkerBodyVAO;
	unsigned int greenMarkerTopVBO, greenMarkerTopVAO;

	unsigned int cubeVBO, cubeVAO; //generate a 6 sided die
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);					
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);					
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);


	//green marker body
	glGenVertexArrays(1, &greenMarkerBodyVAO);
	glGenBuffers(1, &greenMarkerBodyVBO);
	glBindVertexArray(greenMarkerBodyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, greenMarkerBodyVBO);

	//green marker top
	glGenVertexArrays(1, &greenMarkerTopVAO);
	glGenBuffers(1, &greenMarkerTopVBO);
	glBindVertexArray(greenMarkerTopVAO);
	glBindBuffer(GL_ARRAY_BUFFER, greenMarkerTopVBO);

	//black marker body
	glGenVertexArrays(1, &blackMarkerBodyVAO);
	glGenBuffers(1, &blackMarkerBodyVBO);
	glBindVertexArray(blackMarkerBodyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, blackMarkerBodyVBO);

	//black marker top
	glGenVertexArrays(1, &blackMarkerTopVAO);
	glGenBuffers(1, &blackMarkerTopVBO);
	glBindVertexArray(blackMarkerTopVAO);
	glBindBuffer(GL_ARRAY_BUFFER, blackMarkerTopVBO);

	//d20 (a 20 sided die represented as a sphere)
	glGenVertexArrays(1, &diceVAO);
	glGenBuffers(1, &diceVBO);
	glBindVertexArray(diceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, diceVBO);

	glGenVertexArrays(1, &lightCubeVAO);
	glGenBuffers(1, &lightCubeVBO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);

	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glBindVertexArray(VAO4);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4);


	glGenVertexArrays(1, &VAO5);
	glGenBuffers(1, &VBO5);
	glBindVertexArray(VAO5);
	glBindBuffer(GL_ARRAY_BUFFER, VBO5);

	glGenVertexArrays(1, &VAO6);
	glGenBuffers(1, &VBO6);
	glBindVertexArray(VAO6);
	glBindBuffer(GL_ARRAY_BUFFER, VBO6);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);





	// load and create a texture 
	// -------------------------

	unsigned int texture1 = loadTexture("images/wood_table.jpg");
	unsigned int texture2 = loadTexture("images/bottle_label.png");
	unsigned int texture3 = loadTexture("images/test.png");
	unsigned int texture4 = loadTexture("images/bottle_top2.jpg");

	unsigned int diceTexture = loadTexture("images/dice20.jpg");
	unsigned int markerBlackTexture = loadTexture("images/marker_black.jpg");
	unsigned int markerBlackTopTexture = loadTexture("images/blackMarkerTop.jpg");
	unsigned int markerGreenTexture = loadTexture("images/marker_green.jpg");
	unsigned int markerGreenTopTexture = loadTexture("images/greenMarkerTop.jpg");
	unsigned int dice6Texture = loadTexture("images/d6.png");



	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setInt("texture3", 2);
	ourShader.setInt("texture4", 3);

	glm::mat4 model;


	unsigned int diffuseMap = texture1; //this is the wooden table
	unsigned int specularMap = loadTexture("images/wood_table_BW.jpg"); //black and white version of the table

	

	unsigned int bulpMap = texture1;


	//shader configuration
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//activating the shader
		lightingShader.use();
		lightingShader.setVec3("viewPos", cameraPos);
		lightingShader.setFloat("material.shininess", 32.0f);

		//directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light
		lightingShader.setVec3("pointLights.position", 0.7f, 0.2f, 2.0f);
		lightingShader.setVec3("pointLights.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights.diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights.constant", 1.0f);
		lightingShader.setFloat("pointLights.linear", 0.09);
		lightingShader.setFloat("pointLights.quadratic", 0.032);


		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);

		// activate shader
		ourShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		if (threeDview == true)
		{
			glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			ourShader.setMat4("projection", projection);
			lightingShader.setMat4("projection", projection);
			lightingCubeShader.setMat4("projection", projection);
		}
		
		else if (threeDview == false)
		{
			glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, 0.0f, 1.0f);
			ourShader.setMat4("projection", projection);
			lightingShader.setMat4("projection", projection);
			lightingCubeShader.setMat4("projection", projection);
		}

		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);
		lightingShader.setMat4("view", view);
		lightingCubeShader.setMat4("view", view);
		

		//render plane
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO6);
		model = glm::mat4(2.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-0.5f, .84f, 0.0f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 6); //draws triangles using the first 6 vertices 

		//bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		//bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);


		//d6
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dice6Texture);
		glBindVertexArray(cubeVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-2.25f, -0.02f, 0.1f));
		model = glm::scale(model, glm::vec3(0.25f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36); 


		//d20
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diceTexture);

		glBindVertexArray(diceVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.7f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		Sphere dice(.18, 8, 10);
		dice.Draw();

		//black marker body
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, markerBlackTexture);
		glBindVertexArray(blackMarkerBodyVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-1.25f, -0.08f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate pen around x-axis
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C8(.1, 10, 1.75, true, true, true);
		C8.render();

		//black marker top
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, markerBlackTopTexture);
		glBindVertexArray(blackMarkerTopVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-1.25f, -0.08f, -1.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate pen around x-axis
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C9(.12, 10, .75, true, true, true);
		C9.render();

		//green marker body
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, markerGreenTexture);
		glBindVertexArray(greenMarkerBodyVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-1.65f, -0.08f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate pen around x-axis
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C10(.1, 10, 1.75, true, true, true);
		C10.render();

		//green marker top
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, markerGreenTopTexture);
		glBindVertexArray(greenMarkerTopVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-1.65f, -0.08f, -1.25f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate pen around x-axis
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C11(.12, 10, .75, true, true, true);
		C11.render();

		//base of soda bottle
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C(.75, 10, .3, true, true, true);
		C.render();

		//middle of soda bottle
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2); //bottle lable

		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 0.42f, 0.0f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C2(.7, 20, .8, true, true, true);
		C2.render();

		//top of soda bottle
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);

		glBindVertexArray(VAO3);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);
		static_meshes_3D::Cylinder C3(.75, 7.5, .3, true, true, true);
		C3.render();

		//rounded cap base
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);

		glBindVertexArray(VAO3);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 0.80f, 0.0f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		Sphere S(.68, 10, 10);
		S.Draw();


		//cap of soda bottle
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture4);
		glBindVertexArray(VAO5);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C4(.25, 22, .9, true, true, true);
		C4.render();

		//draw the light object
		lightingCubeShader.use();
		if (threeDview == true)
		{
			glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			lightingCubeShader.setMat4("projection", projection);
		}
		else {
			glm::mat4 projection = glm::ortho(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			lightingCubeShader.setMat4("projection", projection);		
		}
		lightingCubeShader.setMat4("view", view);

		//draw the light
		glBindVertexArray(lightCubeVAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.5f, 5.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		lightingCubeShader.setMat4("model", model);


		glBindVertexArray(lightVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(8.0f, 6.80f, 0.0f));
		ourShader.setMat4("model", model);
		lightingShader.setMat4("model", model);
		Sphere S11(.5, 10, 10);
		S11.Draw();
	


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------


void processInput(GLFWwindow* window)
{
	float cameraSpeed = speedMultiplier * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	
	//change between 2D and 3D scenes
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		if (threeDview == true)
			threeDview = false;
		else if (threeDview == false)
			threeDview = true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);

}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//scrolling up will speed up the camera
	if (yoffset == 1.0)
		speedMultiplier += 0.5;
	//scrolling down will slow down the camera until the camera can no longer move
	if (yoffset == -1.0 && speedMultiplier > 0)
		speedMultiplier -= 0.5;
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}