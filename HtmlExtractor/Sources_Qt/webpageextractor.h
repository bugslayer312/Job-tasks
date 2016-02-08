#ifndef WEBPAGEEXTRACTOR_H
#define WEBPAGEEXTRACTOR_H

#include <QObject>
#include "interfaces.h"

class WebPageExtractor : public QObject
{
    Q_OBJECT
private:
    IHtmlSourceProvider* m_pSourceProvider;
    IDomParser* m_pParser;
    IBlockAnalyzer* m_pAnalyzer;
    IPlainTextSaver* m_pSaver;
    ITextFormatter* m_pFormatter;

public:
    explicit WebPageExtractor(QObject *parent = 0);
    void init(IHtmlSourceProvider* provider,
              IDomParser* parser,
              IBlockAnalyzer* analyzer,
              IPlainTextSaver* saver,
              ITextFormatter* formatter);
    void run();

signals:

public slots:
};

#endif // WEBPAGEEXTRACTOR_H
