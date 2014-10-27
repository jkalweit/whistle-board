import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    visible: true
    width: 1024
    height: 800
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    Button {
        anchors.top: parent.top
        anchors.right: parent.right
        text: "Do Test"
        onClicked: app.doTest()
    }

    function createBuckets() {
        var min = 5000;
        var max = 15000;
        var numBuckets = 16;
        var bucketSize = (max - min) / numBuckets;
        var buckets = [];
        for(var i = 0; i < 16; i++) {
            buckets[i] = {
                bucket: i,
                min: min + (i * bucketSize),
                max: min + ((i + 1) * bucketSize)
            };
        }
        return buckets;
    }

    Grid {
        anchors.fill: parent
        rows: 4
        columns: 4
        spacing: 2

        Repeater {
            model: createBuckets()

            Rectangle {
                width: 100
                height: 100
                color: mouseArea.pressed ? "#DDDDFF" : "#7777AA"

                Text {
                    anchors.centerIn: parent
                    text: modelData.min + " - " + modelData.max
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent

                }
            }
        }
    }


    Rectangle {
        id: chart
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height
        color: "#CC7777DD"

        Repeater {
            model: app.whistleListener.frequencies

            Rectangle {
                 width: chart.width / (app.whistleListener.N / 2)
                 height: modelData.value
                 anchors.bottom: chart.bottom
                 anchors.left: chart.left
                 anchors.leftMargin: width * index
                 color: "#CCDD7777"
            }
        }
    }


}
