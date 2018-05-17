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

#include <Menu.h>
#include <melodies.h>
#include <kuksa_logo.h>

using namespace std;

bool update_curr_option(RoverButtons &btn, Menu *menu) {
  double state = 1;
  static bool trigered = false;

  btn.read(shutdown_button, state);

  if (trigered && state != 0) {
    trigered = false;
    menu->inc_option();
  }

  if (state == 0) {
    trigered = true;
  }
}

Menu * update_select_option(RoverButtons &btn, Menu * menu) {
  double state = 1;
  static bool trigered = false;

  btn.read(user_button, state);

  if (trigered && state != 0) {
    trigered = false;
    return menu->select();
  }

  if (state == 0) {
    trigered = true;
  }

  return menu;
}

void shutdown_cb(Menu * menu, void * closure) {

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

  if (menu->get_option() == 0) {
    system("shutdown -r now");
  }

  return;
}

void main_cb(Menu * menu, void * closure) {

  if (menu->get_option() == 3) {
    system("shutdown -f");
  }

  return;
}

void demo_cb(Menu * menu, void * closure) {

  if (menu->get_option() == 0) {
    RoverBuzzer * bzr = (RoverBuzzer *)closure;
    bzr->play_melody(mario_theme, MARION_THEME_SIZE, mario_theme_tempo, MARION_THEME_SIZE);
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

  RoverDisplay display(uri);

  sprintf(uri, "127.0.0.1:%s/api?token=%s", port, token);
  RoverButtons btn(uri);
  RoverBuzzer bzr(uri);


  Menu main_menu = Menu("Main");
  Menu demo_menu = Menu("Demo");
  Menu shut_menu = Menu("Shutdown");
  Menu * curr_menu = &main_menu;

  main_menu.add_submenu("demo", &demo_menu);
  main_menu.add_option("status", NULL, NULL);
  main_menu.add_option("info", NULL, NULL);
  main_menu.add_submenu("shutdown", &shut_menu);

  demo_menu.add_option("buzzer", demo_cb, &bzr);
  demo_menu.add_option("driving", demo_cb, &bzr);
  demo_menu.add_option("infrared", demo_cb, &bzr);
  demo_menu.add_submenu("back", &main_menu);

  shut_menu.add_option("reset", shutdown_cb, NULL);
  shut_menu.add_option("shutdown", shutdown_cb, NULL);
  shut_menu.add_submenu("back", &main_menu);


  while (true) {
    update_curr_option(btn, curr_menu);

    curr_menu = update_select_option(btn, curr_menu);

    curr_menu->draw(display);
  }

  if (rc) {
    return -1;
  }

}
