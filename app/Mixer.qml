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
    width: container.width * container.scale
    height: container.height * container.scale

    // ----- Childs
    Mixer {
        // ----- Signals
        signal sliderVolumeChanged(string role, int value)

        // ----- Properties

        // ----- Setup
        id: mixer

        onSliderVolumeChanged: {
            console.log("======role: " + role + ", volume: " + value);
            mixer.setVolume(role, value);
        }

        Component.onCompleted: {
            var vs = Qt.createComponent("VolumeSlider.qml");
            if (vs.status !== Component.Ready) {
                console.log("Failed to load the VolumeSlider.qml component: " + vs.errorString());
            }
            root.volumeSlider = vs
            mixer.open(bindingAddress);
        }

        onRolesChanged: {
            // Remove existing sliders
            for(var i = sliders.children.length; i > 0 ; --i) {
                console.log("destroying: " + i);
                sliders.children[i-1].destroy();
            }

            // Add slider for each role
            for(var j = 0; j < mixer.roles.length; ++j) {
                addSlider(mixer.roles[j]);
            }
        }

        onVolumeChanged: {
            console.log("onVolumeChanged(\"" + name + "\", " + value + ")");
            for(var i = 0; i < sliders.children.length ; i++) {
                var sld = sliders.children[i];
                console.log(i + " - Slider found:" + sld + "[\"" + sld.role + "\"] = " + sld.value);
                if (sld.role === name) {
                    sld.value = value;
                }
            }
        }

        // ----- Functions
        function addSlider(name) {
            var sld = root.volumeSlider.createObject(sliders)
            sld.role = name
            sld.onSliderValueChanged.connect(mixer.sliderVolumeChanged)
            mixer.getVolume(name); // Update volume
        }

        function deleteChilds(item) {
            for(var i = item.children.length; i > 0 ; i--) {
                deleteChilds(item.children[i-1]);
            }
            item.destroy();
        }
    }

    Item {
        id: container
        anchors.centerIn: parent
        width: 1080
        height: 1487
        scale: screenInfo.scale_factor()

        Label {
            id: title
            font.pixelSize: 48
            text: "Mixer"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ColumnLayout {
            id: sliders
            anchors.margins: 80
            anchors.top: title.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}

