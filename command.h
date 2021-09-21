#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

class Command : public QObject
{
    Q_OBJECT

public:
    explicit Command(QObject* parent = nullptr);

    enum class Type {
        ShowImage,
        ShowText,
        Invalid,
    };

    Q_ENUM(Type)

    bool parse(const QByteArray& ba);
    bool parse(const QStringList& sl);
    static QStringList usage();
    QByteArray toByteArray() const;

public slots:
    Type type() const { return m_type; }

    QString showImagePath() const { return m_showImagePath; }

    int showTextX() const { return m_showTextX; }
    int showTextY() const { return m_showTextY; }
    int showTextFontPixelSize() const { return m_showTextFontPixelSize; }
    QString showTextFont() const { return m_showTextFont; }
    QString showTextText() const { return m_showTextText; }

private:
    QStringList parametersToStringList() const;
    bool parseShowImage(const QStringList& sl);
    bool parseShowText(const QStringList& sl);

    Type m_type = Type::Invalid;
    QString m_showImagePath;

    int m_showTextX = 0;
    int m_showTextY = 0;
    int m_showTextFontPixelSize = 0;
    QString m_showTextFont;
    QString m_showTextText;
};

#endif // COMMAND_H
