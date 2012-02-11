/*
 * mallet.cpp
 *
 *  Created on: 11 �ub 2012
 *      Author: Engin Manap
 */

#include "mallet.h"

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <math.h>
#include <stdio.h>
extern float height;

Mallet::Mallet() {
	this->x = 512;
	this->y = 256;
}

Mallet::Mallet(float x, float y,float size, GLfloat color) {
	this->x = x;
	this->y = y;
	this->color = color;
	this->size = size;
	speed_x = 0;
	speed_y = 0;
}

void Mallet::move(float coord_x, float coord_y) {
	speed_x = x - coord_x;
	speed_y = y - (height - coord_y);

	x = coord_x;
	y = height - coord_y;
}

void Puck::move() {
	if ( speed_x > 30.0f)
			speed_x = 30.0f;
	if ( speed_y > 30.0f)
			speed_y = 30.0f;
	if ( speed_x < -30.0f)
			speed_x = -30.0f;
	if ( speed_y <  -30.0f)
			speed_y = -30.0f;
	 x += speed_x;
	 y += speed_y;

}


void Puck::searchCollition(Mallet mallet) {
	//calculate the line that puck moves
	//the formula for line is mx+n=y

	float m = speed_x / speed_y;
	float n = centery() - m * centerx();

	//now we have a line starts from center of puck
	//calculate a line with right angle to first one from mallet

	float n2 = mallet.centery() + m * mallet.centerx();

	//calculate pivot point:

	float pivot_x = (n2 - n) / (2*m);
	float pivot_y = m * pivot_x + n;

	//now look is the mallet close enough to collide
	float pivot_lenght_sqr = (pow(abs(mallet.centerx()-pivot_x),2) + pow(abs(mallet.centery()-pivot_y),2));
	float collide_distance_sqr = pow((this->size + mallet.size),2);
	if (pivot_lenght_sqr < collide_distance_sqr) {
		//collision is possible, look if it will be before next loop:

		//in which point the collition will happen?
		//calculating the triangle, mallet base, pivot point and collition point.
		float distance_after_collide_sqr = collide_distance_sqr - pivot_lenght_sqr;

		//calculate distance between puck and pivot
		float distance_to_pivot_sqr = pow((abs(pivot_x - centerx())),2) + pow((abs(pivot_y - centery())),2);
		//calculate distance that will be taken in this loop
		float distance_to_go_sqr = pow(speed_x,2) + pow(speed_y,2);
		if(distance_to_go_sqr + distance_after_collide_sqr > distance_to_pivot_sqr) {		//collition before next loop
			//calculate exact speed needed to collide at next loop

			float distance_to_past_sqr = distance_to_pivot_sqr - distance_after_collide_sqr;

			//the speed needed can be calculated as this:
			// x2 + m2x2 = Hypotenuse2, means (m2+1)x2 = distance_to_past_sqr

			float new_speedx_sqr = distance_to_past_sqr / (pow(m,2)+1);
			float new_speedy_sqr = new_speedx_sqr * pow(m,2);

			//change the speed values to new ones

			speed_x = sqrt(new_speedx_sqr) + 1;			//
			speed_y = sqrt(new_speedy_sqr) + 1;			// +1 to be sure, floats might cause troubles.

			//deposit the difference to speed_vector_from_collition_sqr

			speed_vector_from_collition_sqr = distance_after_collide_sqr;

			fprintf(stderr, "collition ahead");
		}


	}

}

void Puck::calculateSpeedAfterCollision(Mallet mallet) {		//this function assumes x and y is centered. fix this
	//calculate the line that collition created
	// mx+n = y is the formula we are trying to get.
	float m = ((mallet.y - y)/(mallet.x - x));
	float n = mallet.y - (m * mallet.x);

	//now get the pivot using right angled line.
	float n2 = (y - speed_y) + (m * (x - speed_x));	// + m since we need to reverse the tilt.

	float pivot_x = (n2 - n) / (2*m);
	float pivot_y = m * pivot_x + n;

	//now just get the mirror from the pivot point;
	float new_x = 2 * pivot_x - (x - speed_x);
	float new_y = 2 * pivot_y - (y - speed_y);
	//we know where the puck should go, calculate speed accordingly

	speed_x = new_x - x;
	speed_y = new_y - y;
}


