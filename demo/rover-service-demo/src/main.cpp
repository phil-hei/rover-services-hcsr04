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

#include <systemd/sd-event.h>
#include <json-c/json.h>

#include <afb/afb-wsj1.h>
#include <afb/afb-ws-client.h>
#include <app/RoverDriving.h>
#include <app/RoverInfraredSensor.h>
#include <app/RoverGrooveUltrasonicSensor.h>
#include <app/RoverBuzzer.h>
#include <app/RoverUtils.h>
#include <app/RoverDisplay.h>
#include <app/RoverConfig.h>

#include "melodies.h"
#include "appstacle_logo.c"

using namespace std;

int test_rover_driving(char * uri) {
  int rc = 0;
  RoverDriving ws((const char *)uri);

  rc |= ws.setspeed(480);
	sleep(1);

	rc |= ws.goforward();
	sleep(1);

  rc |= ws.gobackward();
	sleep(1);

  rc |= ws.stop();
  sleep(1);

  rc |= ws.turnright();
	sleep(1);

  rc |= ws.turnleft();
	sleep(1);

  if (rc) {
    return -1;
  }
}

int test_rover_infraredsensor(char * uri) {
  int rc = 0;
  int count = 0;
  double sensor_val;
  RoverInfraredSensor ws((const char *)uri);

  while (count < 3) {
    rc |= ws.read(rover_sensor_id::front_right, sensor_val);
    printf("Front right sensor value: %f\n", sensor_val);
  	sleep(1);
    count++;
  }

  count = 0;

  while (count < 3) {
    rc |= ws.read(rover_sensor_id::front_left, sensor_val);
    printf("Front left sensor value: %f\n", sensor_val);
  	sleep(1);
    count++;
  }

  count = 0;

  while (count < 3) {
    rc |= ws.read(rover_sensor_id::rear_right, sensor_val);
    printf("Rear right sensor value: %f\n", sensor_val);
    sleep(1);
    count++;
  }

  count = 0;

  while (count < 3) {
    rc |= ws.read(rover_sensor_id::rear_left, sensor_val);
    printf("Rear left sensor value: %f\n", sensor_val);
    sleep(1);
    count++;
  }


  if (rc) {
    return -1;
  }
}

int test_rover_groovesensor(char * uri) {
  int rc = 0;
  int count = 0;
  double sensor_val;
  RoverGrooveUltrasonicSensor ws((const char *)uri);

  while (count < 3) {
    rc |= ws.read(rover_sensor_id::rear, sensor_val);
    printf("Rear sensor value: %f\n", sensor_val);
    sleep(1);
    count++;
  }

  count = 0;

  while (count < 3) {
    rc |= ws.read(rover_sensor_id::front, sensor_val);
    printf("Front sensor value#include <app/RoverUtils.h>: %f\n", sensor_val);
    sleep(1);
    count++;
  }


  if (rc) {
    return -1;
  }
}


int test_rover_buzzer(char * uri) {
  int rc = 0;

  RoverBuzzer ws((const char *)uri);

  rc |= ws.set_tone(300);
  sleep(1);
  rc |= ws.play_melody(mario_theme, MARION_THEME_SIZE, mario_theme_tempo, MARION_THEME_SIZE);


  if (rc) {
    return -1;
  }
}

int test_rover_utils(char * uri) {
  int rc = 0;
  bool status;
  int n_cores = 0;
  double * cores_util;

  RoverUtils ws((const char *)uri);

  rc |= ws.get_ethernet_status(status);
  printf("Ethernet Status: %s\n", status? "ON" : "OFF");

  rc |= ws.get_wlan_status(status);
  printf("WLAN Status: %s\n", status? "ON" : "OFF");

  rc |= ws.get_internet_status(status);
  printf("Internet Status: %s\n", status? "ON" : "OFF");

  rc |= ws.get_bluetooth_status(status);
  printf("Bluetooth Status: %s\n", status? "ON" : "OFF");

  rc |= ws.get_number_cores(n_cores);
  printf("Number Of Cores: %d\n", n_cores);

  cores_util = new double[n_cores];

  rc |= ws.get_core_utilization(cores_util, n_cores);
  for (int i = 0; i < n_cores; i++) {
      printf("Core %d: Utilization %f\n", i, cores_util[i]);
  }

  delete [] cores_util;

  if (rc) {
    return -1;
  }
}

// using namespace rover;

int test_rover_display(char * uri) {
  int rc = 0;
  bool status;
  // double * cores_util;
  // int n_cores = 0;

  RoverDisplay ws((const char *)uri);

  rc |= ws.draw_bitmap(0, 0, appstacle_logo, 1024, 128, 64, 1);
  // rc |= ws.clear_display();
  // RoverBase base = RoverBase();
  // RoverDisplay disp = RoverDisplay();
  // RoverHMC5883L other = RoverHMC5883L();
  // base.initialize();
  // other.initialize();
  // disp.initialize();
  // printf("Cleaning Display\n");
  // while (true) {
  //   disp.clearDisplay();
  //   // disp.drawLine(0, 0, 50, 50, 1);
  //   disp.display();
  // }
  // printf("Ethernet Status: %s\n", status? "ON" : "OFF");

  // delete [] cores_util;

  if (rc) {
    return -1;
  }
}

int test_rover_config(char * uri) {
  int rc = 0;
  string value;
  // double * cores_util;
  // int n_cores = 0;

  RoverConfig ws((const char *)uri);

  rc |= ws.get("MQTT_USERNAME_C", value);
  // rc |= ws.clear_display();
  // RoverBase base = RoverBase();
  // RoverDisplay disp = RoverDisplay();
  // RoverHMC5883L other = RoverHMC5883L();
  // base.initialize();
  // other.initialize();
  // disp.initialize();
  // printf("Cleaning Display\n");
  // while (true) {
  //   disp.clearDisplay();
  //   // disp.drawLine(0, 0, 50, 50, 1);
  //   disp.display();
  // }
  printf("Value: %s\n", value);

  // delete [] cores_util;

  if (rc) {
    return -1;
  }
}

/* entry function */
int main(int ac, char **av, char **env)
{
	int rc = 0;

	/*get port and token from the command arg*/
	char *port = av[1];
	char *token = av[2];
  char uri[500];

  sprintf(uri, "127.0.0.1:%s/api?token=%s", port, token);

  // Test Driving Service
  // rc |= test_rover_driving(uri);
  // Test Infrared Sensor
  // rc |= test_rover_infraredsensor(uri);
  // Test Groove Sensors
  // rc |= test_rover_groovesensor(uri);
  // Test Buzzer
  // rc |= test_rover_buzzer(uri);
  // Test Utils
  // rc |= test_rover_utils(uri);
  rc |= test_rover_config(uri);

  if (rc) {
    return -1;
  }

}
