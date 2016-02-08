#include "analyzeroptions.h"
#include <QDir>
#include <QtXml>
#include <QDomDocument>

// AnalyzerOptions

AnalyzerOptions::AnalyzerOptions()
{
    nplainLengthRate = 10;
    nparagraphRate = 10;
    ndotRate = 10;
    nheadRate = 20;
    nblockTypeRate = 10;
    nrefDensityRate = 10;
    ntagDensityRate = 5;
    nidClassBadRate = 10;
    nsimilarIdBadRate = 10;
    nvisCriterio = 10;
}

AnalyzerOptions::AnalyzerOptions(const AnalyzerOptions& rhs){
    nplainLengthRate = rhs.nparagraphRate;
    nparagraphRate = rhs.nparagraphRate;
    ndotRate = rhs.ndotRate;
    nheadRate = rhs.nheadRate;
    nblockTypeRate = rhs.nblockTypeRate;
    nrefDensityRate = rhs.nrefDensityRate;
    ntagDensityRate = rhs.ntagDensityRate;
    nidClassBadRate = rhs.nidClassBadRate;
    nsimilarIdBadRate = rhs.nsimilarIdBadRate;
    nvisCriterio = rhs.nvisCriterio;
}

AnalyzerOptions& AnalyzerOptions::operator=(const AnalyzerOptions& rhs){
    if(&rhs == this){
        return *this;
    }
    nplainLengthRate = rhs.nparagraphRate;
    nparagraphRate = rhs.nparagraphRate;
    ndotRate = rhs.ndotRate;
    nheadRate = rhs.nheadRate;
    nblockTypeRate = rhs.nblockTypeRate;
    nrefDensityRate = rhs.nrefDensityRate;
    ntagDensityRate = rhs.ntagDensityRate;
    nidClassBadRate = rhs.nidClassBadRate;
    nsimilarIdBadRate = rhs.nsimilarIdBadRate;
    nvisCriterio = rhs.nvisCriterio;
    return *this;
}

void AnalyzerOptions::loadFromXml(QDomElement* el){
    QDomElement ch = el->firstChildElement("plainlenghtrate");
    if(!ch.isNull()){
        nplainLengthRate = ch.text().toInt();
    }
    ch = el->firstChildElement("paragraphrate");
    if(!ch.isNull()){
        nparagraphRate = ch.text().toInt();
    }
    ch = el->firstChildElement("dotrate");
    if(!ch.isNull()){
        ndotRate = ch.text().toInt();
    }
    ch = el->firstChildElement("headrate");
    if(!ch.isNull()){
        nheadRate = ch.text().toInt();
    }
    ch = el->firstChildElement("blocktyperate");
    if(!ch.isNull()){
        nblockTypeRate = ch.text().toInt();
    }
    ch = el->firstChildElement("refdensityrate");
    if(!ch.isNull()){
        nrefDensityRate = ch.text().toInt();
    }
    ch = el->firstChildElement("tagdensityrate");
    if(!ch.isNull()){
        nidClassBadRate = ch.text().toInt();
    }
    ch = el->firstChildElement("tagdensityrate");
    if(!ch.isNull()){
        nidClassBadRate = ch.text().toInt();
    }
    ch = el->firstChildElement("idclassbadrate");
    if(!ch.isNull()){
        ntagDensityRate = ch.text().toInt();
    }
    ch = el->firstChildElement("nsimilaridbadrate");
    if(!ch.isNull()){
        nsimilarIdBadRate = ch.text().toInt();
    }
    ch = el->firstChildElement("viscriterio");
    if(!ch.isNull()){
        nvisCriterio = ch.text().toInt();
    }
}

// OptionsProvider

OptionsProvider::OptionsProvider(const QString& file, QObject* parent):QObject(parent)
{
    options.insert("*", new AnalyzerOptions()); // default
    QDir dir = QDir::current();
    QFile _file(dir.absoluteFilePath(file));
    if(_file.open(QIODevice::ReadOnly)){
        QDomDocument document("options");
        if(!document.setContent(&_file)){
            _file.close();
            return;
        }
        QDomNodeList nodes = document.elementsByTagName("option");
        for(int i=0;i < nodes.count();i++){
            QDomElement el = nodes.at(i).toElement();
            QString key = el.attribute("url", "*");
            if(!options.contains(key)){
                options.insert(key, new AnalyzerOptions());
            }
            options[key]->loadFromXml(&el);
        }

        _file.close();
    }
}

 OptionsProvider::~OptionsProvider(){
     for(QMap<QString, AnalyzerOptions*>::const_iterator it = options.begin();it != options.end();it++){
         delete it.value();
     }
 }

IAnalyzerOptions* OptionsProvider::getOptions(const QString& url){
    if(options.contains(url)){
        return options[url];
    }
    return options["*"];
}
