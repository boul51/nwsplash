import QtQuick 2.12
import QtQuick.Window 2.12
import com.nw 1.0

Window {
    id: rootWindow
    width: 640
    height: 480
    visible: true

    Rectangle {
        color: "black"
        anchors.centerIn: parent
        rotation: rotationParameter * 90
        width: parent.height
        height: parent.width
        Image {
            id: image
            anchors.fill: parent
        }

        Text {
            id: text
            width: parent.width - 60
            height: implicitHeight
            horizontalAlignment: Text.AlignHCenter
            anchors.centerIn: parent
            wrapMode: Text.WordWrap
        }
    }

    Connections {
        target: listener
        onCommandReceived: {
            switch (command.type()) {
            case Command.ShowImage:
                handleShowImage(command)
                break;
            case Command.ShowText:
                handleShowText(command)
                break;
            default:
                console.info("Don't know how to handle command type " + command.type())
            }
        }
    }

    function handleShowImage(command)
    {
        image.source = command.showImagePath()
    }

    function handleShowText(command)
    {
        text.anchors.horizontalCenterOffset = command.showTextX()
        text.anchors.verticalCenterOffset = command.showTextY()
        text.font.family = command.showTextFont()
        text.font.pixelSize = command.showTextFontPixelSize()
        text.text = command.showTextText()
    }
}
