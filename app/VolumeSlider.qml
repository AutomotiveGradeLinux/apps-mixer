import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

RowLayout {
	anchors.left: parent.left
	anchors.right: parent.right
	Layout.minimumHeight: 75

	Label {
		font.pixelSize: 24
		text: modelData.name
		Layout.minimumWidth: 150
		Layout.maximumWidth: 150
		elide: Text.ElideRight
	}
	Label {
		font.pixelSize: 24
		text: modelData.value + " %"
		Layout.minimumWidth: 75
		Layout.maximumWidth: 75
		elide: Text.ElideRight
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
