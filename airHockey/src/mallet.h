/*
 * mallet.h
 *
 *  Created on: 11 Þub 2012
 *      Author: engin
 */

#ifndef MALLET_H_
#define MALLET_H_

#include <EGL/egl.h>
#include <GLES/gl.h>

class Mallet{
public:
	float x;
	float y;
	float size;											//this is radious, will be rendered as pi*size^2 or in this case 2*size

	float speed_x;
	float speed_y;
	GLfloat color;

	Mallet(float x, float y,float size, GLfloat color);
	Mallet();
	void move(float coor_x, float coor_y);
	float centerx() {return x + size; }
	float centery() {return y + size; }
};

class Puck : public Mallet {
public:
	float speed_vector_from_collition_sqr;

	Puck(float size, float speed_x, float speed_y); { this->size = size; this->speed_x = speed_x; this->speed_y = speed_y; speed_vector_from_collition_sqr = 0;}

	void move();
	void searchCollition(Mallet mallet);
	void calculateSpeedAfterCollision(Mallet mallet);
};


#endif /* MALLET_H_ */
