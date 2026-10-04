#pragma once
#include <string>
#include <iostream>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined(__linux__)
#include <cstdlib>
#include <unistd.h>
#endif

class OpenGLContext
{
public:
	GLFWwindow* window{ nullptr };

	void onFramebufferResize(int width, int height)
	{
		m_windowWidth = width;
		m_windowHeight = height;
		glViewport(0, 0, width, height);
	}


	OpenGLContext()
	{}
	OpenGLContext(
		const int& width,
		const int& height,
		const std::string& title,
		const int& openGLMajorVersion,
		const int& openGLMinorVersion
	)
	{
		m_windowWidth = width;
		m_windowHeight = height;
		m_OpenGLVersionMajor = openGLMajorVersion;
		m_OpenGLVersionMinor = openGLMinorVersion;
		m_windowTitle = title;
		
		std::cout << "OPenGL Context" << std::endl;
	}
	bool initOpenGL()
	{
		glfwSetErrorCallback(glErrorCallback);
		// WSLg exposes the GPU as /dev/dxg. Wayland asks Mesa for an EGL
		// device, and Mesa answers with Zink against a Vulkan device that
		// is not there. X11 plus the D3D12 Gallium driver creates the context.
#if defined(__linux__)
		const bool wslGpu = access("/dev/dxg", F_OK) == 0
			&& access("/dev/dri", F_OK) != 0;
		if (wslGpu)
		{
			if (!getenv("GALLIUM_DRIVER"))
				setenv("GALLIUM_DRIVER", "d3d12", 0);
			glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
			std::cout << "WSL GPU: using X11 and the D3D12 OpenGL driver\n";
		}
#endif
		if (!glfwInit())
		{
			std::cout << "Failed to init GLFW\n";
			return false;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_OpenGLVersionMajor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_OpenGLVersionMinor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
		if (!window)
		{
			std::cerr << "Failed to create GLFW window\n";
			glfwTerminate();
			return false;
		}


		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
		glfwSwapInterval(1); // vsync



		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cerr << "Failed to init GLAD\n";
			return false;
		}
		bool glad{ loadGladPointers() };
		if (glad == false)
		{
			return glad;
		}

		std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
		std::cout << "GPU: " << glGetString(GL_RENDERER) << "\n";

#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugCallback, nullptr);
#endif
		
		return true;

	}





	std::string getGPUName()
	{
		std::string gpuName = reinterpret_cast <const char*>(glGetString(GL_RENDERER));
		return gpuName;
	}
	std::string getOpenGLVersion()
	{
		std::string version = reinterpret_cast <const char*>(glGetString(GL_VERSION));
		return version;
	}


	void setOpenGLContextDatas(
		const int& width,
		const int& height,
		const std::string& title,
		const int& openGLMajorVersion,
		const int& openGLMinorVersion
	)
	{
		m_windowWidth = width;
		m_windowHeight = height;
		m_OpenGLVersionMajor = openGLMajorVersion;
		m_OpenGLVersionMinor = openGLMinorVersion;
		m_windowTitle = title;
	}

	int getWindowsWidth()
	{
		return m_windowWidth;
	}

	int getWindowsheight()
	{
		return m_windowHeight;
	}
	
private:


	int m_windowWidth, m_windowHeight;
	std::string m_windowTitle;
	int m_OpenGLVersionMinor;
	int m_OpenGLVersionMajor;
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		OpenGLContext* self = static_cast<OpenGLContext*>(glfwGetWindowUserPointer(window));
		if (self)
		{
			self->onFramebufferResize(width, height);
		}
			
	}
	static void glErrorCallback(int code, const char* description)
	{
		std::cerr << "GLFW " << code << ": " << description << "\n";
	}

	static void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar* message, const void* userParam)
	{
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;
		std::cerr << "[GL DEBUG] " << message << "\n";
	}


	bool loadGladPointers()
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cerr << "Failed to init GLAD\n";
			return false;
		}
		return true;
	}

};