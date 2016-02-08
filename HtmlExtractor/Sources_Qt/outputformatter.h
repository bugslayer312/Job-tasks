#ifndef OUTPUTFORMATTER_H
#define OUTPUTFORMATTER_H

#include <QObject>
#include "interfaces.h"

class OutputFormatter : public QObject, public ITextFormatter
{
    Q_OBJECT
    Q_INTERFACES(ITextFormatter)

private:
    int m_nStrLength;

public:
    explicit OutputFormatter(QObject *parent = 0);
    void format(QString& text);
signals:

public slots:
};

#endif // OUTPUTFORMATTER_H
