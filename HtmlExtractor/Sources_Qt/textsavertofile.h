#ifndef TEXTSAVERTOFILE_H
#define TEXTSAVERTOFILE_H

#include <QObject>
#include "interfaces.h"

class TextSaverToFile : public QObject, public IPlainTextSaver
{
    Q_OBJECT
    Q_INTERFACES(IPlainTextSaver)

private:
    QString m_txtUrl;

public:
    explicit TextSaverToFile(const QString& url, QObject *parent = 0);

    bool saveToDevice(const QString& text);
signals:

public slots:
};

#endif // TEXTSAVERTOFILE_H
