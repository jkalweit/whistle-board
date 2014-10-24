import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    visible: true
    width: 640
    height: 480
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
}
