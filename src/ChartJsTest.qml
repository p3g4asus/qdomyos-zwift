import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.0
import Qt.labs.settings 1.0
import QtWebView 1.1

ColumnLayout {
    signal popupclose()
    id: column1
    spacing: 10
    anchors.fill: parent
    WebView {
        id: webView
        anchors.fill: parent
        url: "http://localhost:7666/chart/chart.htm"
        visible: true
        onLoadingChanged: {
            if (loadRequest.errorString)
                console.error(loadRequest.errorString);
        }
    }

    Button {
        id: closeButton
        height: 50
        width: parent.width
        text: "Close"
        Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
        onClicked: {
            popupclose();
        }
        anchors {
            bottom: parent.bottom
        }
    }
}
