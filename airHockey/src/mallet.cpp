/*
 * mallet.cpp
 *
 *  Created on: 11 Þub 2012
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
	this->isMallet = 1;
}

Mallet::Mallet(float x, float y,float size, GLfloat color) {
	this->x = x;
	this->y = y;
	this->color = color;
	this->size = size;
	speed_x = 0;
	speed_y = 0;
	this->isMallet = 1;
}

void Mallet::move(float coord_x, float coord_y, Mallet &puck) {

	//the parameter x and y is touch coordinates, our x and y defines left bottom corner.
	//to fix this, we will subtract size
	speed_x = (coord_x - size) - x;
	speed_y = (height - coord_y - size) - y ;

	searchCollition(&puck);

	x = coord_x - size;
	y = height - coord_y - size;
}

Puck::Puck(float size, float speed_x, float speed_y) {
	this->size = size;
	this->speed_x = speed_x;
	this->speed_y = speed_y;
	speed_vector_from_collition_sqr = 0;
	this->isMallet = 0;
}

void Puck::move(Mallet &mallet1, Mallet &mallet2) {

	searchCollition(&mallet1);
	searchCollition(&mallet2);

	float MAXSPEED = 10.0F;
	if(speed_x > MAXSPEED) {
		speed_x = MAXSPEED;
	}
	if(speed_x < -1 *MAXSPEED) {
		speed_x = -1 * MAXSPEED;
	}
	if(speed_y >  MAXSPEED) {
		speed_y = MAXSPEED;
	}
	if(speed_y < -1 * MAXSPEED) {
		speed_y = -1 * MAXSPEED;
	}

	 x += speed_x;
	 y += speed_y;
}


void Mallet::searchCollition(Mallet *mallet) {
	//TODO is speed_x is 0, the program will collapse.

	//calculate the line that mallet moves
	//the formula for line is mx+n=y

	float m = speed_y / speed_x;
	float n = centery() - m * centerx();

	//now we have a line starts from center of our mallet
	//calculate a line with right angle to first one from mallet2

	float m_pivot = -1 * speed_x / speed_y;
	float n_pivot = mallet->centery() - m_pivot * mallet->centerx();

	//calculate pivot point:

	float pivot_x = (n - n_pivot) / (m_pivot - m);
	float pivot_y = m * pivot_x + n;

	//now look is the mallet2 close enough to collide
	float pivot_lenght_sqr = (pow(mallet->centerx()-pivot_x,2) + pow(mallet->centery()-pivot_y,2));
	float collide_distance_sqr = pow((size + mallet->size),2);
	if (pivot_lenght_sqr < collide_distance_sqr) {
		//collision is possible, look if it will be within this loop:

		//in which point the collition will happen?
		//calculating the triangle, mallet base, pivot point and collition point.
		float distance_after_collide_sqr = collide_distance_sqr - pivot_lenght_sqr;

		//calculate distance between puck and pivot
		float distance_to_pivot_sqr = pow(pivot_x - centerx(),2) + pow(pivot_y - centery(),2);

		//calculate distance that will be taken in this loop
		float distance_to_go_sqr = pow(speed_x,2) + pow(speed_y,2);
		float distance_to_collide_sqr = distance_to_pivot_sqr - distance_after_collide_sqr;
		if(distance_to_go_sqr >= distance_to_collide_sqr) {		//collition in this loop
			//calculate collition line
			float rate_of_move = sqrt(abs(distance_to_go_sqr / distance_to_collide_sqr));

			float distance_x = rate_of_move * speed_x;
			float distance_y = rate_of_move * speed_y;

			float collition_x = centerx() + distance_x;
			float collition_y = centery() + distance_y;

			//TODO moving the mallet to the collition point at this stage might be useful.

			this->x = collition_x - size;
			this->y = collition_y - size;

			//calculating m for collition line

			float mc = (collition_y - mallet->centery()) / (collition_x - mallet->centerx());

			float collide_angle_tan = tan((m - mc)/ (1 + m * mc));

			//collide line slope and tanjent calculated

			fprintf(stderr, "there will be a collition with ");
			if(isMallet == 1) {
				fprintf(stderr, "mallet\n" );
				//since we calculate mallet move while puck is not moving,
				//mallet always hits puck with the same line of its speed.

				//get reflection of speed_x and speed_y to the collition line
				float n_for_speed = mallet->speed_y + m * mallet->speed_x;		//+m since we are calculating a pivot point.
				float pivot_x_for_speed = n_for_speed / (2*m);
				float pivot_y_for_speed = pivot_x_for_speed * m;

				//calculate new speed values to the pivot:
				mallet->speed_x = mallet->speed_x - 2 *(mallet->speed_x - pivot_x_for_speed);
				mallet->speed_y = mallet->speed_y - 2 *(mallet->speed_y - pivot_y_for_speed);

				float pi = 3.14159265;
				float difference = atan2(mallet->speed_x, mallet->speed_y) - atan2(speed_x, speed_y);
				if ( (abs(difference) > (pi / 2)) && (abs(difference) < (pi * 1.5))) {
					fprintf(stderr, "reverse\n");
					mallet->speed_x = - mallet->speed_x;
					mallet->speed_y = - mallet->speed_y;
				}

				float new_speed_x = mallet->speed_x + speed_x;
				float new_speed_y = mallet->speed_y + speed_y;

				mallet->speed_x = new_speed_x;
				mallet->speed_y = new_speed_y;


				while((pow((centerx() + speed_x) - mallet->centerx(),2) + pow((centery() + speed_y) - mallet->centery(),2)) <= pow(mallet->size + size,2)) {
					//since the parameter we take is Mallet, accessing Puck.move practically impossible, so here is a little copy code.

					mallet->x += mallet->speed_x;
					mallet->y += mallet->speed_y;
				}

			} else {
				//puck collition part

				fprintf(stderr, "puck ");

				//the collition angle is bigger than 45 degrees.

				// we know the collition point, and move lines formula,
				// we need to mirror the collition line from move line.

				// to find new speed line, I will use center of mallet,
				// since we already have the pivot line and pivot length.

				//re use of variable distance_x/y.

				distance_x = mallet->centerx() - pivot_x;
				distance_y = mallet->centery() - pivot_y;

				float new_x = collition_x - distance_x;
				float new_y = collition_y - distance_y;

				float new_m = (new_y - collition_y) / (new_x - collition_x);

				//calculate new speed values
				// speed_x^2 + speed_y^2 = (m * new_speed_x)^2 + new_speed_x^2

				float new_speed_x = (new_m * new_m + 1)	/	distance_to_go_sqr;
				new_speed_x = sqrt(abs(new_speed_x));
				float new_speed_y = new_m * new_speed_x;

				this->speed_x = new_speed_x;
				this->speed_y = new_speed_y;

				if(abs(collide_angle_tan) <= 1) {
					fprintf(stderr, " with tan <= 1\n");
					// the collition angle is smaller than 45 degrees.

					// this means we need to mirror the speed on pivot too.

					// find the distance of new speed to pivot line
					// since the move line was already with the right angle,
					// use its slope.

					float n_new_speed = (collition_y + speed_y) - m * (collition_x + speed_x);

					//find the intersection

					float intersection_x = (n_new_speed - n_pivot) / (m_pivot - m);
					float intersection_y = m * intersection_x + n_new_speed;

					distance_x = collition_x + speed_x - intersection_x;
					distance_y = collition_y + speed_y - intersection_y;

					float new_x = collition_x - distance_x;
					float new_y = collition_y - distance_y;

					float new_m = (new_y - collition_y) / (new_x - collition_x);

					float new_speed_x = (new_m * new_m + 1)	/	distance_to_go_sqr;
					new_speed_x = sqrt(abs(new_speed_x));
					float new_speed_y = new_m * new_speed_x;

					this->speed_x = new_speed_x;
					this->speed_y = new_speed_y;

				} else {
					fprintf(stderr, "\n");
				}

				rate_of_move = 1 - rate_of_move;
				this->x = x + speed_x * rate_of_move;
				this->y = y + speed_y * rate_of_move;

			}


		}

	}

}
