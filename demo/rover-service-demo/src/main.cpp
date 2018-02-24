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

int test_rover_driving(char * uri) {\
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
  rc |= test_rover_driving(uri);

  if (rc) {
    return -1;
  }

}