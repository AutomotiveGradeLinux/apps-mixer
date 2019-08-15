import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

ColumnLayout {
	anchors.left: parent.left
	anchors.right: parent.right
	Layout.minimumHeight: 125

	Label {
		font.pixelSize: 24
		text: modelData.name
	}
	Label {
		font.pixelSize: 24
		text: modelData.value + " %"
	}
	Slider {
		id: roleValueSlider
		Layout.fillWidth: true
		from: 0
		to: 100
		value: modelData.value
		stepSize: 1
		snapMode: Slider.SnapOnRelease
	}

	Binding {
		target: modelData
		property: "value"
		value: roleValueSlider.value
	}
}
