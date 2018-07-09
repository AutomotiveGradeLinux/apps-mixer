###########################################################################
# Copyright 2018 IoT.bzh
#
# author: Loïc Collignon <loic.collignon@iot.bzh>
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

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick QuickControls2 WebSockets REQUIRED)

PROJECT_TARGET_ADD(mixer)

option(NATIVE_BUILD "Build for native environment, without homescreen and window manager" OFF)
if(NATIVE_BUILD)
    message(STATUS "Native build is ON")
    add_definitions(-DNATIVE_BUILD)
endif()

add_executable(mixer
	"main.cpp"
	"mixer.cpp"
        "Mixer.qrc"
)

set_target_properties(mixer PROPERTIES
	LABELS "EXECUTABLE"
	PREFIX ""
	COMPILE_FLAGS "${EXTRAS_CFLAGS} -DFOR_AFB_BINDING"
	LINK_FLAGS "${BINDINGS_LINK_FLAG}"
	LINK_LIBRARIES "${EXTRAS_LIBRARIES}"
	OUTPUT_NAME "${TARGET_NAME}"
)

target_link_libraries(mixer
    Qt5::Core
	Qt5::Quick
	Qt5::QuickControls2
	Qt5::WebSockets
	json-c
	afb-helpers
)

if(NOT NATIVE_BUILD)
    target_link_libraries(mixer
        homescreen
		qtwindowmanager
    )
endif()
