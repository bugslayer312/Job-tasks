#include "webkithtmldomparser.h"
#include <QWebPage>
#include <QWebFrame>
#include <QEventLoop>
#include <QTimer>

WebKitHtmlDomParser::WebKitHtmlDomParser(QObject *parent) : QObject(parent)
{
    m_pWebPage = new QWebPage(this);

    // теги, которые выкидываем вместе с их содержимым
    // в них нашего решения нет

    exceptTags.push_back("ASIDE");
    exceptTags.push_back("IFRAME");
    exceptTags.push_back("FORM");
    exceptTags.push_back("NAV");
    exceptTags.push_back("SVG");
    exceptTags.push_back("NOINDEX");
    exceptTags.push_back("SCRIPT");
    exceptTags.push_back("NOSCRIPT");
    exceptTags.push_back("EMBED");
    exceptTags.push_back("FIGURE");
    exceptTags.push_back("SELECT");
    exceptTags.push_back("LINK");
    exceptTags.push_back("IMG");
    exceptTags.push_back("HR");
    exceptTags.push_back("DEFS");
    exceptTags.push_back("MAP");
    exceptTags.push_back("PATH");
    exceptTags.push_back("POLYGON");
    exceptTags.push_back("POLYLINE");
    exceptTags.push_back("SYMBOL");
    exceptTags.push_back("USE");
    exceptTags.push_back("LABEL");
    exceptTags.push_back("INPUT");
    exceptTags.push_back("BUTTON");
    exceptTags.push_back("TEXT");
    exceptTags.push_back("OPTION");
    exceptTags.push_back("AREA");
    exceptTags.push_back("FIGCAPTION");
    exceptTags.push_back("STYLE");
    exceptTags.push_back("TEXTAREA");
    exceptTags.push_back("META");

    // теги, которые являются блоками, чье содержимое мы будем анализировать
    // в общем случае там будут в основном div

    blocksTags.insert("DIV");
    blocksTags.insert("SECTION");
    blocksTags.insert("ARTICLE");
    blocksTags.insert("HEADER");
    blocksTags.insert("FOOTER");
    blocksTags.insert("BODY");
    blocksTags.insert("MAIN");
}

void WebKitHtmlDomParser::setHtml(const QString& txt){
    QWebFrame* frame = m_pWebPage->mainFrame();
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(frame, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start(10000);
    frame->setHtml(txt);
    loop.exec();
    if(timer.isActive()){
        timer.stop();
    }
    else{
        // timeout
        disconnect(frame, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));
    }
    initDivMap();
}

void WebKitHtmlDomParser::calcPrimaryStat(){
    // получаем для каждого узла вклад его содержимого а также дочерних узлов-блоков
    for(int i=0;i < allDivs.count();i++){
        obtainItemStats(allDivs[i]);
    }
    // исключить вклад дочерних узлов-блоков
    excludeChildNodeStats(-1);
    // считаем максимальный уровень тега <H1>-<H6>
    // в самом крупном должен быть заголовок статьи
    calcMaxHeadLevel();
}

void WebKitHtmlDomParser::getPrimaryStat(QVector<PrimaryStatistics>& stat){
    stat.clear();
    stat.reserve(allDivs.count());
    for(int i=0;i < allDivs.count();i++){
        stat.push_back(allDivs[i].second);
    }
}

void WebKitHtmlDomParser::toOutput(const QSet<int>& idxs){
    QString str;
    for(QSet<int>::const_iterator it = idxs.begin();it != idxs.end();it++){
        if(!str.isEmpty()){
            str += ", ";
        }
        str += QString::number(*it);
    }
    qDebug() << str;
}

QString WebKitHtmlDomParser::getPlainText(const QSet<int>& visibleNodes){
    //testDebug();
    //qDebug() << "vis nodes";
    //toOutput(visibleNodes);
    QWebElement root = rootElement();
    makeNodeVisibility(root, visibleNodes.contains(0), 0, visibleNodes);
    QWebElementCollection refs = root.findAll("a");
    for(int i=0;i < refs.count();i++){
        QWebElement el = refs[i];
        QString h = el.attribute("href");
        if(h.indexOf("://") > 0){
            el.setInnerXml(el.toInnerXml() + "[" + h + "]");
        }
    }
    return root.toPlainText();
}

bool WebKitHtmlDomParser::allTreeHasSameVisibility(int rootIdx, bool vis, const QSet<int>& visibleNodes){
    // проверяем корень дерева
    if(vis != visibleNodes.contains(rootIdx)){
        return false;
    }
    // проверяем узлы
    for(int i=rootIdx+1;i < allDivs.count();i++){
        if(allDivs[i].second.parent == rootIdx){
            if(!allTreeHasSameVisibility(i, vis, visibleNodes)){
                return false;
            }
        }
    }
    return true;
}

void WebKitHtmlDomParser::makeNodeVisibility(QWebElement curNode, bool curVis, int blockIdx, const QSet<int>& visibleNodes){
    // дочерние элементы, которые являются блоками (или родителями блоков),
    // у которых видимость (или видимость дочернего блока) отличается от видимости текущего блока
    QVector<QPair<int, QWebElement> > childs;
    for(int i=blockIdx+1;i<allDivs.count();i++){
        QPair<QWebElement, PrimaryStatistics>& p = allDivs[i];
        if(p.second.parent == blockIdx){
            if(!allTreeHasSameVisibility(i, curVis, visibleNodes)){
                QWebElement el = p.first;
                while(!el.isNull() && el.parent() != curNode){
                    el = el.parent();
                }
                bool isNew = true;
                for(int j=0;j < childs.count();j++){
                    if(childs[j].second == el){
                        isNew = false;
                        break;
                    }
                }
                if(isNew){
                    childs.push_back(QPair<int, QWebElement>(i, el));
                }
            }
        }
    }
    if(childs.isEmpty()){
        // спорных блоков нет, проставляем общую видимость блока, выходим
        if(!curVis){
            curNode.setAttribute("style", "display:none");
        }
        return;
    }
    // проставляем видимость дочерних элементов
    QWebElement el = curNode.firstChild();
    while(!el.isNull()){
        bool bDone = false;
        for(int i=0;i < childs.count();i++){
            QPair<int, QWebElement>& child = childs[i];
            if(el == child.second){
                // спорный элемент, относящийся к блоку с отличающейся видимостью от текущего
                bool subVis = curVis;
                int subBlockIdx = blockIdx;
                if(el == allDivs[child.first].first){
                    subBlockIdx = child.first;
                    subVis = visibleNodes.contains(subBlockIdx);
                }
                makeNodeVisibility(el, subVis, subBlockIdx, visibleNodes);
                bDone = true;
                break;
            }
        }
        if(!bDone){ // не спорный элемент, проставляем видимость согласно видимости блока
            if(!curVis){
                el.setAttribute("style", "display:none");
            }
        }
        el = el.nextSibling();
    }
}

void WebKitHtmlDomParser::obtainItemStats(QPair<QWebElement, PrimaryStatistics>& p){
    PrimaryStatistics& stat = p.second;
    QWebElement el = p.first;
    stat.classId = el.classes().count() ? el.classes().front().toLower() : "";
    stat.id = el.attribute("id").toLower();
    stat.tag = el.tagName().toLower();
    stat.xmlLength = el.toOuterXml().length();
    QString plain = el.toPlainText();
    stat.plainLength = plain.length();
    for(int i=0;i<plain.length();i++){
        if(plain[i] == '.' && (i == plain.length()-1 || plain[i+1].isSpace())){
            stat.dotCount++;
        }
    }
    QWebElementCollection col = el.findAll("a");
    for(int i=0;i<col.count();i++){
        stat.refLength += col[i].toPlainText().length();
    }
    col = el.findAll("p");
    stat.paragraphCount = col.count();
}

void WebKitHtmlDomParser::enumerateChildBlocks(const QWebElement& parent, int parentIdx){
    QWebElement el = parent.firstChild();
    QString tag;
    PrimaryStatistics empty;
    while(!el.isNull()){
        int pidx = parentIdx;
        if(blocksTags.contains(el.tagName().toUpper())){
            allDivs.push_back(QPair<QWebElement, PrimaryStatistics>(el, empty));
            pidx = allDivs.count()-1;
            allDivs[pidx].second.parent = parentIdx;
        }
        enumerateChildBlocks(el, pidx);
        el = el.nextSibling();
    }
}

void WebKitHtmlDomParser::excludeChildNodeStats(int idx){
    PrimaryStatistics* p = NULL;
    if(idx >= 0)
        p = &(allDivs[idx].second);
    for(int i=0;i < allDivs.count();i++){
        PrimaryStatistics& q = allDivs[i].second;
        if(q.parent == idx){
            if(p){
                p->xmlLength -= q.xmlLength;
                p->plainLength -= q.plainLength;
                p->paragraphCount -= q.paragraphCount;
                p->dotCount -= q.dotCount;
                p->refLength -= q.refLength;
            }
            excludeChildNodeStats(i);
        }
    }
}

void WebKitHtmlDomParser::calcMaxHeadLevel(){
    QWebElement root = rootElement();
    QWebElementCollection col = root.findAll("h1,h2,h3,h4,h5,h6");
    for(int i=0;i < col.count();i++){
        QWebElement el = col[i];
        int lvl = 7 - QString(el.tagName()[1]).toInt();
        QWebElement par = el.parent();
        while(!par.isNull() && par != root && !blocksTags.contains(par.tagName().toUpper())){
            par = par.parent();
        }
        if(!par.isNull() && par != root){
            for(int i=0;i < allDivs.count();i++){
                QPair<QWebElement, PrimaryStatistics>& p = allDivs[i];
                if(p.first == par){
                    if(p.second.maxHeadLevel < lvl){
                        p.second.maxHeadLevel = lvl;
                    }
                    break;
                }
            }
        }
    }
}

void WebKitHtmlDomParser::initDivMap(){
    allDivs.clear();
    QWebElement root = rootElement();
    // убираем заведомо ненужные узлы, в которых нет решения
    removeExceptNodes(root);
    // корневой блок
    allDivs.push_back(QPair<QWebElement, PrimaryStatistics>(root, PrimaryStatistics()));
    // берем узлы-блоки, проставляем родителя, устанавливаем пустые статистики
    enumerateChildBlocks(root, 0);
}

void WebKitHtmlDomParser::testDebug(){
    for(int i=0;i < allDivs.count();i++){
        qDebug() << QString::number(i) + ": id=" + allDivs[i].second.id +
                    "; parent=" + QString::number(allDivs[i].second.parent) +
                    "; parCnt=" + QString::number(allDivs[i].second.paragraphCount) +
                    "; headLvl=" + QString::number(allDivs[i].second.maxHeadLevel);
    }
}

QWebElement WebKitHtmlDomParser::rootElement(){
    // начинаем поиск с блока body
    QWebElement body = m_pWebPage->mainFrame()->findFirstElement("body");
    // но если есть блок main, то статья должна быть там
    QWebElement el = body.findFirst("main");
    if(!el.isNull())
        return el;
    return body;
}

void WebKitHtmlDomParser::removeExceptNodes(QWebElement root){
    QString tag;
    QWebElement el;
    for(int i=0;i < exceptTags.count();i++){
        tag = exceptTags[i];
        el = root.findFirst(tag);
        while(!el.isNull()){
            el.removeFromDocument();
            el = root.findFirst(tag);
        }
    }
}
