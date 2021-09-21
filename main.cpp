#include "command.h"
#include "tcplistener.h"
#include "tcpclient.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QCommandLineParser>
#include <QDebug>

namespace {
const QString StartServerSwitch = "start-server";
const QString RunCommandSwitch = "run-command";
const QString RotationSwitch = "rotation";
const quint16 TcpPort = 5155;

const QVector<int> AllowedRotations {
    0, 1, 2, 3
};
const int DefaultRotation = 0;
}

struct Parameters {
    bool serverMode = false;
    int rotation = 0;  // 0: 0°, 1: 90°, 2: 180°, 3: 270°
    Command command;
};

void usage()
{
    qDebug().noquote() << QString("Usage: showsplash [--%1 <args>] [--%2 <args>]").arg(StartServerSwitch).arg(RunCommandSwitch);
    qDebug().noquote() << QString("    --%1: start showsplash server [--%2 value]").arg(StartServerSwitch).arg(RotationSwitch);
    qDebug().noquote() << QString("    --%1: send command to showsplash server with given arguments").arg(RunCommandSwitch);
    qDebug().noquote() << "Available commands:";
    for (const auto s : Command::usage())
        qDebug().noquote() << s;
}

bool parseParameters(const QGuiApplication& app, Parameters& parameters)
{
    QStringList allowedRotationStrings;

    std::transform(AllowedRotations.cbegin(), AllowedRotations.cend(), std::back_inserter(allowedRotationStrings), [](int allowedRotation) {
        if (allowedRotation == DefaultRotation)
            return QString("[%1]").arg(allowedRotation);
        else
            return QString::number(allowedRotation);
    });

    QCommandLineParser parser;
    QStringList applicationDescription;
    applicationDescription << "Client/server to show splashscreens";
    applicationDescription << "";
    applicationDescription << "Available commands:";
    applicationDescription << Command::usage();
    applicationDescription << "";
    applicationDescription << "Examples:";
    applicationDescription << "showsplash --start-server --rotation 3";
    applicationDescription << "showsplash --run-command showimage /home/root/image.png";

    parser.setApplicationDescription(applicationDescription.join("\n"));
    parser.addHelpOption();
    parser.addOption({StartServerSwitch, "Start in server mode"});
    parser.addOption({RunCommandSwitch, "Start in client mode (command)"});
    parser.addOption({RotationSwitch, "Set rotation (server mode only). Value will be muluplied by 90°. Allowed values:" + allowedRotationStrings.join("/"), "rotation"});
    parser.addPositionalArgument("command", "command name");
    parser.addPositionalArgument("arguments", "command arguments");

    parser.process(app);

    if (parser.isSet(StartServerSwitch) && parser.isSet(RunCommandSwitch)) {
        qDebug().noquote() << QString("%1 and %2 cannot be set at the same time").arg(StartServerSwitch).arg(RunCommandSwitch);
        return false;
    }

    if (!parser.isSet(StartServerSwitch) && !parser.isSet(RunCommandSwitch)) {
        qDebug().noquote() << QString("%1 or %2 must be enabled").arg(StartServerSwitch).arg(RunCommandSwitch);
        return false;
    }

    parameters.serverMode = parser.isSet(StartServerSwitch);

    const auto& rotationString = parser.value(RotationSwitch);

    if (!parameters.serverMode && !rotationString.isNull()) {
        qWarning().noquote() << QString("%1 can only by used when %2 is enabled").arg(RotationSwitch).arg(StartServerSwitch);
        return false;
    }

    if (parameters.serverMode) {
        bool ok = true;
        parameters.rotation = rotationString.isNull() ? 0 : rotationString.toInt(&ok);
        if (!ok || !AllowedRotations.contains(parameters.rotation)) {
            qWarning() << "Invalid rotation parameter" << rotationString;
            return false;
        }
        return true;
    }

    return parameters.command.parse(parser.positionalArguments());
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Parameters parameters;
    if (!parseParameters(app, parameters)) {
        return 1;
    }

    if (!parameters.serverMode) {
        return TcpClient::sendCommand(parameters.command, TcpPort) ? 0 : 1;
    }

    TcpListener listener;
    if (!listener.listen(TcpPort))
        return 1;

    QQmlApplicationEngine engine;

    qmlRegisterUncreatableType<TcpListener>("com.nw", 1, 0, "TcpListener", "Cannot be created from qml");
    qmlRegisterUncreatableType<Command>("com.nw", 1, 0, "Command", "Cannot be created from qml");
    qRegisterMetaType<Command*>("Command*");

    engine.rootContext()->setContextProperty("listener", &listener);
    engine.rootContext()->setContextProperty("rotationParameter", parameters.rotation);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
