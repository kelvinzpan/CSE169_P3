////////////////////////////////////////
// Core.h
////////////////////////////////////////

#pragma once

// This file just has a bunch of common stuff used by all objects. It mainly just
// includes GL and some other standard headers.

// Set up GLM
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Set up GLEW
#include <GL/glew.h>

// Set up GLUT
#include <GL/glut.h>

// Set up GLFW
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

// Types
typedef unsigned int uint;

// STD library
#include <iostream>
#include <vector>
#include <string>
#include <ctype.h>

static void printVec3(glm::vec3 vec)
{
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

static void printVec4(glm::vec4 vec)
{
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")" << std::endl;
}

static void printMat4(glm::mat4x4 mat)
{
	std::cout <<
		"[ " << mat[0][0] << " " << mat[1][0] << " " << mat[2][0] << " " << mat[3][0] << std::endl <<
		"  " << mat[0][1] << " " << mat[1][1] << " " << mat[2][1] << " " << mat[3][1] << std::endl <<
		"  " << mat[0][2] << " " << mat[1][2] << " " << mat[2][2] << " " << mat[3][2] << std::endl <<
		"  " << mat[0][3] << " " << mat[1][3] << " " << mat[2][3] << " " << mat[3][3] << " ]" << std::endl;
}