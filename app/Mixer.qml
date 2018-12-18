/*
 * Copyright 2016 Konsulko Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import AGL.Demo.Controls 1.0
import Mixer 1.0

ApplicationWindow {
	// ----- Signals

	// ----- Properties
	property Component volumeSlider

	// ----- Setup
	id: root
	width: 1080 * roles.scale
	height: 1487 * roles.scale

	// ----- Childs
	Label {
		id: title
		font.pixelSize: 48
		text: "Mixer"
		anchors.horizontalCenter: parent.horizontalCenter
	}

	Mixer {
		signal sliderVolumeChanged(string role, int value)

		id: mixer

		Component.onCompleted: {
			mixer.open(bindingAddress);
		}
	}

	ListView {
		id: roles
		model: mixer.roles
		scale: scale_factor

		anchors.margins: 80
		anchors.top: title.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		delegate: VolumeSlider {}
	}
}
