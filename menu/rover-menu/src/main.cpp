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

#include "melodies.h"
#include "appstacle_logo.c"

using namespace std;

RoverDisplay display;

class Menu {
public:
  char * name;

  Menu(char *name) {
    this->name = name;
  }

  int get_max_size() {
    return options.size();
  }

  void set_option(int opt) {
    this->opt = opt;
  }

  int get_option(){
    return this->opt;
  }

  void inc_option() {
    this->opt++;

    if (this->opt >= this->get_max_size()) {
      this->opt = 0;
    }
  }

  void add_option(char * option, void (*callback)(Menu* menu, void * closure), void * closure) {
    options.push_back(option);
    submenu.push_back(NULL);
    callbacks.push_back(callback);
    cookies.push_back(closure);
  }

  void add_submenu(char * option, Menu *menu) {
    options.push_back(option);
    submenu.push_back(menu);
    callbacks.push_back(NULL);
    cookies.push_back(NULL);
  }

  void draw(RoverDisplay &disp) {
    int rc = 0;
    bool status;
    int start_opt = 0;
    int end_opt = 3;
    int text_color = 1;
    int bg_color = 0;


    rc |= disp.clear_display();

    disp.set_text_size(2);
  	disp.set_text_color(1);

    start_opt = 0;
    if (this->opt >= 3) {
      start_opt = 1 + this->opt - 3;
    }

    disp.set_cursor(100, 5);
    disp.print(to_string_s(this->opt));

    disp.set_cursor(100, 25);
    disp.print(to_string_s(start_opt));

    for (int i = 0; i < 3; i++) {
      if ((i + start_opt) == this->opt) {
        bg_color = 1;
      } else {
        bg_color = 0;
      }

      disp.set_text_color(text_color);
      disp.draw_rectangle(0, i*20, 128, 20, bg_color, false);

      disp.set_cursor(5, 2 + 20 * i);
    	disp.print(options[i + start_opt]);
    }

    disp.display();
  }

  Menu * select() {
    Menu * ret;
    if (submenu[this->opt] != NULL) {
      ret = submenu[this->opt];
      this->set_option(0);
      return ret;
    }

    if (callbacks[this->opt]) {
      callbacks[this->opt](this, cookies[this->opt]);
    }

    return this;
  }

private:
  vector<char *> options;
  vector<Menu *> submenu;
  vector<void (*)(Menu* menu, void * closure)> callbacks;
  vector<void *> cookies;
  uint opt = 0;
};

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
  Menu * curr_menu = &main_menu;

  main_menu.add_submenu("demo", &demo_menu);
  main_menu.add_option("status", NULL, NULL);
  main_menu.add_option("info", NULL, NULL);
  main_menu.add_option("shutdown", NULL, NULL);

  demo_menu.add_option("buzzer", demo_cb, &bzr);
  demo_menu.add_option("driving", demo_cb, &bzr);
  demo_menu.add_option("infrared", demo_cb, &bzr);
  demo_menu.add_submenu("back", &main_menu);


  while (true) {
    update_curr_option(btn, curr_menu);

    curr_menu = update_select_option(btn, curr_menu);

    curr_menu->draw(display);
  }

  if (rc) {
    return -1;
  }

}
