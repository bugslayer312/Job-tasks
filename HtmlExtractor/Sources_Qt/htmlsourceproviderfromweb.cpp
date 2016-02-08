#include <QtNetwork>
#include "htmlsourceproviderfromweb.h"

HtmlSourceProviderFromWeb::HtmlSourceProviderFromWeb(const QString& url, QObject *parent) : QObject(parent)
  , m_bComplete(false)
  , m_txtUrl(url)
{
    m_pNam = new QNetworkAccessManager(this);
}

void HtmlSourceProviderFromWeb::doRequest(int msec){
    m_Html.clear();
    m_bComplete = false;
    QUrl url(m_txtUrl);
    QNetworkRequest req(url);
    QNetworkReply* rep = m_pNam->get(req);
    QTimer timer;
    timer.setSingleShot(msec);
    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(rep, SIGNAL(finished()), &loop, SLOT(quit()));
    timer.start(msec);
    loop.exec();
    QString err;
    if(timer.isActive()){
        timer.stop();
        if(rep->error() > 0){
            err = "Http request error: " + rep->errorString();
        }else{
            int v = rep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if(v >= 200 && v < 300){    // success
                m_Html = rep->readAll();
                m_bComplete = true;
            }
            else{
                err = "Http request bad status code:" + QString::number(v);
            }
        }
    }else{
        // timeout
        disconnect(rep, SIGNAL(finished()), &loop, SLOT(quit()));
        rep->abort();
        err = "Http request timeout";
    }
    if(!err.isEmpty()){
        qDebug() << err;
    }
}

bool HtmlSourceProviderFromWeb::isRequestComplete(){
    return m_bComplete;
}

QString HtmlSourceProviderFromWeb::getHtml(){
    return m_Html;
}

