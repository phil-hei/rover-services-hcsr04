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
#include <app/RoverButtons.h>
#include <app/RoverDriving.h>

#include <Menu.h>
#include <demo/RoverBuzzerDemo.h>
#include <demo/RoverDrivingDemo.h>
#include <kuksa_logo.h>

using namespace std;

// Callback for handling shutdown menu
void shutdown_cb(Menu * menu, RoverButtons* btn, void * closure) {

  switch (menu->get_option()) {
    case 0: // Reset
      system("shutdown -r now");
      break;
    case 1: // Shutdown
      system("shutdown -h now");
      break;
    default:
      break;
  }

  return;
}

// Callback for handling Main menu
void main_cb(Menu * menu, RoverButtons* btn, void * closure) {

  return;
}

// Callback for handling demo menu
void demo_cb(Menu * menu, RoverButtons* btn, void * closure) {
  RoverBuzzerDemo * bzrd = (RoverBuzzerDemo *)closure;
  RoverDrivingDemo * drvd = (RoverDrivingDemo *)closure;

  switch (menu->get_option()) {
    case 0: // Buzzer
      bzrd->run();
      break;
    case 1: // Driving
      drvd->run();
      break;
    default:
      break;
  }

  return;
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

  // Create services objects
  RoverDisplay display(uri);
  RoverButtons btn(uri);
  RoverBuzzer bzr(uri);
  RoverDriving drv(uri);

  // Create demos objects
  RoverBuzzerDemo bzr_demo(&bzr);
  RoverDrivingDemo drv_demo(&drv, &display, &btn);

  // Create the menu objects
  Menu main_menu = Menu("Main", &btn, &display);
  Menu demo_menu = Menu("Demo", &btn, &display);
  Menu shut_menu = Menu("Shutdown", &btn, &display);
  Menu * curr_menu = &main_menu;

  // Add Main Menu options
  main_menu.add_submenu("1:Demo", &demo_menu);
  main_menu.add_option("2:Status", NULL, NULL);
  main_menu.add_option("3:Info", NULL, NULL);
  main_menu.add_submenu("4:Shutdown", &shut_menu);

  // Add Demo Menu options
  demo_menu.add_option("1:Buzzer", demo_cb, &bzr_demo);
  demo_menu.add_option("2:Driving", demo_cb, &drv_demo);
  demo_menu.add_option("3:Infrared", demo_cb, NULL);
  demo_menu.add_submenu("4:Back", &main_menu);

  // Add Shutdown menu options
  shut_menu.add_option("1:Reset", shutdown_cb, NULL);
  shut_menu.add_option("2:Shutdown", shutdown_cb, NULL);
  shut_menu.add_submenu("3:Back", &main_menu);


  while (true) {
    curr_menu->update();
    curr_menu = curr_menu->next();
    curr_menu->draw();
  }

  if (rc) {
    return -1;
  }

}
