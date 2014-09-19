/**
 * @author  Morteza Sabetraftar <morteza.sabetraftar@gmail.com>
 */


import QtQuick 2.3;
import QtQuick.Controls 1.1;
import QtQuick.Controls.Styles 1.2;
import QtQuick.Layouts 1.1;
import ScreenTypes 1.0;
import "custom"
import "utils"

Rectangle {
    id: root
    anchors.fill: parent;
    anchors.centerIn: parent;
    color: "#203070";

    QtObject {
        id: privates

        property bool isInitialized: false
        property int barHeight: UiEngine.TargetScreenType === ScreenType.Phone ? root.height * 0.08 : 40
        property int itemHeight: UiEngine.TargetScreenType === ScreenType.Phone ? root.height * 0.2 : 100
        property int itemSpacing: UiEngine.TargetScreenType === ScreenType.Phone ? root.height * 0.01 : 5
    }

    Component.onCompleted: {
        privates.isInitialized = true;
        privates.itemHeight = UiEngine.TargetScreenType === ScreenType.Phone ? root.height * 0.2 : 100
    }

    Rectangle {
        id: topBar
        width: root.width
        height: privates.barHeight
        color: "#333"
        z: 1

        ExtButton {
            height: parent.height * 0.8;
            width: height;
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: (parent.height - height) / 2;
            defaultImage: "qrc:///img/btn_bar_return.png"
            pressedImage: "qrc:///img/btn_bar_return_pressed.png"
            onSignal_clicked: {
                pageLoader.setSource("qrc:///ui/Home.qml")
            }
        }

        Text {
            anchors.centerIn: parent
            text: qsTr("CONVERSATIONS") + UiEngine.EmptyLangString;
            color: "white"
        }
    }

    JSONListModel {
        id: jsonModel
        json: "{ \"log\" : { \"conversation\" : [ "
              + "{ \"contact\" : \"Majid\", \"date\" : \"9/17/2014\", \"time\": \"15:24\" }, "
              + "{ \"contact\" : \"Sadegh\", \"date\" : \"10/17/2014\", \"time\": \"13:12\" }, "
              + "{ \"contact\" : \"Morteza\", \"date\" : \"11/17/2014\", \"time\": \"10:44\" }"
              + " ] } }"
        query: "$.log.conversation[*]"
    }

    ListModel {
        id: model1

        ListElement { contact: "Majid"; date: "9/17/2014"; time: "15:24" }
        ListElement { contact: "Majid"; date: "9/17/2014"; time: "15:24" }
        ListElement { contact: "Majid"; date: "9/17/2014"; time: "15:24" }
        ListElement { contact: "Majid"; date: "9/17/2014"; time: "15:24" }
        ListElement { contact: "Majid"; date: "9/17/2014"; time: "15:24" }
    }

    ListView {
        anchors.top: topBar.bottom
        anchors.bottom: parent.bottom
        width: parent.width
        spacing: privates.itemSpacing
        model: jsonModel.model;
        delegate: Component {
            Rectangle {
                color: "#aaffffff"
                width: parent.width
                height: privates.itemHeight

                Image {
                    id: contactImage
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.margins: 5
                    width: height
                    source: "qrc:///img/Splash.png"
                }
                Text {
                    id: contactText
                    anchors.left: contactImage.right
                    anchors.top: parent.top
                    anchors.margins: 5
                    text: model.contact
                }
                Text {
                    id: dateText
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 5
                    text: model.date
                }
                Text {
                    id: timeText
                    anchors.right: parent.right
                    anchors.top: dateText.bottom
                    anchors.margins: 5
                    text: model.time
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        pageLoader.setSource("qrc:///ui/Chat.qml")
                    }
                }
            }
        }
    }
}
