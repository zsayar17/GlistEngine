/*
 * gPostProcessManager.cpp
 *
 *  Created on: 29 Kas 2021
 *      Author: kayra
 */

#include <gPostProcessManager.h>

gPostProcessManager* gPostProcessManager::instance;
const int gPostProcessManager::fbocount = 2;

gPostProcessManager::gPostProcessManager() {
    fbos = nullptr;
    fbotoread = 0;
    fbotowrite = 0;
    lastwrittenfbo = 0;
}

gPostProcessManager::~gPostProcessManager() {
	delete[] fbos;
}

gPostProcessManager* gPostProcessManager::getInstance() {
	if(instance != nullptr) {
		return instance;
	}
	instance = new gPostProcessManager();
	return instance;
}

void gPostProcessManager::setDimensions(int width, int height) {
	this->width = width;
	this->height = height;
	fbos = new gFbo[fbocount];
	for(int i = 0; i < fbocount; i++) fbos[i].allocate(width, height, false);
}

void gPostProcessManager::addEffect(gBasePostProcess *effect) {
	effects.push_back(effect);
}

void gPostProcessManager::enable() {
	fbos[0].bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gPostProcessManager::disable() {
	glDisable(GL_DEPTH_TEST);
	fbotoread = 0;
	fbotowrite = 1;

	for(int i = 0; i < effects.size(); i++) {
		fbos[fbotowrite].bind();
		glClear(GL_COLOR_BUFFER_BIT);
		effects[i]->use();
		glBindVertexArray(gFbo::getQuadVao());
		fbos[fbotoread].getTexture().bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		lastwrittenfbo = fbotowrite;
		int temp = fbotoread;
		fbotoread = fbotowrite;
		fbotowrite = temp;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	gRenderObject::getRenderer()->getFboShader()->use();
	glBindVertexArray(gFbo::getQuadVao());
	fbos[lastwrittenfbo].getTexture().bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
