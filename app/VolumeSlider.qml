import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

RowLayout {
    property int value
    property string role

    onRoleChanged: sliderName.text = role
    onValueChanged: {
        sliderValue.text = value + " %"
        sliderControl.value = value;
    }

    //id: slider_" + name + "
    Layout.minimumHeight: 75
    Label {
        id: sliderName
        font.pixelSize: 24
        text: role
        Layout.minimumWidth: 150
    }
    Label {
        id: sliderValue
        //id: slider_" + name + "_textvalue
        font.pixelSize: 24
        text: "0 %"
    }
    Slider {
        id: sliderControl
        //id: slider_" + name + "_slider
        Layout.fillWidth: true
        from: 0
        to: 100
        stepSize: 1
        snapMode: Slider.SnapOnRelease
        onValueChanged: {
            //slider_" + name + "_textvalue.text = value + \" %\"
            //mixer.setVolume(\"" + name + "\", value)
        }
        //Component.onCompleted: {
            //mixer.getVolume(\"" + name + "\")
        //}
    }
}
