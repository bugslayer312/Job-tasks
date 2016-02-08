#ifndef HTMLSOURCEPROVIDER_H
#define HTMLSOURCEPROVIDER_H

#include <QObject>
#include "interfaces.h"

class QNetworkReply;
class QNetworkAccessManager;

class HtmlSourceProviderFromWeb : public QObject, public IHtmlSourceProvider
{
    Q_OBJECT
    Q_INTERFACES(IHtmlSourceProvider)

private:
    bool m_bComplete;
    QString m_txtUrl;
    QString m_Html;
    QNetworkAccessManager* m_pNam;
public:
    explicit HtmlSourceProviderFromWeb(const QString& url, QObject *parent = 0);

    // IHtmlSourceProvider
    void doRequest(int msec);
    bool isRequestComplete();
    QString getHtml();


signals:

public slots:
};

#endif // HTMLSOURCEPROVIDER_H
