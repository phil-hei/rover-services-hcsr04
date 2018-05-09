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
#include <service/ServiceRoverGrooveUltrasonicSensor.h>
#include <roverapi/rover_grooveultrasonic.hpp>

using namespace rover;

static RoverGrooveUltrasonic rearSensor(static_cast<const RoverSensorID_t>(rover_sensor_id::rear));

// TODO: Implement check of the presence of this sensor
static bool enabled = false;

ServiceRoverGrooveUltrasonicSensor::ServiceRoverGrooveUltrasonicSensor() {
  AFB_NOTICE("[ServiceRoverGrooveUltrasonicSensor] Constructor ");
}

int ServiceRoverGrooveUltrasonicSensor::init() {
  AFB_NOTICE("[ServiceRoverGrooveUltrasonicSensor] Init ");

  rearSensor.initialize();

  enabled = true;

  return 0;
}


/** Autogenrated doc for check */
int ServiceRoverGrooveUltrasonicSensor::check(bool &out_enable) {
  AFB_NOTICE("[ServiceRoverGrooveUltrasonicSensor] Check");

  out_enable = enabled;

  return 0;
}

/** Autogenrated doc for get_sig_pin */
int ServiceRoverGrooveUltrasonicSensor::get_sig_pin(int &out_sig_pin) {
  AFB_NOTICE("[ServiceRoverGrooveUltrasonicSensor] Get_sig_pin");

  out_sig_pin = rearSensor.getSigPin();

  return 0;
}

/** Autogenrated doc for read */
int ServiceRoverGrooveUltrasonicSensor::read(double &out_distance) {
  AFB_NOTICE("[ServiceRoverGrooveUltrasonicSensor] Read");

  out_distance = static_cast<double>(rearSensor.read());

  return 0;
}

/** Autogenrated doc for set_sig_pin */
int ServiceRoverGrooveUltrasonicSensor::set_sig_pin(const int in_sig_pin) {
  AFB_NOTICE("[ServiceRoverGrooveUltrasonicSensor] Set_sig_pin");

  rearSensor.setSigPin(in_sig_pin);

  return 0;
}
