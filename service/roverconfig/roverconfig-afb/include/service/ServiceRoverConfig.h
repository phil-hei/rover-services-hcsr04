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

/* WARNING: This file is automatically generated by raml2agl */
#ifndef __RAML2AGL_SERVICE_CLASS_SERVICEROVERCONFIG_H_
#define __RAML2AGL_SERVICE_CLASS_SERVICEROVERCONFIG_H_

#pragma once
#include <cstddef>
#include <string.h>
#include <string>

extern "C"
{
    #define AFB_BINDING_VERSION 2
    #include <afb/afb-binding.h>
};

enum rover_button_id {
  user_button,
  shutdown_button,
  custom_button
};
enum rover_demo_id {
  buzzer,
  driving,
  buttons,
  infrared,
  rover_demo_max
};
enum rover_sensor_id {
  rear_right,
  front_right,
  rear_left,
  front_left,
  front,
  rear
};

class ServiceRoverConfig {
public:
  ServiceRoverConfig();

  int init();

    /** Autogenrated doc for get */
    int get(const std::string in_key,
        std::string &out_value);

};

#endif //__RAML2AGL_SERVICE_CLASS_SERVICEROVERCONFIG_H_