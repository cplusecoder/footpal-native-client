/**
 * @author  Morteza Sabetraftar <morteza.sabetraftar@gmail.com>
 */


import QtQuick 2.3;
import QtQuick.Controls 1.2;
import QtQuick.Controls.Styles 1.2;
import QtQuick.Layouts 1.1;
import ScreenTypes 1.0;
import "custom"
import "scripts/settings.js" as Settings


Rectangle {
    id: root
    anchors.fill: parent;
    anchors.centerIn: parent;
    color: Settings.globalBgColor;

    Component.onCompleted: {
    }

    Component.onDestruction: {
    }

    QtObject {
        id: privates

        property int textFieldHeight: root.height * 0.05
        property int textFieldMaxPixelSize: root.height * 0.04
        property int textFieldMinPixelSize: root.height * 0.02
    }

    Bar {
        id: topBar

        ExtButton {
            height: parent.height * 0.8;
            width: height;
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: (parent.height - height) / 2;
            defaultImage: "qrc:///img/btn_bar_return.png"
            pressedImage: "qrc:///img/btn_bar_return_pressed.png"
            onSignal_clicked: {
                pageLoader.setSource("qrc:///ui/Mailbox.qml")
            }
        }

        Text {
            anchors.centerIn: parent
            text: qsTr("COMPOSE_MAIL") + UiEngine.EmptyLangString;
            color: "white"
        }
    }

    Column {
        id: headerColumn
        anchors.top: topBar.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.margins: 5;
        spacing: 5;

        TextField {
            id: toTextField;
            style: textFieldStyle;
            width: parent.width;
            height: privates.textFieldHeight;
            font.pixelSize: privates.textFieldMaxPixelSize
            placeholderText: qsTr("TO") + UiEngine.EmptyLangString;
            focus: true;
        }

        TextField {
            id: subjectTextField;
            style: textFieldStyle;
            width: parent.width;
            height: privates.textFieldHeight;
            font.pixelSize: privates.textFieldMaxPixelSize
            placeholderText: qsTr("SUBJECT") + UiEngine.EmptyLangString;
        }

        TextField {
            id: ccTextField;
            style: textFieldStyle;
            width: parent.width;
            height: privates.textFieldHeight;
            font.pixelSize: privates.textFieldMaxPixelSize
            placeholderText: qsTr("CC") + UiEngine.EmptyLangString;
        }

        TextField {
            id: bccTextField;
            style: textFieldStyle;
            width: parent.width;
            height: privates.textFieldHeight;
            font.pixelSize: privates.textFieldMaxPixelSize
            placeholderText: qsTr("BCC") + UiEngine.EmptyLangString;
        }

        Text {
            text: qsTr("CONTENT") + UiEngine.EmptyLangString + ":";
            font.pixelSize: privates.textFieldMaxPixelSize;
            color: "white"
        }
    }

    TextArea {
        id: contentTextField;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: headerColumn.bottom
        anchors.bottom: bottomBar.top
        anchors.margins: 5

        property int lineChars: (width / (font.pixelSize * 0.8));
        property bool firstLine: true;
        property int charCount: 0
        font.pixelSize: Math.max(Math.min(height / lineCount, privates.textFieldMaxPixelSize),
                                 privates.textFieldMinPixelSize);
        onTextChanged: {
//            var len = text.length
//            var a = lineCount > 2 ? 2 : 0;
//            var b = (firstLine && lineCount > 1) ? 2 : 0;
//            console.log(a + " " + b + " " + lineCount);
            charCount++;
            console.log(charCount);
            if (charCount > lineChars) {
                charCount = -1;
                console.log("Append");
                append("");
            }
        }
    }

    Bar {
        id: bottomBar
        anchors.bottom: parent.bottom

        Item {
            anchors.fill: parent
            anchors.margins: 5

            ExtButton {
                id: buttonReply
                height: parent.height;
                width: height;
                anchors.left: parent.left
                defaultImage: "qrc:///img/btn_bar_send.png"
                pressedImage: "qrc:///img/btn_bar_send_pressed.png"
                onSignal_clicked: {
                    createJsonMessage();
                }
            }
        }
    }

    function createJsonMessage() {
        var subject = subjectTextField.text;
        var text = contentTextField.text;
        var recipients = [];
        var tos = toTextField.text.split(",");
        var ccs = ccTextField.text.split(",");
        var bccs = bccTextField.text.split(",");

        tos.forEach(function(target){
            recipients.push({ email: target, type: "TO" });
        });
        ccs.forEach(function(target){
            recipients.push({ email: target, type: "CC" });
        });
        bccs.forEach(function(target){
            recipients.push({ email: target, type: "BCC" });
        });

        var result = { subject: subject,
            text: text,
            to: recipients
        }

        var res = JSON.stringify(result);
        console.log(res);
        return res;
    }
}

