###########################################################################
# Copyright 2015, 2016, 2017 IoT.bzh
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

include_directories(${CMAKE_SOURCE_DIR}/rovermarkerdetect-afb/include)

# Add target to project dependency list
PROJECT_TARGET_ADD(rovermarkerdetect)

if(DEFINED ENV{SDKTARGETSYSROOT})
    message(STATUS "SDK Detected")
    set(SDK_EN 1)

    # Use OpenCV config from Yocto SDK (WA)
    #set(OpenCV_DIR $ENV{SDKTARGETSYSROOT}/usr/share/OpenCV)
    # Raspicam module
    #set(CMAKE_MODULE_PATH $ENV{SDKTARGETSYSROOT}/usr/lib/cmake ${CMAKE_MODULE_PATH})

else(DEFINED ENV{SDKTARGETSYSROOT})
    set(SDK_EN 0)
endif(DEFINED ENV{SDKTARGETSYSROOT})

#For find_package packages, export someextlib_DIR=/path/to/..
# Required packages
find_package(OpenCV REQUIRED HINTS $ENV{SDKTARGETSYSROOT}/usr/share/OpenCV)
find_package(raspicam REQUIRED HINTS  $ENV{SDKTARGETSYSROOT}/usr/lib/cmake ${CMAKE_MODULE_PATH})

message(STATUS ${OpenCV_DIRS})

# Define project Targets
ADD_LIBRARY(${TARGET_NAME} MODULE src/rovermarkerdetect-binding-auto.cpp  src/ServiceRoverMarkerDetect.cpp)

# Binder exposes a unique public entry point
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
    PREFIX "afb-"
    LABELS "BINDING"
    LINK_FLAGS  ${BINDINGS_LINK_FLAG}
    OUTPUT_NAME ${TARGET_NAME}
)

TARGET_LINK_LIBRARIES(${TARGET_NAME}
 ${link_libraries} roverapi ${raspicam_CV_LIBS}
)
