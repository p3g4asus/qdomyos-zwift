#ifndef POWERZONEPACK_H
#define POWERZONEPACK_H

#include <QObject>
#include <QAbstractOAuth2>

#include <QNetworkAccessManager>

#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QSettings>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QtWebSockets/QWebSocket>
#include "trainprogram.h"
#include "bluetooth.h"

class powerzonepack : public QObject {
    Q_OBJECT
  public:
    powerzonepack(bluetooth *bl, QObject *parent);
    bool searchWorkout(const QString &classid);
    QList<trainrow> trainrows;

  private:
    const int peloton_workout_second_resolution = 10;
    bool pzp_credentials_wrong = false;

    QString response;
    QWebSocket websocket;
    bluetooth* bluetoothManager = nullptr;
    QString token;
    QString lastWorkoutID = QLatin1String("");

    void startEngine();

  private slots:
    void search_workout_onfinish(const QString &message);
    void error(QAbstractSocket::SocketError error);
    void login_onfinish(const QString &message);

  signals:
    void workoutStarted(QList<trainrow> *list);
    void loginState(bool ok);
};

#endif // POWERZONEPACK_H