#include "command.h"

#include <QDebug>
#include <QMap>

namespace {
QMap<Command::Type, QString> TypeMap = {
    {Command::Type::ShowImage, "showimage"},
    {Command::Type::ShowText, "showtext"},
};
const uint8_t ParameterSeparator = 0x00;
const QString FileProtocolSeparator = "://";
const QString DefaultFileProtocol = "file://";
}

Command::Command(QObject* parent)
    : QObject(parent)
{
}

bool Command::parse(const QByteArray& ba)
{
    QStringList sl;
    auto tokens = ba.split(ParameterSeparator);
    tokens.removeAll(QByteArray());
    std::transform(tokens.cbegin(), tokens.cend(), std::back_inserter(sl), [](const QByteArray& token) {
        return QString::fromLatin1(token);
    });

    qInfo() << "Parsing command" << sl;

    return parse(sl);
}

bool Command::parse(const QStringList& sl)
{
    if (sl.size() == 0) {
        qWarning() << "Missing command name";
        return false;
    }

    QString commandString = sl[0];

    m_type = TypeMap.key(commandString, Type::Invalid);

    switch (m_type) {
    case Type::ShowImage:
        return parseShowImage(sl);
    case Type::ShowText:
        return parseShowText(sl);
    default:
        qWarning() << Q_FUNC_INFO << "Unhandled type " << static_cast<int>(m_type);
        return false;
    }

    Q_UNREACHABLE();
}

QStringList Command::usage()
{
    QStringList ret;
    ret << "showimage <path>: show image at given path";
    ret << "showtext <x> <y> <font-pixel-size> <font> <text>: show text with given parameters";
    return ret;
}

QByteArray Command::toByteArray() const
{
    QStringList sl;

    sl << TypeMap[m_type];

    switch (m_type) {
    default:
        sl << parametersToStringList();
        break;
    case Type::Invalid:
        qWarning() << Q_FUNC_INFO << "Invalid type";
        return QByteArray();
    }

    QByteArray ba = sl.join(QChar(ParameterSeparator)).toLatin1();

    return ba;
}

QStringList Command::parametersToStringList() const
{
    switch (m_type) {
    case Type::ShowImage:
        return {
            m_showImagePath
        };
    case Type::ShowText:
        return {
            QString::number(m_showTextX),
            QString::number(m_showTextY),
            QString::number(m_showTextFontPixelSize),
            m_showTextFont,
            m_showTextText
        };
    default:
        return {};
    }
}

bool Command::parseShowImage(const QStringList& sl)
{
    if (sl.size() != 2) {
        qWarning() << Q_FUNC_INFO << "Invalid parameters count for command " << sl.first();
        return false;
    }

    if (sl[1].contains(FileProtocolSeparator))
        m_showImagePath = sl[1];
    else
        m_showImagePath = DefaultFileProtocol + sl[1];

    return true;
}

bool Command::parseShowText(const QStringList& sl)
{
    if (sl.size() != 6) {
        qWarning() << Q_FUNC_INFO << "Invalid parameters count for command" << sl.first();
        return false;
    }

    m_showTextX = sl[1].toInt();
    m_showTextY = sl[2].toInt();
    m_showTextFontPixelSize = sl[3].toInt();
    m_showTextFont = sl[4];
    m_showTextText = sl[5];

    return true;
}
