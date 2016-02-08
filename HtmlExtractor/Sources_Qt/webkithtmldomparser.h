#ifndef WEBKITHTMLDOMPARSER_H
#define WEBKITHTMLDOMPARSER_H

#include <QObject>
#include <QWebElement>
#include "interfaces.h"

class QWebPage;

class WebKitHtmlDomParser : public QObject, public IDomParser
{
    Q_OBJECT
    Q_INTERFACES(IDomParser)

private:
    QWebPage* m_pWebPage;
    QVector<QPair<QWebElement, PrimaryStatistics> > allDivs;
    QVector<QString> exceptTags;
    QSet<QString> blocksTags;

    void obtainItemStats(QPair<QWebElement, PrimaryStatistics>& p);
    void initDivMap();
    void removeExceptNodes(QWebElement root);
    QWebElement rootElement();
    void enumerateChildBlocks(const QWebElement& parent, int parentIdx);
    void excludeChildNodeStats(int idx);
    void calcMaxHeadLevel();
    void makeNodeVisibility(QWebElement curNode, bool curVis, int blockIdx, const QSet<int>& visibleNodes);
    bool allTreeHasSameVisibility(int rootIdx, bool vis, const QSet<int>& visibleNodes);
    void testDebug();
    void toOutput(const QSet<int>& idxs);

public:
    explicit WebKitHtmlDomParser(QObject *parent = 0);

    void setHtml(const QString& txt);
    void calcPrimaryStat();
    void getPrimaryStat(QVector<PrimaryStatistics>& stat);
    QString getPlainText(const QSet<int>& visibleNodes);


signals:

public slots:
};

#endif // WEBKITHTMLDOMPARSER_H
