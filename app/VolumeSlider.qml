import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

RowLayout {
    property int value
    property string role
    signal sliderValueChanged(string role, int value)

    onRoleChanged: sliderName.text = role
    onValueChanged: {
        sliderValue.text = value + " %"
        sliderControl.value = value;
    }

    Layout.minimumHeight: 75
    Label {
        id: sliderName
        font.pixelSize: 24
        text: role
        Layout.minimumWidth: 150
    }
    Label {
        id: sliderValue
        font.pixelSize: 24
        text: "0 %"
    }
    Slider {
        id: sliderControl
        Layout.fillWidth: true
        from: 0
        to: 100
        stepSize: 1
        snapMode: Slider.SnapOnRelease
        onValueChanged: {
            sliderValue.text = value + " %";
            sliderValueChanged(role, value);
        }
    }
}
