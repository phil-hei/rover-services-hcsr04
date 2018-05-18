/* Copyright 2018 Pedro Cuadra - pjcuadra@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <vector>

#include <demo/RoverInfraredDemo.h>

RoverInfraredDemo::RoverInfraredDemo(RoverInfraredSensor *inf_sensor, RoverDisplay * disp, RoverButtons * btn) {
  this->inf_sensor = inf_sensor;
  this->disp = disp;
  this->btn = btn;
}

int RoverInfraredDemo::run() {
  double sensor_val;

  this->disp->set_text_size(2);
  this->disp->set_text_color(1);

  while (!check_button()) {
    this->disp->clear_display();

    // Front right
    this->inf_sensor->read(rover_sensor_id::front_right, sensor_val);
    // Arrow
    this->disp->set_cursor(5, 10);
    this->disp->draw_triangle(5, 10, 15, 10, 5, 20, 1, true);

    this->disp->set_cursor(30, 10);
  	this->disp->print(to_string(sensor_val).c_str());

    // Front left
    this->inf_sensor->read(rover_sensor_id::front_left, sensor_val);
    // Arrow
    this->disp->set_cursor(5, 50);
    this->disp->draw_triangle(5, 50, 15, 60, 5, 60, 1, true);

    this->disp->set_cursor(30, 50);
    this->disp->print(to_string(sensor_val).c_str());

    this->inf_sensor->read(rover_sensor_id::rear_right, sensor_val);
    // Arrow
    this->disp->set_cursor(65, 10);
    this->disp->draw_triangle(65, 10, 75, 10, 75, 20, 1, true);


    this->disp->set_cursor(90, 10);
    this->disp->print(to_string(sensor_val).c_str());

    this->inf_sensor->read(rover_sensor_id::rear_left, sensor_val);
    // Arrow
    this->disp->set_cursor(65, 50);
    this->disp->draw_triangle(65, 60, 75, 60, 75, 50, 1, true);

    this->disp->set_cursor(90, 50);
    this->disp->print(to_string(sensor_val).c_str());

    this->disp->display();
  }

}

bool RoverInfraredDemo::check_button() {
  double state = 1;
  static bool trigered = false;

  this->btn->read(shutdown_button, state);

  if (trigered && state != 0) {
    trigered = false;
    return true;
  }

  if (state == 0) {
    trigered = true;
  }

  return false;
}
