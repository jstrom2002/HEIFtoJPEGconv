/**	stdafx.h
* 
*	Common header for includes for all files. Also contains initial compilation instructions, so this file should run first.
*/

#pragma once


#define LIBHEIF_STATIC_BUILD
#define LIBDE265_STATIC_BUILD
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "libde265.lib")
#pragma comment(lib, "heif.lib")
#pragma comment(lib, "aom.lib")

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "Bcrypt.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libxml2.lib")
#pragma comment(lib, "charset.lib")
#pragma comment(lib, "iconv.lib")
#pragma comment(lib, "turbojpeg.lib")
#pragma comment(lib, "poly2tri.lib")
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(lib, "minizip.lib")
#pragma comment(lib, "kubazip.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "jpeg.lib")
#pragma comment(lib, "glfw3.lib")


#ifdef _DEBUG
#pragma comment(lib, "bz2d.lib")
#pragma comment(lib, "assimp-vc142-mtd.lib")
#pragma comment(lib, "glew32d.lib")
#pragma comment(lib, "libprotobufd.lib")
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "tiffd.lib")
#pragma comment(lib, "webpd.lib")
#pragma comment(lib, "lzmad.lib")
#pragma comment(lib, "libszip_D.lib")
#pragma comment(lib, "libhdf5_D.lib")
#pragma comment(lib, "lzmad.lib")
#pragma comment(lib, "lz4d.lib")
#pragma comment(lib, "libpng16d.lib")
#pragma comment(lib, "opencv_cored.lib")
#pragma comment(lib, "opencv_imgcodecsd.lib")
#pragma comment(lib, "opencv_imgprocd.lib")
#else
#pragma comment(lib, "bz2.lib")
#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "assimp-vc142-mt.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "tiff.lib")
#pragma comment(lib, "webp.lib")
#pragma comment(lib, "lzma.lib")
#pragma comment(lib, "lz4.lib")
#pragma comment(lib, "libpng16.lib")
#pragma comment(lib, "libszip.lib")
#pragma comment(lib, "libhdf5.lib")
#pragma comment(lib, "opencv_core.lib")
#pragma comment(lib, "opencv_imgcodecs.lib")
#pragma comment(lib, "opencv_imgproc.lib")
#endif



#define GLEW_STATIC

#include <GL/glew.h>
#ifdef _WIN32
#include <Windows.h>
#else
//PUT CROSS-PLATFORM STUFF HERE
#endif
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#include <string>
#include <vector>
#include <memory>
#define GLM_FORCE_SWIZZLE
#define GLM_PRECISION_HIGHP_FLOAT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#define USE_IMGUI_TABLES
#include "imgui.h"
#include "imconfig.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace HEIFtoJPEG
{
	class UI;
	extern std::shared_ptr<UI> ui;
	extern GLFWwindow* window;
	extern ImTextureID fileID;
	extern ImTextureID homeID;
	extern ImTextureID logoIcon;
	extern ImTextureID upArrowID;
	extern std::vector<std::string> to_load_filenames;
}
#include "Util.h"