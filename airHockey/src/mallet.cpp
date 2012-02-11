/*
 * mallet.cpp
 *
 *  Created on: 11 Þub 2012
 *      Author: Engin Manap
 */

#include "mallet.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

extern float height;

void Mallet::move(float coord_x, float coord_y) {
	speed_x = x - coord_x;
	speed_y = y - (height - coord_y);

	x = coord_x;
	y = height - coord_y;
}




