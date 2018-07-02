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

// BUG: ValueChanged event is raised by sliders when you are moving the caret, should be raised only when you release it.
// TODO: Call mixer.setVolume(sliderName, Value) on value change
// TODO: Call mixer.getVolume(sliderName) on load

import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import AGL.Demo.Controls 1.0
import Mixer 1.0

ApplicationWindow {
    id: root

    Mixer {
        id: mixer
        Component.onCompleted: {
            mixer.open(bindingAddress)
        }
        onRolesChanged: {
            // Remove existing sliders
            for(var i = sliders.children.length; i > 0 ; --i) {
                console.log("destroying: " + i)
                sliders.children[i-1].destroy()
            }

            // Add slider for each role
            for(var j = 0; j < mixer.roles.length; ++j) {
                addSlider(mixer.roles[j])
            }
        }

        function addSlider(name) {
            Qt.createQmlObject("
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
RowLayout {
    property int value
    id: slider_" + name + "
            Layout.minimumHeight: 75
            Label {
                font.pixelSize: 24
                text: \"" + name+ "\"
                Layout.minimumWidth: 150
            }
            Label {
                id: slider_" + name + "_textvalue
                font.pixelSize: 24
                text: \"0 %\"
            }
            Slider {
                id: slider_" + name + "_slider
                Layout.fillWidth: true
                from: 0
                to: 100
                stepSize: 1
                snapMode: Slider.SnapOnRelease
                onValueChanged: {
                    slider_" + name + "_textvalue.text = value + \" %\"
                    mixer.setVolume(\"" + name + "\", value)
                }
				Component.objectName: {
                    mixer.getVolume(\"" + name + "\")
                }
            }
        }", sliders, "volumeslider")
        }

        function deleteChilds(item) {
            for(var i = item.children.length; i > 0 ; i--) {
                deleteChilds(item.children[i-1])
            }
            item.destroy()
        }
    }

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

