/*
 * Copyright (c) 2011 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/keycodes.h>
#include <screen/screen.h>
#include <assert.h>
#include <bps/sensor.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>
#include <bps/orientation.h>
#include <math.h>
#include <time.h>
#include <screen/screen.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include "bbutil.h"
#include "input/screen_helpers.h"
#include "mallet.h"

static bool shutdown;
static int orientation_angle;
static screen_context_t screen_cxt;
static float max_size;
float width, height;

float MALLETSIZE = 40.0f;
float PUCKSIZE = 30.0f;

class Puck {
public:
	float x;
	float y;
	float speed_x;
	float speed_y;

	float speed_vector_from_collition_sqr;

	Puck() { speed_x = 10; speed_y = 10; speed_vector_from_collition_sqr = 0;}

	float centerx() {return x + PUCKSIZE / 2; }
	float centery() {return y + PUCKSIZE / 2; }
	void move();
	void searchCollition(Mallet mallet);
	void calculateSpeedAfterCollision(Mallet mallet);
};

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
static Mallet mallet1(100, 256, MALLETSIZE, 0), mallet2(800, 256, MALLETSIZE, 0);
static Puck puck;

static GLfloat vertices[8];
void move_puck();

int init_blocks() {
	EGLint surface_width, surface_height;

	//Initialize common vertex data
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;

	vertices[2] = 1.0f;
	vertices[3] = 0.0f;

	vertices[4] = 0.0f;
	vertices[5] = 1.0f;

	vertices[6] = 1.0f;
	vertices[7] = 1.0f;

	//Initialize app data
	max_size = 60.0;

	//Query width and height of the window surface created by utility code
	eglQuerySurface(egl_disp, egl_surf, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_disp, egl_surf, EGL_HEIGHT, &surface_height);

	EGLint err = eglGetError();
	if (err != 0x3000) {
		fprintf(stderr, "Unable to query egl surface dimensions\n");
		return EXIT_FAILURE;
	}

	width = (float) surface_width;
	height = (float) surface_height;

	//Initialize GL for 2D rendering
	glViewport(0, 0, (int) width, (int) height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrthof(0.0f, width / height, 0.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Set world coordinates to coincide with screen pixels
	glScalef(1.0f / height, 1.0f / height, 1.0f);

	//Set clear color to a shade of green for good looks
	glClearColor(0.0f, 0.25f, 0.0f, 1.0f);

	//initialise the puck
	puck.x = 512;
	puck.y = 256;

	return EXIT_SUCCESS;
}

void clear_score() {
	puck.x = 512;
	puck.y = 256;
	puck.speed_x = 10;
	puck.speed_y = 10;
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
	float collide_distance_sqr = pow((PUCKSIZE + MALLETSIZE),2);
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

void move_puck() {
	/*
	 * 4 ways for start. center is x + size/2, y + size/2
	 */

	float collade_distance = (PUCKSIZE + MALLETSIZE) / 2;
	if(puck.x <= 0)
		puck.speed_x = -puck.speed_x;
	if(puck.y <= 0)
		puck.speed_y = -puck.speed_y;
	if (puck.x >= (1024 - MALLETSIZE))
		puck.speed_x = -puck.speed_x;
	if (puck.y >= (600 - MALLETSIZE))
		puck.speed_y = -puck.speed_y;

	if((pow(fabs(mallet1.centerx() - puck.centerx()),2) + pow(fabs(mallet1.centery() - puck.centery()),2)) <= pow(collade_distance,2)) {		//collision

		puck.calculateSpeedAfterCollision(mallet1);
	}

	if((pow(fabs(mallet2.centerx() - puck.centerx()),2) + pow(fabs(mallet2.centery() - puck.centery()),2)) <= pow(collade_distance,2)) {		//collision
		puck.calculateSpeedAfterCollision(mallet2);
	}


	puck.move();		//move according to speed settings

	/*
	 *	fprintf(stderr,"mallet1 position %f %f,\t", mallet1.x, mallet1.y);
	 *	fprintf(stderr,"puck position %f %f,\t", puck.x, puck.y);
	 *	fprintf(stderr,"mallet2 position %f %f,\n", mallet2.x, mallet2.y);
	 */
}

void render() {
	//Typical rendering pass
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertices);

	/* draw first mallet */
	glPushMatrix();

	glColor4f(mallet1.color, 0.78f, 0, 1.0f);
	glTranslatef(mallet1.x, mallet1.y, 0.0f);
	glScalef(MALLETSIZE*2, MALLETSIZE*2, 1.0f);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glPopMatrix();
	/* draw first mallet finished */
	/* draw second mallet */
	glPushMatrix();

	glColor4f(mallet2.color, 0.78f, 0, 1.0f);
	glTranslatef(mallet2.x, mallet2.y, 0.0f);
	glScalef(MALLETSIZE*2, MALLETSIZE*2, 1.0f);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glPopMatrix();
	/* draw second mallet finished */
	/* draw third mallet */
	glPushMatrix();

	//glColor4f(puck.color, 0.78f, 0, 1.0f);
	glTranslatef(puck.x, puck.y, 0.0f);
	glScalef(PUCKSIZE*2, PUCKSIZE*2, 1.0f);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glPopMatrix();
	/* draw third mallet finished */

	glDisableClientState(GL_VERTEX_ARRAY);

	//Use utility code to update the screen
	bbutil_swap();
}

static void handleScreenEvent(bps_event_t *event) {
	int screen_val, buttons;
	int pair[2];

	int return_error;
	static bool mouse_pressed = false;

	screen_event_t screen_event = screen_event_get_event(event);

	//Query type of screen event and its location on the screen
	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE,
			&screen_val);
	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION,
			pair);

	mtouch_event_t mtouch_event;
	return_error = screen_get_mtouch_event(screen_event, &mtouch_event, 0);

	if (return_error)
		fprintf(stderr, "Error: failed to get mtouch event\n");

	//There is a difference between touch screen events and mouse events
	if (screen_val == SCREEN_EVENT_MTOUCH_TOUCH || screen_val == SCREEN_EVENT_MTOUCH_MOVE) {
		//This is touch screen event.
		if(pair[0] < 1024/2)						//left half of the area, player
			mallet1.move(pair[0], pair[1]);
		else
			mallet2.move(pair[0], pair[1]);
	} else if (screen_val == SCREEN_EVENT_POINTER) {
		//This is a mouse move event, it is applicable to a device with a usb mouse or simulator
		screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_BUTTONS,
				&buttons);

		if (buttons == SCREEN_LEFT_MOUSE_BUTTON) {
			//Left mouse button is pressed
			mouse_pressed = true;
		} else {
			if (mouse_pressed) {
				//Left mouse button was released
				mouse_pressed = false;
				fprintf(stderr,"click position %d %d\n", pair[0], pair[1]);
			}
		}
	}
}

static void handleNavigatorEvent(bps_event_t *event) {
	switch (bps_event_get_code(event)) {
	case NAVIGATOR_SWIPE_DOWN:
		clear_score();
		break;
	case NAVIGATOR_EXIT:
		shutdown = true;
		break;
	}
}

static void handleSensorEvent(bps_event_t *event) {
	if (SENSOR_AZIMUTH_PITCH_ROLL_READING == bps_event_get_code(event)) {
		float azimuth, pitch, roll;
		float result_x = 0.0f, result_y = -1.0f;

		sensor_event_get_apr(event, &azimuth, &pitch, &roll);

		float radians = abs(roll) * M_PI / 180 ;//+ adjustment_angle;
		float horizontal = sin(radians) * 0.5f;
		float vertical = cos(radians) * 0.5f;

		if (pitch < 0) {
			vertical = -vertical;
		}
		if (roll >= 0) {
			horizontal = -horizontal;
		}

		//Account for axis change due to different starting orientations
		if (orientation_angle == 0) {
			result_x = horizontal;
			result_y = vertical;
		} else if (orientation_angle == 90) {
			result_x = -vertical;
			result_y = horizontal;
		} else if (orientation_angle == 180) {
			result_x = -horizontal;
			result_y = -vertical;
		} else if (orientation_angle == 270) {
			result_x = vertical;
			result_y = -horizontal;
		}

	}
}

static void handle_events() {
	int screen_domain = screen_get_domain();
	int navigator_domain = navigator_get_domain();
	int sensor_domain = sensor_get_domain();

	int rc;

	//Request and process available BPS events
	for(;;) {
		bps_event_t *event = NULL;
		rc = bps_get_event(&event, 0);
		assert(rc == BPS_SUCCESS);

		if (event) {
			int domain = bps_event_get_domain(event);

			if (domain == screen_domain) {
				handleScreenEvent(event);
			} else if (domain == navigator_domain) {
				handleNavigatorEvent(event);
			} else if (domain == sensor_domain) {
				handleSensorEvent(event);
			}
		} else {
			//No more events in the queue
			break;
		}
	}
}

int main(int argc, char **argv) {
	shutdown = false;

	//Create a screen context that will be used to create an EGL surface to to receive libscreen events
	screen_create_context(&screen_cxt, 0);

	//Initialize BPS library
	bps_initialize();

	//Determine initial orientation angle
	orientation_direction_t direction;
	orientation_get(&direction, &orientation_angle);

	//Use utility code to initialize EGL for rendering with GL ES 1.1
	if (EXIT_SUCCESS != bbutil_init_egl(screen_cxt, GL_ES_1)) {
		fprintf(stderr, "bbutil_init_egl failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	//Initialize application logic
	if (EXIT_SUCCESS != init_blocks()) {
		fprintf(stderr, "initialize failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	//Signal BPS library that navigator and screen events will be requested
	if (BPS_SUCCESS != screen_request_events(screen_cxt)) {
		fprintf(stderr, "screen_request_events failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	if (BPS_SUCCESS != navigator_request_events(0)) {
		fprintf(stderr, "navigator_request_events failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	//Signal BPS library that navigator orientation is not to be locked
	if (BPS_SUCCESS != navigator_rotation_lock(false)) {
		fprintf(stderr, "navigator_rotation_lock failed\n");
		bbutil_terminate();
		screen_destroy_context(screen_cxt);
		return 0;
	}

	//Setup Sensors
	if (sensor_is_supported(SENSOR_TYPE_AZIMUTH_PITCH_ROLL)) {
		//Microseconds between sensor reads. This is the rate at which the
		//sensor data will be updated from hardware. The hardware update
		//rate is set below using sensor_set_rate.
		static const int SENSOR_RATE = 25000;

		//Initialize the sensor by setting the rates at which the
		//sensor values will be updated from hardware
		sensor_set_rate(SENSOR_TYPE_AZIMUTH_PITCH_ROLL, SENSOR_RATE);

		sensor_set_skip_duplicates(SENSOR_TYPE_AZIMUTH_PITCH_ROLL, true);
		sensor_request_events(SENSOR_TYPE_AZIMUTH_PITCH_ROLL);
	} else {
		//set_gravity(0.0f, -1.0f);
	}

	//Start with one cube on the screen
	//add_cube(200, 100);

	while (!shutdown) {
		// Handle user input and sensors
		handle_events();

		//search collition

		//puck.searchCollition(mallet1);
		//puck.searchCollition(mallet2);
		//Update puck positions
		move_puck();


		// Draw Scene
		render();
	}

	//Stop requesting events from libscreen
	screen_stop_events(screen_cxt);

	//Shut down BPS library for this process
	bps_shutdown();

	//Free app data
	//free(mallet_list);
	//Use utility code to terminate EGL setup
	bbutil_terminate();

	//Destroy libscreen context
	screen_destroy_context(screen_cxt);
	return 0;
}
