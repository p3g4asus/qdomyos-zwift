#include <QApplication>
#include <QStyleFactory>
#include <stdio.h>
#include <stdlib.h>
#ifdef Q_OS_LINUX
#ifndef Q_OS_ANDROID
#include <unistd.h> // getuid
#endif
#endif
#include <QStandardPaths>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QDir>
#include <QOperatingSystemVersion>
#include "virtualtreadmill.h"
#include "domyostreadmill.h"
#include "bluetooth.h"
#include "mainwindow.h"
#include "homeform.h"
#include "qfit.h"

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include "keepawakehelper.h"
#endif

#ifdef Q_OS_MACOS
#include "macos/lockscreen.h"
#endif

#ifdef Q_OS_IOS
#include "ios/lockscreen.h"
#endif

bool logs = true;
bool noWriteResistance = false;
bool noHeartService = true;
bool noConsole = false;
bool onlyVirtualBike = false;
bool onlyVirtualTreadmill = false;
bool testResistance = false;
bool forceQml = true;
bool miles = false;
bool bluetooth_no_reconnection = false;
bool bluetooth_relaxed = false;
bool bike_cadence_sensor = false;
bool bike_power_sensor = false;
bool battery_service = false;
bool service_changed = false;
bool bike_wheel_revs = false;
bool run_cadence_sensor = false;
QString trainProgram;
QString deviceName = "";
uint32_t pollDeviceTime = 200;
uint8_t bikeResistanceOffset = 4;
double bikeResistanceGain = 1.0;
static QString logfilename = "debug-" + QDateTime::currentDateTime().toString().replace(":", "_").replace(" ","_").replace(".","_") + ".log";
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

QCoreApplication* createApplication(int &argc, char *argv[])
{
    QSettings settings;
    bool nogui = false;

    for (int i = 1; i < argc; ++i) {
        if (!qstrcmp(argv[i], "-no-gui"))
            nogui = true;
        if (!qstrcmp(argv[i], "-qml"))
            forceQml = true;
        if (!qstrcmp(argv[i], "-miles"))
            miles = true;
        if (!qstrcmp(argv[i], "-no-console"))
            noConsole = true;
        if (!qstrcmp(argv[i], "-test-resistance"))
            testResistance = true;
        if (!qstrcmp(argv[i], "-no-log"))
            logs = false;
        if (!qstrcmp(argv[i], "-no-write-resistance"))
            noWriteResistance = true;
        if (!qstrcmp(argv[i], "-no-heart-service"))
            noHeartService = true;
        if (!qstrcmp(argv[i], "-heart-service"))
            noHeartService = false;
        if (!qstrcmp(argv[i], "-only-virtualbike"))
            onlyVirtualBike = true;
        if (!qstrcmp(argv[i], "-only-virtualtreadmill"))
            onlyVirtualTreadmill = true;
        if (!qstrcmp(argv[i], "-no-reconnection"))
            bluetooth_no_reconnection = true;
        if (!qstrcmp(argv[i], "-bluetooth_relaxed"))
            bluetooth_relaxed = true;
        if (!qstrcmp(argv[i], "-bike-cadence-sensor"))
            bike_cadence_sensor = true;
        if (!qstrcmp(argv[i], "-bike-power-sensor"))
            bike_power_sensor = true;
        if (!qstrcmp(argv[i], "-battery-service"))
            battery_service = true;
        if (!qstrcmp(argv[i], "-service-changed"))
            service_changed = true;
        if (!qstrcmp(argv[i], "-bike-wheel-revs"))
            bike_wheel_revs = true;
        if (!qstrcmp(argv[i], "-run-cadence-sensor"))
            run_cadence_sensor = true;
        if (!qstrcmp(argv[i], "-train"))
        {
            trainProgram = argv[++i];
        }
        if (!qstrcmp(argv[i], "-name"))
        {
            deviceName = argv[++i];
        }
        if (!qstrcmp(argv[i], "-poll-device-time"))
        {
            pollDeviceTime = atol(argv[++i]);
        }
        if (!qstrcmp(argv[i], "-bike-resistance-gain"))
        {
            bikeResistanceGain = atof(argv[++i]);
        }
        if (!qstrcmp(argv[i], "-bike-resistance-offset"))
        {
            bikeResistanceOffset = atoi(argv[++i]);
        }
    }

    if(nogui)
        return new QCoreApplication(argc, argv);
    else if(forceQml)
        return new QApplication(argc, argv);
    else
    {
        QApplication* a = new QApplication(argc, argv);

        a->setStyle(QStyleFactory::create("Fusion"));

        /*QFont defaultFont = QApplication::font();
        defaultFont.setPointSize(defaultFont.pointSize()+2);
        qApp->setFont(defaultFont);*/

        // modify palette to dark
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window,QColor(53,53,53));
        darkPalette.setColor(QPalette::WindowText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
        darkPalette.setColor(QPalette::Base,QColor(42,42,42));
        darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
        darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
        darkPalette.setColor(QPalette::ToolTipText,Qt::white);
        darkPalette.setColor(QPalette::Text,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
        darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
        darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
        darkPalette.setColor(QPalette::Button,QColor(53,53,53));
        darkPalette.setColor(QPalette::ButtonText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
        darkPalette.setColor(QPalette::BrightText,Qt::red);
        darkPalette.setColor(QPalette::Link,QColor(42,130,218));
        darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
        darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
        darkPalette.setColor(QPalette::HighlightedText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

        qApp->setPalette(darkPalette);

        return a;
    }
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QSettings settings;
    static bool logdebug = settings.value("log_debug", false).toBool();
#if defined (Q_OS_LINUX) // Linux OS does not read settings file for now
    if(logs == false)
#else
    if(logdebug == false)
#endif
        return;

    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    QString txt = QDateTime::currentDateTime().toString() + " " + QString::number(QDateTime::currentMSecsSinceEpoch()) + " ";
    switch (type) {
    case QtInfoMsg:
        txt += QString("Info: %1 %2 %3\n").arg(file).arg(function).arg(msg);
        break;
    case QtDebugMsg:
        txt += QString("Debug: %1 %2 %3\n").arg(file).arg(function).arg(msg);
        break;
    case QtWarningMsg:
        txt += QString("Warning: %1 %2 %3\n").arg(file).arg(function).arg(msg);
        break;
    case QtCriticalMsg:
        txt += QString("Critical: %1 %2 %3\n").arg(file).arg(function).arg(msg);
        break;
    case QtFatalMsg:
        txt += QString("Fatal: %1 %2 %3\n").arg(file).arg(function).arg(msg);
        abort();
    }

    if(logs == true || logdebug == true)
    {
        QString path = homeform::getWritableAppDir();

        // Linux log files are generated on binary location

        QFile outFile(path + logfilename);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt;

        fprintf(stderr, "%s", txt.toLocal8Bit().constData());
    }

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
}

#include <QDirIterator>
void strava_do(homeform * h) {
    QString trainingDir(homeform::getWritableAppDir() + "out_fit/");
    QDir dir(trainingDir);
    if (!dir.exists())
        dir.mkpath(".");
    QDirIterator it(homeform::getWritableAppDir() + "in_txt");
    QString fileName, filePath;
    QFileInfo fileInfo;
    QRegExp regex("session2_([0-9]{8})_([0-9]{6})_([0-9]+)\\.txt");
    QRegExp lineRegex("([0-9\\.]+)\\t([0-9\\.]+)\\t([0-9\\.]+)\\t([0-9\\.]+)\\t([0-9\\.]+)\\t([0-9\\.]+)\\t([0-9\\.]+)");
    while (it.hasNext()) {
        filePath = it.next();
        fileInfo = it.fileInfo();
        if (fileInfo.isFile() && fileInfo.completeSuffix() == "txt" && (fileName = it.fileName()).length() > 4 && regex.indexIn(fileName)>=0) {
            QList<SessionLine> session;
            QDateTime stdate = QDateTime::fromString(regex.cap(1) + " " + regex.cap(2), "yyyyMMdd HHmmss");
            QFile inputFile(filePath);
            if (inputFile.open(QIODevice::ReadOnly))
            {
                QTextStream in(&inputFile);
                SessionLine mainSL;
                while (!in.atEnd())
                {
                    QString line = in.readLine();
                    if(lineRegex.indexIn(line) >= 0) {
                        uint32_t elapsed = lineRegex.cap(1).toUInt();
                        if (elapsed != mainSL.elapsedTime && mainSL.elapsedTime != SESSIONLINE_INVALID_ELAPSED)
                            session.append(mainSL);
                        SessionLine s(
                                    lineRegex.cap(4).toDouble(),
                                    0,
                                    lineRegex.cap(2).toDouble(),
                                    lineRegex.cap(6).toUInt(),
                                    lineRegex.cap(7).toUInt(),
                                    0,0,0,
                                    lineRegex.cap(5).toUInt(),
                                    lineRegex.cap(3).toDouble(),
                                    0,elapsed, false, stdate.addSecs(elapsed));
                        mainSL += s;
                    }
                }
                session.append(mainSL);
                inputFile.close();
                qDebug() << "File"<<fileName<<" size"<<session.size();
            }
            if (session.size()) {
                qfit::save(trainingDir + fileName + ".fit", session, bluetoothdevice::BIKE);
                /*QFile f(trainingDir + fileName + ".fit");
                f.open(QFile::OpenModeFlag::ReadOnly);
                QByteArray fitfile = f.readAll();
                h->strava_upload_file(fitfile,fileName);
                f.close();
                return;*/
            }
        }
    }
}

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
#ifndef Q_OS_ANDROID
    if (getuid())
    {
        printf("Runme as root!\n");
        return -1;
    }
    else printf("%s", "OK, you are root.\n");
#endif
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    QScopedPointer<QCoreApplication> app(createApplication(argc, argv));
#else
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QScopedPointer<QApplication> app(new QApplication(argc, argv));
#endif

    app->setOrganizationName("Roberto Viola");
    app->setOrganizationDomain("robertoviola.cloud");
    app->setApplicationName("qDomyos-Zwift");

    QSettings settings;
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    if(forceQml)
#endif
    {
        bool defaultNoHeartService = !noHeartService;

        // Android 10 doesn't support multiple services for peripheral mode
        if(QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::Android, 10))
            settings.setValue("bike_heartrate_service", true);

        // some Android 6 doesn't support wake lock
        if(QOperatingSystemVersion::current() < QOperatingSystemVersion(QOperatingSystemVersion::Android, 7) && !settings.value("android_wakelock").isValid())
            settings.setValue("android_wakelock", false);

        noHeartService = settings.value("bike_heartrate_service", defaultNoHeartService).toBool();
        bikeResistanceOffset = settings.value("bike_resistance_offset", bikeResistanceOffset).toInt();
        bikeResistanceGain = settings.value("bike_resistance_gain_f", bikeResistanceGain).toDouble();
        deviceName = settings.value("filter_device", "Disabled").toString();
    }
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    else
    {
        settings.setValue("miles_unit", miles);
        settings.setValue("bluetooth_no_reconnection", bluetooth_no_reconnection);
        settings.setValue("bluetooth_relaxed", bluetooth_relaxed);
        settings.setValue("bike_cadence_sensor", bike_cadence_sensor);
        settings.setValue("bike_power_sensor", bike_power_sensor);
        settings.setValue("battery_service", battery_service);
        settings.setValue("service_changed", service_changed);
        settings.setValue("bike_wheel_revs", bike_wheel_revs);
        settings.setValue("run_cadence_sensor", run_cadence_sensor);
    }
#endif

    qInstallMessageHandler(myMessageOutput);
    qDebug() << "version " << app->applicationVersion();
    foreach(QString s, settings.allKeys())
    {
        if(!s.contains("password"))
        {
            qDebug() << s << settings.value(s);
        }
    }

#if 0 // test gpx or fit export
    QList<SessionLine> l;
    for(int i =0; i< 500; i++)
    {
        QDateTime d = QDateTime::currentDateTime();
        l.append(SessionLine(i%20,i%10,i,i%300,i%10,i%180,i%6,i%120,i,i, d));
    }
    QString path = homeform::getWritableAppDir();
    qfit::save(path + QDateTime::currentDateTime().toString().replace(":", "_") + ".fit", l, bluetoothdevice::BIKE);
    return 0;
#endif
    strava_do(0);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    if(!forceQml)
    {
        if(onlyVirtualBike)
        {
            virtualbike* V = new virtualbike(new bike(), noWriteResistance, noHeartService);
            Q_UNUSED(V)
            return app->exec();
        }
        else if(onlyVirtualTreadmill)
        {
            virtualtreadmill* V = new virtualtreadmill(new treadmill(), noHeartService);
            Q_UNUSED(V)
            return app->exec();
        }
    }
#endif

    /* test virtual echelon
     * settings.setValue("virtual_device_echelon", true);
    virtualbike* V = new virtualbike(new bike(), noWriteResistance, noHeartService);
    Q_UNUSED(V)
    return app->exec();*/
    bluetooth* bl = new bluetooth(logs, deviceName, noWriteResistance, noHeartService, pollDeviceTime, noConsole, testResistance, bikeResistanceOffset, bikeResistanceGain);

#ifdef Q_OS_IOS
#ifndef IO_UNDER_QT
    lockscreen h;
    h.request();
#endif
#endif
    
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    if(forceQml)
#endif
    {
        QQmlApplicationEngine engine;
        const QUrl url(QStringLiteral("qrc:/main.qml"));
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                         qobject_cast<QGuiApplication *>(app.data()), [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

#if defined(Q_OS_ANDROID)
        auto result = QtAndroid::checkPermission(QString("android.permission.READ_EXTERNAL_STORAGE"));
        if(result == QtAndroid::PermissionResult::Denied){
            QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.READ_EXTERNAL_STORAGE"}));
            if(resultHash["android.permission.READ_EXTERNAL_STORAGE"] == QtAndroid::PermissionResult::Denied)
                qDebug() << "READ_EXTERNAL_STORAGE denied!";
        }

        result = QtAndroid::checkPermission(QString("android.permission.ACCESS_FINE_LOCATION"));
        if(result == QtAndroid::PermissionResult::Denied){
            QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.ACCESS_FINE_LOCATION"}));
            if(resultHash["android.permission.ACCESS_FINE_LOCATION"] == QtAndroid::PermissionResult::Denied)
                qDebug() << "ACCESS_FINE_LOCATION denied!";
        }

        result = QtAndroid::checkPermission(QString("android.permission.BLUETOOTH"));
        if(result == QtAndroid::PermissionResult::Denied){
            QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.BLUETOOTH"}));
            if(resultHash["android.permission.BLUETOOTH"] == QtAndroid::PermissionResult::Denied)
                qDebug() << "BLUETOOTH denied!";
        }

        result = QtAndroid::checkPermission(QString("android.permission.BLUETOOTH_ADMIN"));
        if(result == QtAndroid::PermissionResult::Denied){
            QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.BLUETOOTH_ADMIN"}));
            if(resultHash["android.permission.BLUETOOTH_ADMIN"] == QtAndroid::PermissionResult::Denied)
                qDebug() << "BLUETOOTH_ADMIN denied!";
        }
#endif
        engine.load(url);
        homeform* h = new homeform(&engine, bl);
        QObject::connect(qobject_cast<QCoreApplication *>(app.data()), &QCoreApplication::aboutToQuit, h, &homeform::aboutToQuit);
        {
#ifdef Q_OS_ANDROID
            KeepAwakeHelper helper;
#elif defined Q_OS_MACOS
            lockScreen();
#elif defined Q_OS_IOS
#ifndef IO_UNDER_QT
            lockscreen yc;
            yc.setTimerDisabled();
#endif
#endif                        
            // screen and CPU will stay awake during this section
            // lock will be released when helper object goes out of scope
            return app->exec();
        }
#ifdef Q_OS_MACOS
        unlockScreen();
#endif
    }

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    if (qobject_cast<QApplication *>(app.data())) {
        // start GUI version...
        MainWindow* W = 0;
        if(trainProgram.isEmpty())
            W = new MainWindow(bl);
        else
            W = new MainWindow(bl, trainProgram);
        W->show();
    } else {
        // start non-GUI version...
    }
    return app->exec();
#endif
}
