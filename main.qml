import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    visible: true
    width: 1024
    height: 800
    title: qsTr("Whistle Board")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }    

    Connections {
        target: app.whistleListener
        onIsTriggeredChanged: {
            if(app.whistleListener.isTriggered) {
                console.log("triggered");
                if(app.whistleListener.currentFrequency) {
                    keyTimer.characterIndex++;
                    keyTimer.restart();
                }
            } else {
                console.log("un-triggered");
            }
        }
    }

    Timer {
        id: keyTimer
        interval: 500; running: false; repeat: false;
        onTriggered: {
            var character = app.whistleListener.currentFrequency.getCharacter(characterIndex);
            if(character === "bkspc") {
                currentText.text = currentText.text.substring(0, currentText.text.length - 1);
            } else if(character === "clear") {
                currentText.text = "";
            } else {
                currentText.text = currentText.text + character;
            }
            keyTimer.characterIndex = -1;
        }
        property int characterIndex: -1
    }


    Rectangle {
        anchors.fill: parent
        color: "#333333"
        focus: true
        Keys.onPressed: {
            if (event.key == Qt.Key_Space) {
                event.accepted = true;
                if(app.whistleListener.currentFrequency) {
                    keyTimer.characterIndex++;
                    keyTimer.restart();
                }
            }
        }
    }

    Rectangle {
        id: currentIndex
        anchors.top: parent.top
        anchors.topMargin: 200
        anchors.right: parent.right
        anchors.rightMargin: 50
        width: 100
        height: 50

        Text {
            anchors.centerIn: parent
            font.pixelSize: 14
            text: "Index: " + keyTimer.characterIndex;
        }
    }

    TextField {
        id: currentText
        anchors.top: parent.top
        anchors.topMargin: 200
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: TextInput.AlignHCenter
        width: 500
        height: 50
        font.pixelSize: 20
    }

//    Rectangle {
//        anchors.top: parent.top
//        anchors.topMargin: 75
//        anchors.horizontalCenter: parent.horizontalCenter

//        width: 100
//        height: 100
//        color: "#DDDDDD"

//        Text {
//            anchors.centerIn: parent
//            font.pixelSize: 20
//            text: app.whistleListener.currentFrequency ? app.whistleListener.currentFrequency.character : ""
//        }
//    }

//    Button {
//        anchors.top: parent.top
//        anchors.right: parent.right
//        text: "Do Test"
//        onClicked: app.doTest()
//    }

//    function createBuckets() {
//        var min = 5000;
//        var max = 15000;
//        var numBuckets = 16;
//        var bucketSize = (max - min) / numBuckets;
//        var buckets = [];
//        for(var i = 0; i < 16; i++) {
//            buckets[i] = {
//                bucket: i,
//                min: min + (i * bucketSize),
//                max: min + ((i + 1) * bucketSize)
//            };
//        }
//        return buckets;
//    }

//    Grid {
//        anchors.fill: parent
//        rows: 4
//        columns: 4
//        spacing: 2

//        Repeater {
//            model: createBuckets()

//            Rectangle {
//                width: 100
//                height: 100
//                color: mouseArea.pressed ? "#DDDDFF" : "#7777AA"

//                Text {
//                    anchors.centerIn: parent
//                    text: modelData.min + " - " + modelData.max
//                }

//                MouseArea {
//                    id: mouseArea
//                    anchors.fill: parent

//                }
//            }
//        }
//    }


    Rectangle {
        id: buckets
        anchors.right: parent.right
        anchors.top: parent.top
        width: parent.width
        height: parent.height
        color: "transparent"

        Repeater {
            model: app.whistleListener.frequencies

            Rectangle {
                id: frequencyRange
                width: buckets.width / app.whistleListener.frequencies.length
                height: buckets.height
                anchors.top: buckets.top
                anchors.left: buckets.left
                anchors.leftMargin: width * index
                color: "transparent"

                property var frequency: modelData

                Rectangle {
                    width: parent.width
                    height: modelData.value
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    color: "#CC77DD77"
                }

                Repeater {
                    model: modelData.characters

                    Rectangle {
                         width: parent.width
                         height: 30
                         anchors.top: parent.top
                         anchors.topMargin: height * index
                         anchors.left: parent.left
                         color: frequencyRange.frequency.isActive ? (keyTimer.characterIndex === index ? "#7777DD77" : "#CC77DD77") : "#CCDDDDDD"

                         Text {
                            anchors.centerIn: parent
                            text: modelData
                         }
                    }
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
        color: "transparent"

        Repeater {
            model: app.whistleListener.outputs

            Rectangle {
                 width: chart.width / (app.whistleListener.outputs.length)
                 height: modelData.value
                 anchors.bottom: chart.bottom
                 anchors.left: chart.left
                 anchors.leftMargin: width * index
                 color: "#CCDD7777"
            }
        }
    }

}
