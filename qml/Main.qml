import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    visible: false
    width: 320
    height: 440
    minimumWidth: 300
    minimumHeight: 360
    title: "Clipboard"
    color: "transparent"
    flags: Qt.Popup | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    onClosing: { close.accepted = false; hide() }

    function moveCurrent(delta) {
        if (list.count === 0) return
        var target = Math.max(0, Math.min(list.currentIndex + delta, list.count - 1))
        list.currentIndex = target
        list.positionViewAtIndex(target, ListView.Center)
    }
    function selectCurrent() {
        if (list.currentIndex >= 0 && list.currentIndex < list.count)
            clipboardModel.pasteItem(list.currentIndex)
    }

    onVisibleChanged: {
        if (visible) {
            search.text = ""
            clipboardModel.setFilter("")
            list.currentIndex = list.count > 0 ? 0 : -1
            list.forceActiveFocus()
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: 10
        color: "#f7f7fb"
        border.color: "#dedee8"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 7

            RowLayout {
                Layout.fillWidth: true
                spacing: 4
                Label {
                    id: titleLabel
                    text: "Clipboard"
                    font.pixelSize: 17
                    font.weight: Font.DemiBold
                    color: "#20202a"
                    Layout.fillWidth: true
                }
                ToolButton {
                    id: closeButton
                    text: "×"
                    font.pixelSize: 20
                    width: 26; height: 26
                    contentItem: Label { text: closeButton.text; color: closeButton.hovered ? "#ffffff" : "#b43d4b"; font: closeButton.font; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { radius: 8; color: closeButton.hovered ? "#d94f61" : "#f5dfe2"; border.color: "#e8b8bf" }
                    ToolTip.visible: hovered
                    ToolTip.text: "Hide Clipboard"
                    onClicked: window.hide()
                }
            }

            TextField {
                id: search
                Layout.fillWidth: true
                placeholderText: "Search clipboard"
                leftPadding: 36
                rightPadding: 12
                font.pixelSize: 13
                color: "#000000"
                onTextChanged: clipboardModel.setFilter(text)
                Keys.onPressed: (event) => {
                    if (event.key === Qt.Key_Up || event.key === Qt.Key_Down) {
                        list.forceActiveFocus()
                        window.moveCurrent(event.key === Qt.Key_Up ? -1 : 1)
                        event.accepted = true
                        return
                    }
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        window.selectCurrent()
                        event.accepted = true
                        return
                    }
                    if (event.key === Qt.Key_Escape) {
                        window.hide()
                        event.accepted = true
                        return
                    }
                }
                background: Rectangle { radius: 10; color: "#ececf3"; border.color: search.activeFocus ? "#6656d9" : "transparent" }
                Label { anchors.left: parent.left; anchors.leftMargin: 11; anchors.verticalCenter: parent.verticalCenter; text: "⌕"; font.pixelSize: 20; color: "#6d6d78" }
            }

            RowLayout {
                Layout.fillWidth: true
                Label { text: "Clipboard history"; font.pixelSize: 12; font.weight: Font.Medium; color: "#686875"; Layout.fillWidth: true }
                Button {
                    id: clearButton
                    text: "Clear all"
                    flat: true
                    font.pixelSize: 12
                    font.weight: Font.DemiBold
                    contentItem: Label { text: clearButton.text; color: "#000000"; font: clearButton.font; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { radius: 9; color: clearButton.hovered ? "#d3d3da" : "#e7e7ed"; border.color: "#c9c9d2" }
                    onClicked: confirm.open()
                }
            }

            ListView {
                id: list
                Layout.fillWidth: true; Layout.fillHeight: true
                clip: true; spacing: 6
                focus: true
                keyNavigationEnabled: false
                model: clipboardModel
                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                Keys.onPressed: (event) => {
                    if (event.modifiers & (Qt.ControlModifier | Qt.AltModifier | Qt.MetaModifier)) {
                        event.accepted = false
                        return
                    }
                    if (event.key === Qt.Key_Backspace) {
                        search.remove(search.cursorPosition - 1, 1)
                        clipboardModel.setFilter(search.text)
                        event.accepted = true
                        return
                    }
                    if (event.key === Qt.Key_Up || event.key === Qt.Key_Down) {
                        if (list.count === 0) { event.accepted = true; return }
                        window.moveCurrent(event.key === Qt.Key_Up ? -1 : 1)
                        event.accepted = true
                        return
                    }
                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        window.selectCurrent()
                        event.accepted = true
                        return
                    }
                    if (event.key === Qt.Key_Escape) {
                        window.hide()
                        event.accepted = true
                        return
                    }
                    if (event.text.length === 1 && event.text.charCodeAt(0) > 31) {
                        search.insert(search.cursorPosition, event.text)
                        clipboardModel.setFilter(search.text)
                        event.accepted = true
                    }
                }

                delegate: Rectangle {
                    required property string preview
                    required property string text
                    required property bool pinned
                    required property string timestamp
                    required property string type
                    required property url imagePath
                    required property int index
                    width: list.width; height: Math.max(imagePath.toString() !== "" ? 112 : 62, body.implicitHeight + 20)
                    radius: 10
                    color: index === list.currentIndex ? "#e4e1f7" : (hover.containsMouse ? "#f3f1fa" : "#ffffff")
                    border.color: index === list.currentIndex ? "#b5ade3" : (hover.containsMouse ? "#d6d1ea" : "#e7e7ed")
                    Behavior on color { ColorAnimation { duration: 100 } }
                    MouseArea { id: hover; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onPressed: console.log("ROW_PRESS index=" + index); onClicked: { console.log("ROW_CLICK index=" + index); list.currentIndex = index; clipboardModel.pasteItem(index) } }
                    Rectangle {
                        visible: index === list.currentIndex
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: 3
                        radius: 2
                        color: "#6656d9"
                    }
                    ColumnLayout { id: body; anchors.left: parent.left; anchors.right: actions.left; anchors.leftMargin: 12; anchors.rightMargin: 6; anchors.verticalCenter: parent.verticalCenter; spacing: 3
                        Image {
                            id: thumb
                            visible: imagePath.toString() !== ""
                            source: imagePath
                            Layout.preferredWidth: 88
                            Layout.preferredHeight: 88
                            Layout.alignment: Qt.AlignLeft
                            fillMode: Image.PreserveAspectFit
                            clip: true
                            Rectangle { anchors.fill: parent; color: "transparent"; border.color: "#dcdce4"; radius: 5 }
                        }
                        Label { visible: imagePath.toString() === ""; text: preview; color: "#262631"; font.pixelSize: 13; elide: Text.ElideRight; Layout.fillWidth: true; maximumLineCount: 2; wrapMode: Text.Wrap }
                        Label { text: (imagePath.toString() !== "" ? "🖼  " : "") + type + "  ·  " + timestamp; color: "#8a8995"; font.pixelSize: 11 }
                    }
                    Row { id: actions; anchors.right: parent.right; anchors.rightMargin: 6; anchors.verticalCenter: parent.verticalCenter; spacing: 0
                        ToolButton {
                            id: favBtn
                            text: pinned ? "★" : "☆"
                            font.pixelSize: 14
                            width: 30; height: 30; padding: 0
                            contentItem: Label { text: favBtn.text; color: pinned ? "#000000" : "#5f5f6b"; font: favBtn.font; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                            background: Rectangle { radius: 8; color: favBtn.hovered ? "#f0f0f4" : "#ffffff"; border.color: pinned ? "#33334a" : "#d2d2dc"; border.width: 1 }
                            ToolTip.visible: hovered
                            ToolTip.text: pinned ? "Unpin" : "Pin"
                            onClicked: clipboardModel.togglePinned(index)
                        }
                    }
                }
                Label { anchors.centerIn: parent; visible: list.count === 0; text: "Your copied items will appear here"; color: "#8b8a96"; font.pixelSize: 14 }
            }
            Label { text: "Ctrl + Alt + V to open  ·  Enter to paste  ·  ↑ ↓ to navigate"; color: "#9998a5"; font.pixelSize: 10; Layout.alignment: Qt.AlignHCenter }
        }
    }
    Shortcut { sequence: "Escape"; onActivated: window.hide() }
    Dialog { id: confirm; title: "Clear clipboard history?"; modal: true; focus: true; standardButtons: Dialog.Ok | Dialog.Cancel; anchors.centerIn: parent; Label { text: "Pinned items will remain." } onAccepted: clipboardModel.clearUnpinned() }
    Component.onCompleted: { }
}