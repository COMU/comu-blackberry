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

static bool shutdown;
static int orientation_angle;
static screen_context_t screen_cxt;
static float width, height, max_size;

typedef struct box_t {
	float x;
	float y;
	float size;
	GLfloat color;
	int visible;
} box;

static box* boxes;
static GLfloat vertices[8];
static char touch_count = 0;
#define MAX_BOXES 20

static float gravity_x, gravity_y;

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

	gravity_x = 0.0;
	gravity_y = 0.0;

	boxes = (box*) calloc(sizeof(box) * MAX_BOXES, sizeof(box));

	if (!boxes) {
		return EXIT_FAILURE;
	}


	//Set clear color to a shade of green for good looks
	glClearColor(0.0f, 0.25f, 0.0f, 1.0f);

	return EXIT_SUCCESS;
}

void set_gravity(float x, float y) {
	gravity_x = x;
	gravity_y = y;
}

void clear_cubes() {
	int i;
	for (i = 0; i < MAX_BOXES; i++) {
		boxes[i].visible = 0;
	}
}

void add_cube(float x, float y, int cube_to_add) {
	//Add a cube with a random shade of green and some size variation
	boxes[cube_to_add].color = ((float) rand()) / RAND_MAX;

	boxes[cube_to_add].x = (float) x;
	boxes[cube_to_add].y = height - y;
	boxes[cube_to_add].size = 40.0 + 20.0 * ((float) rand()) / RAND_MAX;
	boxes[cube_to_add].visible = 1;

	touch_count++;

}

void move_cube(float x, float y, int box_id) {
	boxes[box_id].x = (float) x;
	boxes[box_id].y = height - y;
}

void remove_cube(int cube_to_remove) {
	boxes[cube_to_remove].visible = 0;

	touch_count--;
}

void render() {
	int i;

	//Typical rendering pass
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertices);

	for (i = 0; i < MAX_BOXES; i++) {
		if(boxes[i].visible == 1) {
			glPushMatrix();

			glColor4f(boxes[i].color, 0.78f, 0, 1.0f);
			glTranslatef(boxes[i].x, boxes[i].y, 0.0f);
			glScalef(boxes[i].size, boxes[i].size, 1.0f);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glPopMatrix();
		}
	}

	if (touch_count == 3) {
		GLfloat _vertices[] = {
			(boxes[0].x + boxes[0].size/2), (boxes[0].y + boxes[0].size/2),
			(boxes[1].x + boxes[1].size/2), (boxes[1].y + boxes[1].size/2),
			(boxes[2].x + boxes[2].size/2), (boxes[2].y + boxes[2].size/2)
		};

		GLfloat _colors[] = {
			0.0f,	0.0f,	0.0f,	1.0f,
			0.0f,	0.0f,	0.0f,	1.0f,
			0.0f,	0.0f,	0.0f,	1.0f
		};

		glVertexPointer(2, GL_FLOAT, 0, _vertices);
		glColorPointer(4, GL_FLOAT, 0, _colors);

	    glPushMatrix();
	    glDrawArrays(GL_TRIANGLES, 0 , 3);
	    glPopMatrix();


	}

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
	if (screen_val == SCREEN_EVENT_MTOUCH_TOUCH) {
		//This is touch screen event.
		add_cube((float) pair[0], (float) pair[1], mtouch_event.contact_id);
		fprintf(stderr,"Touch position %d %d, added new cube as %d\n", pair[0], pair[1], mtouch_event.contact_id);
	} else if (screen_val == SCREEN_EVENT_MTOUCH_MOVE) {
		//This is touch screen event.
		move_cube((float) pair[0], (float) pair[1], mtouch_event.contact_id);
		fprintf(stderr,"Move position %d %d, move the cube %d\n", pair[0], pair[1], mtouch_event.contact_id);
	} else if (screen_val == SCREEN_EVENT_MTOUCH_RELEASE) {
		//This is touch screen event.
		remove_cube(mtouch_event.contact_id);
		fprintf(stderr,"Release position %d %d, removed cube %d\n", pair[0], pair[1], mtouch_event.contact_id);
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
		clear_cubes();
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

		set_gravity(result_x, result_y);
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
		set_gravity(0.0f, -1.0f);
	}

	//Start with one cube on the screen
	//add_cube(200, 100);

	while (!shutdown) {
		// Handle user input and sensors
		handle_events();

		//Update cube positions
		//update();

		// Draw Scene
		render();
	}

	//Stop requesting events from libscreen
	screen_stop_events(screen_cxt);

	//Shut down BPS library for this process
	bps_shutdown();

	//Free app data
	free(boxes);

	//Use utility code to terminate EGL setup
	bbutil_terminate();

	//Destroy libscreen context
	screen_destroy_context(screen_cxt);
	return 0;
}
