import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import io.qt.Backend 1.0

Window {
    visible: true
    width: 700
    minimumWidth: 500
    height: 450
    minimumHeight: 200
    title: "Server"
    color: "#CED0D4"

    Backend {
        id: backend

        onSmbConnected: {
            ti.append(addMsg(msg + " has connected"));
            cbItems.append({text: msg})
        }
        onSmbDisconnected: {
            if(msg !=""){
                ti.append(addMsg(msg + " has disconnected"));
                cbItems.remove(comboBox.find(msg));
            }
        }
        onNewMessage: {
            ti.append(addMsg(msg));
        }
        /*
        onNewConnectionToServer: {
            ti.append(addMsg("New message: " + msg));
            cbItems.append({text: msg})
        }
*/
    }

    ColumnLayout
    {
        id: columnLayout
        anchors.fill: parent
        anchors.margins: 10

        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter

            BetterButton {
                id: btn_start
                anchors.left: parent.left
                text: "Start server"
                color: enabled ? this.down ? "#78C37F" : "#87DB8D" : "gray"
                border.color: "#78C37F"
                onClicked: {
                    ti.append(addMsg(backend.startClicked()));
                    //this.enabled = false;
                }
            }
            BetterButton {
                color: "#507cee"
                anchors.right: parent.right
                enabled: true
                text: "Connect to server"
                border.color: "#DB7A74"
                onClicked: {
                    //ti.append(addMsg(backend.stopClicked()));
                    ti.append(addMsg("Connecting"));
                    backend.connect2server();
                    //cbItems.clear();
                    //btn_start.enabled = true;
                }
            }
        }

        LayoutSection {
            Layout.fillHeight: true

            ScrollView {
                id: scrollView
                anchors.fill: parent

                TextArea {
                    id: ti
                    readOnly: true
                    selectByMouse : true
                    font.pixelSize: 14
                    wrapMode: TextInput.WrapAnywhere
                }
            }
        }

        BetterButton {
            id: btn_send3
            x: 302
            color: enabled ? this.down ? "#6FA3D2" : "#7DB7E9" : "gray"
            text: "Start"
            border.color: "#6fa3d2"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            enabled: true
            onClicked: {
                backend.set_timer();
            }
        }

        RowLayout {
            Layout.rightMargin: 15
            Layout.bottomMargin: 5
            Rectangle {
                y: 1
                height: 38
                color: "#f4f2f5"
                border.color: "#808080"
                border.width: 1
                Layout.fillWidth: true
                TextInput {
                    id: msgToSend1
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 14
                    clip: true
                    leftPadding: 10
                    rightPadding: 10
                }
            }

            BetterButton {
                id: btn_send1
                x: 453
                color: enabled ? this.down ? "#6FA3D2" : "#7DB7E9" : "gray"
                text: "Send to first"
                border.color: "#6fa3d2"
                onClicked: {
                    ti.append(addMsg("Send message to first"));
                    backend.sendToClient(0,msgToSend1.text);
                }
                //enabled: !btn_connect.enabled
            }

            BetterButton {
                id: btn_send2
                x: 453
                color: enabled ? this.down ? "#6FA3D2" : "#7DB7E9" : "gray"
                text: "Send to second"
                border.color: "#6fa3d2"
                //enabled: !btn_connect.enabled
                onClicked: {
                    ti.append(addMsg("Send message to second" ));
                    backend.sendClicked(msgToSend1.text);
                }
            }
            Layout.topMargin: 5
            Layout.leftMargin: 15
        }



    }

    Component.onCompleted: {
        ti.text = addMsg("Application started\n- - - - - -");
    }

    function addMsg(someText)
    {
        return "[" + currentTime() + "] " + someText;
    }

    function currentTime()
    {
        var now = new Date();
        var nowString = ("0" + now.getHours()).slice(-2) + ":"
                + ("0" + now.getMinutes()).slice(-2) + ":"
                + ("0" + now.getSeconds()).slice(-2) + " LT:" + backend.getLT();
        return nowString;
    }
}
