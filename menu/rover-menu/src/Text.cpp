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
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <vector>

#include <Text.h>

using namespace std;

Text::Text(RoverButtons* btn, RoverDisplay *disp) {
  this->btn = btn;
  this->disp = disp;
}

void Text::scroll_down() {
  this->pos++;

  if (this->pos >= fulltext.size()) {
    this->pos = 0;
  }
}


int Text::wrap_long_word(string word) {
  int i = 0;
  for (i = 0; i < word.size(); i+=this->num_chars_line) {
    this->fulltext.push_back(word.substr(i, this->num_chars_line));
  }

  this->fulltext.push_back(word.substr(i));

  return word.size() - i;
}

void Text::add_text(string text) {

  istringstream ss(text);
  string word;
  string line;
  int count = 0;


  // Easiest case text fits in screen
  if (text.size() < this->num_chars_line) {
    fulltext.push_back(text);
    return;
  }

  while (ss >> word) {
    if (word.size() >= this->num_chars_line) {
      count = wrap_long_word(word);
    }

    if (word.size() + count < this->num_chars_line) {
      line += word;
      count += word.size();
    } else {
      fulltext.push_back(line);
      line = word;
      count = word.size();
    }

    // Add space between words if possible
    if (count + 1 < this->num_chars_line) {
      line += " ";
      count++;
    }

  }

}

void Text::draw() {
  int rc = 0;
  bool status;
  int start_opt = 0;
  int end_opt = 3;
  int text_color = 1;
  int bg_color = 0;
  uint screen_lines = 64 / this->line_height;


  rc |= this->disp->clear_display();

  this->disp->set_text_size(this->char_size);
	this->disp->set_text_color(1);

  start_opt = 0;
  if (this->pos >= screen_lines) {
    start_opt = 1 + this->pos - screen_lines;
  }

  for (int i = 0; i < screen_lines; i++) {
    this->disp->set_cursor(5, 2 + 20 * i);
  	this->disp->print(this->fulltext[i + start_opt].c_str());
  }

  this->disp->display();
}

void Text::update() {
  double state = 1;
  static bool scroll_trigered = false;
  static bool exit_trigered = false;

  this->btn->read(shutdown_button, state);

  if (scroll_trigered && state != 0) {
    scroll_trigered = false;
    this->scroll_down();
  }

  if (state == 0) {
    scroll_trigered = true;
  }

  this->btn->read(user_button, state);

  if (exit_trigered && state != 0) {
    exit_trigered = false;
    this->running = false;
  }

  if (state == 0) {
    exit_trigered = true;
  }
}

void Text::print() {
  while (this->running) {
    this->update();
    this->draw();
  }
}
