#include <iostream>
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


glm::vec4 apply(glm::mat4 transformMatrix, glm::vec3 vec)
{
	return glm::vec4(vec.x, vec.y, vec.z, 1) * transformMatrix;
}

glm::mat4 getTransformMatrix(glm::vec3 eye, glm::vec3 view)
{
	glm::mat4 T1(1, 0, 0, 0,
							 0, 1, 0, 0,
							 0, 0, 1, 0,
							 -eye.x, -eye.y, -eye.z, 1);
	T1 = glm::transpose(T1);
	view = apply(T1, view);

	double sAlpha = sqrt(view.x*view.x + view.y*view.y);
	double sinAlpha = view.y / sAlpha;
	double cosAlpha = view.x / sAlpha;

	glm::mat4 T2(cosAlpha, -sinAlpha, 0, 0,
							 sinAlpha, cosAlpha, 0, 0,
										 0, 0, 1, 0,
										 0, 0, 0, 1);

	T2 = glm::transpose(T2);
	
	if (!view.x && !view.y) {
		T2 = glm::mat4();
	}

	view = apply(T2, view);

	double sBeta = sqrt(view.x*view.x + view.z*view.z);
	double sinBeta = view.x / sBeta;
	double cosBeta = view.z / sBeta;
	glm::mat4 T3(cosBeta, 0, sinBeta, 0, 
										 0, 1, 0, 0,
										 -sinBeta, 0, cosBeta, 0,
									   0, 0, 0, 1);

	if (!view.x && !view.z) {
		T3 = glm::mat4();
	}

	T3 = glm::transpose(T3);
	
	glm::mat4 T4(0, -1, 0, 0,
							 1, 0, 0, 0,
							 0, 0, 1, 0,
							 0, 0, 0, 1);
	T4 = glm::transpose(T4);

	glm::mat4 T5(-1, 0, 0, 0,
							 0, 1, 0, 0,
							 0, 0, 1, 0,
							 0, 0, 0, 1);
	T5 = glm::transpose(T5);

	return T1 * T2 * T3 * T4 * T5;
}

glm::mat4 getPerspectiveMatrix(double H)
{
	glm::mat4 perspective(1, 0, 0, 0,
											  0, 1, 0, 0,
												0, 0, 0, 1/H,
												0, 0, 0, 0);
	return glm::transpose(perspective);
}


