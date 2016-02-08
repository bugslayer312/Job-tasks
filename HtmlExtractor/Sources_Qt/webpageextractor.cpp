#include "webpageextractor.h"

WebPageExtractor::WebPageExtractor(QObject *parent) : QObject(parent)
{

}

void WebPageExtractor::init(IHtmlSourceProvider* provider,
                            IDomParser* parser,
                            IBlockAnalyzer* analyzer,
                            IPlainTextSaver* saver,
                            ITextFormatter* formatter)
{
    m_pSourceProvider = provider;
    m_pParser = parser;
    m_pSaver = saver;
    m_pAnalyzer = analyzer;
    m_pFormatter = formatter;
}

void WebPageExtractor::run(){
    m_pSourceProvider->doRequest(10000);
    if(!m_pSourceProvider->isRequestComplete()){
        qDebug() << "Html reques failed!";
    }
    QString html = m_pSourceProvider->getHtml();
    m_pParser->setHtml(html);
    m_pParser->calcPrimaryStat();
    QVector<PrimaryStatistics> stats;
    m_pParser->getPrimaryStat(stats);
    m_pAnalyzer->analyze(stats);
    QSet<int> visibleBlocks;
    m_pAnalyzer->getVisibleBlocks(visibleBlocks);
    QString plain = m_pParser->getPlainText(visibleBlocks);
    m_pFormatter->format(plain);
    if(!m_pSaver->saveToDevice(plain)){
        qDebug() << "Failed save to file!";
    }
}
