import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Librum.elements
import Librum.style
import Librum.globals
import Librum.controllers
import "DocumentNavigation.js" as NavigationLogic


/*
  A view on the document's pages in a certain layout (e.g. vertical)
  */
Pane {
  id: root
  property var lastSelectedPage
  signal clicked

  signal zoomFactorChanged(real factor)
  property var bookController

  padding: 0
  background: Rectangle {
    color: "transparent"
  }
  // Disable pressing tab to focus other elements
  Keys.onTabPressed: event => {
                       event.accepted = true
                     }

  Connections {
    target: DictionaryController

    function onGettingDefinitionFailed() {}

    function onStartedGettingDefinition(word) {
      if (!dictionaryPopup.opened) {
        dictionaryPopup.x = Qt.binding(function () {
          return root.width / 2 - dictionaryPopup.width / 2
        })
        dictionaryPopup.y = Qt.binding(function () {
          return root.height / 2 - dictionaryPopup.height / 2
        })
        dictionaryPopup.open()
      }

      dictionaryPopup.word = word
    }
  }

  DocumentView {
    id: documentView
    height: parent.height
    anchors.centerIn: parent
    width: Math.min(root.width * 0.9, implicitWidth)
    bookController: root.bookController
  }

  MSelectionOptionsPopup {
    id: selectionOptionsPopup
    property real highlightCenterX
    property real highlightBottomY

    onNewWidth: internal.openSelectionOptionsPopup(-1, -1)

    onExplanationOptionSelected: text => {
                                   explanationPopup.question = text
                                   explanationPopup.open()
                                 }

    onTranslationOptionSelected: text => {
                                   translationPopup.input = text.replace(
                                     /\r|\n/g, '')
                                   translationPopup.open()
                                 }
  }

  MDictionaryPopup {
    id: dictionaryPopup
  }

  MExplanationPopup {
    id: explanationPopup
    x: root.width / 2 - explanationPopup.width / 2
    y: root.height / 2 - explanationPopup.height / 2
  }

  MTranslationPopup {
    id: translationPopup
    x: root.width / 2 - translationPopup.width / 2
    y: root.height / 2 - translationPopup.height / 2
  }
}
