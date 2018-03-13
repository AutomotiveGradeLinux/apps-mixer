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
	id: root

    Mixer {
        id: mixer
        Component.objectName: {
            mixer.open(bindingAddress)
        }
        onMasterVolumeChanged: sliderMasterVolume.value = masterVolume
        onPcmVolumeChanged: sliderPcmVolume.value = pcmVolume
        onMicrophoneVolumeChanged: sliderMicrophoneVolume.value = microphoneVolume
    }

	Label { 
		id: title
		font.pixelSize: 48
		text: "Mixer"
		anchors.horizontalCenter: parent.horizontalCenter
	}

    ColumnLayout {
        anchors.margins: 80
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        ComboBox {
            id: soundCardSelector
            anchors.left: parent.left
            anchors.right: parent.right
            model: mixer.hals
            onCurrentIndexChanged: mixer.ActiveHal = currentIndex
        }

        RowLayout {
            Layout.minimumHeight: 75
            Label {
                font.pixelSize: 24
                text: "Master"
                Layout.minimumWidth: 150
            }
            Label {
                id: textMasterVolume
                font.pixelSize: 24
                text: "0 %"
            }
            Slider {
                id: sliderMasterVolume
                Layout.fillWidth: true
                from: 0
                to: 100
                stepSize: 1
                snapMode: Slider.SnapOnRelease
                onValueChanged: {
                    textMasterVolume.text = value + " %"
                    mixer.masterVolume = value
                }
            }
        }
/*
        RowLayout {
            Layout.minimumHeight: 75
            Label {
                font.pixelSize: 24
                text: "PCM"
                Layout.minimumWidth: 150
            }
            Label {
                font.pixelSize: 24
                text: "0 %"
            }
            Slider {
                id: sliderPcmVolume
                Layout.fillWidth: true
                from: 0
                to: 100
                stepSize: 1
                snapMode: Slider.SnapOnRelease
                onValueChanged: mixer.pcmVolume = value
            }
        }
*/
        RowLayout {
            Layout.minimumHeight: 75
            Label {
                font.pixelSize: 24
                text: "Microphone"
                Layout.minimumWidth: 150
            }
            Label {
                id: textMicrophoneVolume
                font.pixelSize: 24
                text: "0 %"
            }
            Slider {
                id: sliderMicrophoneVolume
                Layout.fillWidth: true
                from: 0
                to: 100
                stepSize: 1
                snapMode: Slider.SnapOnRelease
                onValueChanged: {
                    textMicrophoneVolume.text = value + " %"
                    mixer.microphoneVolume = value
                }
            }
        }
    }
}
