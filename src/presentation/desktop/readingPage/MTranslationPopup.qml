import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import CustomComponents
import Librum.style
import Librum.icons
import Librum.controllers
import Librum.fonts

Popup {
    id: root
    property string translation
    property string input

    implicitWidth: 560
    height: layout.height + 58

    padding: 32
    bottomPadding: 28
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle {
        color: Style.colorPopupBackground
        radius: 6
        border.width: 1
        border.color: Style.colorContainerBorder
    }

    onOpened: {
        root.translation = ""
        internal.sendTranslationRequest()

        let x = AppInfoController.lastTranslationLanguage
        print("AAA: " + x)

        targetLanguageComboBox.setDefaultItem("a", "code")
    }

    onClosed: {
        root.translation = ""
        root.input = ""

        errorItem.visible = false
    }

    ListModel {
        id: langModel

        ListElement {
            code: ""
            text: qsTr("(Detect)")
        }
        ListElement {
            code: "AR"
            text: "Arabic"
        }
        ListElement {
            code: "BG"
            text: "Bulgarian"
        }
        ListElement {
            code: "CS"
            text: "Czech"
        }
        ListElement {
            code: "DA"
            text: "Danish"
        }
        ListElement {
            code: "DE"
            text: "German"
        }
        ListElement {
            code: "EL"
            text: "Greek"
        }
        ListElement {
            code: "EN"
            text: "English"
        }
        ListElement {
            code: "ES"
            text: "Spanish"
        }
        ListElement {
            code: "ET"
            text: "Estonian"
        }
        ListElement {
            code: "FI"
            text: "Finnish"
        }
        ListElement {
            code: "FR"
            text: "French"
        }
        ListElement {
            code: "HU"
            text: "Hungarian"
        }
        ListElement {
            code: "ID"
            text: "Indonesian"
        }
        ListElement {
            code: "IT"
            text: "Italian"
        }
        ListElement {
            code: "JA"
            text: "Japanese"
        }
        ListElement {
            code: "KO"
            text: "Korean"
        }
        ListElement {
            code: "LT"
            text: "Lithuanian"
        }
        ListElement {
            code: "LV"
            text: "Latvian"
        }
        ListElement {
            code: "NB"
            text: "Norwegian"
        }
        ListElement {
            code: "NL"
            text: "Dutch"
        }
        ListElement {
            code: "PL"
            text: "Polish"
        }
        ListElement {
            code: "PT"
            text: "Portuguese"
        }
        ListElement {
            code: "RO"
            text: "Romanian"
        }
        ListElement {
            code: "RU"
            text: "Russian"
        }
        ListElement {
            code: "SK"
            text: "Slovak"
        }
        ListElement {
            code: "SL"
            text: "Slovenian"
        }
        ListElement {
            code: "SV"
            text: "Swedish"
        }
        ListElement {
            code: "TR"
            text: "Turkish"
        }
        ListElement {
            code: "UK"
            text: "Ukrainian"
        }
        ListElement {
            code: "ZH"
            text: "Chinese"
        }
    }

    Connections {
        target: AiToolsController

        function onTranslationReady(text) {
            root.translation = text
            loadingAnimation.visible = false
        }

        function onTranslationRequestTooLong() {
            loadingAnimation.visible = false
        }

        function onTranslationLimitReached() {
            loadingAnimation.visible = false
        }

        function onTranslationErrorOccured() {
            loadingAnimation.visible = false
        }

        // function onExplanationLimitReached() {
        //     errorItem.visible = true
        //     //: Make sure that the words make a valid sentence
        //     errorText.text = qsTr('You have reached your daily limit.') + ' '
        //             + ' <a href="update" style="color: ' + Style.colorBasePurple
        //             + '; text-decoration: none;">' + qsTr(
        //                 'Upgrade') + '</a> ' + qsTr('to continue.')
        // }

        // function onExplanationRequestTooLong() {
        //     errorItem.visible = true
        //     errorText.text = qsTr(
        //                 'Oops! The text is too long. Please shorten your selection.')
        // }
    }

    ColumnLayout {
        id: layout
        width: parent.width

        MButton {
            id: closeButton
            Layout.preferredHeight: 32
            Layout.preferredWidth: 32
            Layout.topMargin: -20
            Layout.rightMargin: -18
            Layout.alignment: Qt.AlignRight
            backgroundColor: "transparent"
            opacityOnPressed: 0.7
            borderColor: "transparent"
            radius: 6
            borderColorOnPressed: Style.colorButtonBorder
            imagePath: Icons.closePopup
            imageSize: 14

            onClicked: root.close()
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            Layout.topMargin: 10

            MComboBox {
                id: sourceLanguageComboBox
                Layout.preferredWidth: parent.width * 0.45
                Layout.preferredHeight: 40
                itemHeight: 32
                selectedItemFontColor: Style.colorBaseInputText
                fontSize: Fonts.size11
                selectedItemFontSize: Fonts.size11dot5
                selectedItemPadding: 3
                defaultIndex: 0
                dropdownIconSize: 10
                contentPropertyName: "text"
                checkBoxStyle: false

                model: langModel
                onItemChanged: {
                    sourceLanguageComboBox.closePopup()
                    internal.sendTranslationRequest()
                }
            }

            Item {
                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                Image {
                    id: isOfflineIndicator
                    anchors.centerIn: parent
                    sourceSize.width: 20
                    fillMode: Image.PreserveAspectFit
                    source: Icons.repeat
                }
            }

            MComboBox {
                id: targetLanguageComboBox
                Layout.preferredWidth: parent.width * 0.45
                Layout.preferredHeight: 40
                itemHeight: 32
                selectedItemFontColor: Style.colorBaseInputText
                fontSize: Fonts.size11
                selectedItemFontSize: Fonts.size11dot5
                selectedItemPadding: 3
                defaultIndex: 0
                dropdownIconSize: 10
                contentPropertyName: "text"
                checkBoxStyle: false

                model: langModel
                onItemChanged: {
                    targetLanguageComboBox.closePopup()
                    internal.sendTranslationRequest()
                }
            }
        }

        Pane {
            id: inputContainer
            Layout.fillWidth: true
            Layout.preferredHeight: inputFlick.height
            Layout.topMargin: 12
            rightPadding: 16
            clip: true
            background: Rectangle {
                color: Style.colorContainerBackground
                radius: 6
                border.width: 1
                border.color: Style.colorContainerBorder
            }

            Flickable {
                id: inputFlick
                height: Math.min(Math.max(inputField.implicitHeight,
                                          internal.minTextHeight),
                                 internal.maxTextHeight)
                width: parent.width
                contentHeight: inputField.contentHeight
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds

                TextEdit {
                    id: inputField
                    width: inputFlick.width
                    focus: true
                    text: root.input
                    font.pointSize: Fonts.size12
                    color: Style.colorText
                    readOnly: true
                    wrapMode: Text.WordWrap
                    selectionColor: Style.colorTextSelection
                }
            }

            ScrollBar {
                id: inputScrollbar
                width: pressed ? 14 : 12
                hoverEnabled: true
                active: true
                policy: ScrollBar.AlwaysOff
                visible: inputFlick.contentHeight > inputFlick.height
                orientation: Qt.Vertical
                size: inputFlick.height / inputFlick.contentHeight
                minimumSize: 0.04
                position: (inputFlick.contentY - inputFlick.originY) / inputFlick.contentHeight
                onPositionChanged: if (pressed)
                                       inputFlick.contentY = position
                                               * inputFlick.contentHeight + inputFlick.originY
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.rightMargin: -12
                anchors.bottom: parent.bottom
                horizontalPadding: 4

                contentItem: Rectangle {
                    color: Style.colorScrollBarHandle
                    opacity: inputScrollbar.pressed ? 0.8 : 1
                    radius: 4
                }

                background: Rectangle {
                    implicitWidth: 26
                    implicitHeight: 200
                    color: "transparent"
                }
            }
        }

        Pane {
            id: translationContainer
            Layout.fillWidth: true
            Layout.preferredHeight: translationFlick.height
            Layout.topMargin: 6
            rightPadding: 16
            clip: true
            background: Rectangle {
                color: Style.colorContainerBackground
                radius: 6
                border.width: 1
                border.color: Style.colorContainerBorder
            }

            MSpinner {
                id: loadingAnimation
                anchors.centerIn: parent
                visible: false
                arcColor: Style.colorBasePurple
                width: 42
                height: 42
                arcWidth: 4
            }

            Flickable {
                id: translationFlick
                height: Math.min(Math.max(translationField.implicitHeight,
                                          internal.minTextHeight),
                                 internal.maxTextHeight)
                width: parent.width
                contentHeight: translationField.implicitHeight
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds

                TextEdit {
                    id: translationField
                    width: translationFlick.width
                    focus: true
                    text: root.translation
                    font.pointSize: Fonts.size12
                    bottomPadding: 20
                    color: Style.colorText
                    readOnly: true
                    wrapMode: Text.WordWrap
                    selectionColor: Style.colorTextSelection
                }
            }

            Item {
                id: errorItem
                visible: false
                anchors.fill: parent

                ColumnLayout {
                    width: parent.width
                    anchors.centerIn: parent

                    Image {
                        id: errorIllustration
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: -30
                        source: Icons.attentionPurple
                        sourceSize.width: 270
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        id: errorText
                        Layout.alignment: Qt.AlignHCenter
                        textFormat: Text.RichText
                        text: ""
                        color: Style.colorTitle
                        font.weight: Font.Medium
                        font.pointSize: Fonts.size14
                        onLinkActivated: Qt.openUrlExternally(
                                             AppInfoController.website + "/pricing")

                        // Switch to the proper cursor when hovering above the link
                        MouseArea {
                            id: mouseArea
                            acceptedButtons: Qt.NoButton // Don't eat the mouse clicks
                            anchors.fill: parent
                            cursorShape: errorText.hoveredLink
                                         !== "" ? Qt.PointingHandCursor : Qt.ArrowCursor
                        }
                    }
                }
            }

            ScrollBar {
                id: verticalScrollbar
                width: pressed ? 14 : 12
                hoverEnabled: true
                active: true
                policy: ScrollBar.AlwaysOff
                visible: translationFlick.contentHeight > translationFlick.height
                orientation: Qt.Vertical
                size: translationFlick.height / translationFlick.contentHeight
                minimumSize: 0.04
                position: (translationFlick.contentY - translationFlick.originY)
                          / translationFlick.contentHeight
                onPositionChanged: if (pressed)
                                       translationFlick.contentY = position
                                               * translationFlick.contentHeight
                                               + translationFlick.originY
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.rightMargin: -12
                anchors.bottom: parent.bottom
                horizontalPadding: 4

                contentItem: Rectangle {
                    color: Style.colorScrollBarHandle
                    opacity: verticalScrollbar.pressed ? 0.8 : 1
                    radius: 4
                }

                background: Rectangle {
                    implicitWidth: 26
                    implicitHeight: 200
                    color: "transparent"
                }
            }
        }

        MButton {
            id: applyButton
            Layout.preferredWidth: 124
            Layout.preferredHeight: 38
            Layout.topMargin: 8
            Layout.alignment: Qt.AlignLeft
            borderWidth: 0
            backgroundColor: Style.colorBasePurple
            text: qsTr("Translate")
            textColor: Style.colorFocusedButtonText
            fontWeight: Font.Medium
            fontSize: Fonts.size12

            onClicked: ;
        }
    }

    QtObject {
        id: internal
        property int maxTextHeight: 240
        property int minTextHeight: 120

        function sendTranslationRequest() {
            root.translation = ""
            loadingAnimation.visible = true

            AppInfoController.lastTranslationLanguage
                    = sourceLanguageComboBox.getPropertyOfSelectedItem("code")

            let sourceLang = sourceLanguageComboBox.getPropertyOfSelectedItem(
                    "code")
            let targetLang = targetLanguageComboBox.getPropertyOfSelectedItem(
                    "code")

            AiToolsController.getTranslation(root.input, sourceLang, targetLang)
        }
    }
}
