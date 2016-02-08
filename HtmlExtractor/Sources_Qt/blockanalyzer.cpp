#include "blockanalyzer.h"
#include <QtMath>
#include <QDir>
#include <QFile>

BlockAnalyzer::BlockAnalyzer(QObject *parent) : QObject(parent)
{
    blockTagsLowRate.insert("footer");
    blockTagsLowRate.insert("header");

    blockTagsHighRate.insert("article");

    idOrClassLowRateMarkers.push_back("banner");
    idOrClassLowRateMarkers.push_back("comment");

    plainLengthRate = 10;
    paragraphRate = 10;
    dotRate = 10;
    headRate = 20;
    blockTypeRate = 10;
    refDensityRate = 10;
    tagDensityRate = 5;
    idClassBadRate = 10;
    similarIdBadRate = 10;
    visCriterio = 10;
}

void BlockAnalyzer::analyze(const QVector<PrimaryStatistics>& stat){
    data.clear();
    data.reserve(stat.count());
    for(int i=0;i < stat.count();i++){
        data.push_back(ExtendedStatistics(stat[i]));
    }
    calcExtendedStats();
    applyPlainLengthRate(plainLengthRate);
    applyParagraphRate(paragraphRate);
    applyDotRate(dotRate);
    applyHeadRate(headRate);
    applyBlockTypeRate(blockTypeRate);
    applyRefDensityRate(refDensityRate);
    applyTagDensityRate(tagDensityRate);
    applyIdClassBadRate(idClassBadRate);
    // applySimilarIdBadRate(similarIdBadRate); testing...
}

void BlockAnalyzer::getVisibleBlocks(QSet<int>& visIdxs){
    for(int i=0;i<data.count();i++){
        if(data[i].visibility > visCriterio){
            visIdxs.insert(i);
        }
    }
}

void BlockAnalyzer::setOptions(IAnalyzerOptions* options){
    plainLengthRate = options->plainLengthRate();
    paragraphRate = options->paragraphRate();
    dotRate = options->dotRate();
    headRate = options->headRate();
    blockTypeRate = options->blockTypeRate();
    refDensityRate = options->refDensityRate();
    tagDensityRate = options->tagDensityRate();
    idClassBadRate = options->idClassBadRate();
    similarIdBadRate = options->similarIdBadRate();
    visCriterio = options->visibleCriterio();
}

void BlockAnalyzer::calcExtendedStats(){
    for(int i=0;i < data.count();i++){
        ExtendedStatistics& stat = data[i];
        stat.tagDensity = stat.xmlLength ? (float)stat.plainLength/stat.xmlLength : 0.0f;
        stat.refDensity = stat.plainLength ? (float)stat.refLength/stat.plainLength : 1.0f;
    }
}

void BlockAnalyzer::applyPlainLengthRate(int maxRate){
    // val => cost
    // 0..10 => -0.2*maxRate .. 0
    // 10..100 => 0..0.8*maxRate
    // 100..max => 0.8..1.0*maxRate
    if(maxRate <= 0)
        return;
    quint16 max = 0;
    for(int i=0;i < data.count();i++){
        quint16 val = data[i].plainLength;
        if(val > max){
            max = val;
        }
    }
    double k1 = 0.2/10;
    double k2 = 0.8/(100 - 10);
    double k3 = max > 100 ? (1.0 - 0.8)/(max - 100) : 0;
    for(int i=0;i < data.count();i++){
        ExtendedStatistics& stat = data[i];
        int vis = 0;
        if(stat.plainLength <= 10){
            vis = (int)ceil((-0.2 + k1*stat.plainLength)*maxRate);
        }
        else if(stat.plainLength <= 100){
            vis = (int)ceil((k2*(stat.plainLength - 10))*maxRate);
        }
        else{
            vis = (int)ceil((0.8 + k3*(stat.plainLength - 100))*maxRate);
        }
        stat.visibility += vis;
    }
}

void BlockAnalyzer::applyParagraphRate(int maxRate){
    // val => cost
    // 0 => -0.1*maxRate
    // 1 => 0.1*maxRate
    // 2 => 0.5*maxRate
    // >2 => maxRate
    if(maxRate <= 0)
        return;
    int minRate = -(int)ceil(0.1f*maxRate);
    int rate1 = (int)ceil(0.1f*maxRate);
    int rate2 = (int)ceil(0.5f*maxRate);
    for(int i=0;i < data.count();i++){
        ExtendedStatistics& stat = data[i];
        stat.visibility += stat.paragraphCount <= 0 ? minRate :
                           stat.paragraphCount <= 2 ? (stat.paragraphCount < 2 ? rate1 : rate2) :
                           maxRate;
    }
}

void BlockAnalyzer::applyDotRate(int maxRate){
    // val => cost
    // 0 => -0.2*maxRate
    // 1..5 => 0.1*maxRate..0.5*maxRate
    // >5 => 0.5*maxRate..maxRate
    if(maxRate <= 0)
        return;
    quint16 max = 0;
    for(int i=0;i < data.count();i++){
        int val = data[i].dotCount;
        if(val > max){
            max = val;
        }
    }
    int minRate = -(int)ceil(0.2f*maxRate);
    double k1 = 0.1; // k1 = (0.5 - 0.0)/(5 - 0)
    double b1 = 0;
    double k2 = max > 5 ? (1.0 - 0.5)/(max - 5) : 0;
    double b2 = 0.5;
    for(int i=0;i < data.count();i++){
        ExtendedStatistics& stat = data[i];
        int val = minRate;
        if(stat.dotCount > 0){
            if(stat.dotCount > 5){
                val = (int)ceil((b2 + k2*(stat.dotCount - 5))*maxRate);
            }
            else{
                val = (int)ceil(b1 + k1*stat.dotCount*maxRate);
            }
        }
        stat.visibility += val;
    }
}

void BlockAnalyzer::applyRefDensityRate(int maxRate){
    // val => cost
    // 0..0.1 => 0
    // 0.1 .. 1.0 => 0.5 .. 1.0 * maxRate
    if(maxRate <= 0)
        return;
    double k = (1.0 - 0.5)/(1.0 - 0.1);
    double b = 0.5;
    for(int i=0;i < data.count();i++){
        ExtendedStatistics& stat = data[i];
        if(stat.refDensity > 0.1){
            stat.visibility -= (int)ceil((b + k*(stat.refDensity - 0.1))*maxRate);
        }
    }
}

void BlockAnalyzer::applyTagDensityRate(int maxRate){
    // val => cost
    // 0 .. 0.6 => -maxRate .. 0
    // 0.6 .. 1.0 => 0
    if(maxRate <= 0)
        return;
    for(int i=0;i < data.count();i++){
        ExtendedStatistics& stat = data[i];
        if(stat.tagDensity < 0.6f){
            stat.visibility -= (int)ceil((0.6f - stat.tagDensity)*maxRate);
        }
    }
}

void BlockAnalyzer::applyHeadRate(int maxRate){
    // val => cost
    // max => maxRate
    // 0 .. max-1 => 0 .. 0.5*maxRate
    if(maxRate <= 0)
        return;
    int max = 0;
    for(int i=0;i < data.count();i++){
        int val = data[i].maxHeadLevel;
        if(val > max){
            max = val;
        }
    }
    double k = max > 1 ? (0.5 - 0.0)/(max-1 - 0) : 0;
    double b = 0;
    for(int i=0;i < data.count();i++){
        ExtendedStatistics& stat = data[i];
        if(stat.maxHeadLevel > 0){
            if(stat.maxHeadLevel == max){
                stat.visibility += maxRate;
            }
            else{
                stat.visibility += (int)ceil((b + k*stat.maxHeadLevel)*maxRate);
            }
        }
    }
}

void BlockAnalyzer::applyBlockTypeRate(int maxRate){
    if(maxRate <= 0)
        return;
    for(int i=0;i<data.count();i++){
        ExtendedStatistics& stat = data[i];
        if(blockTagsLowRate.contains(stat.tag)){
            changeVisibilityWithChilds(i, -maxRate);
        }
        else if(blockTagsHighRate.contains(stat.tag)){
            changeVisibilityWithChilds(i, maxRate/2);
        }
    }
}

void BlockAnalyzer::changeVisibilityWithChilds(int idx, int visDelta){
    ExtendedStatistics& stat = data[idx];
    stat.visibility += visDelta;
    for(int i=idx+1;i<data.count();i++){
        if(data[i].parent == idx){
            changeVisibilityWithChilds(i, visDelta);
        }
    }
}

void BlockAnalyzer::applyIdClassBadRate(int maxRate){
    if(maxRate <= 0)
        return;
    for(int i=0;i<data.count();i++){
        ExtendedStatistics& stat = data[i];
        for(int j=0;j < idOrClassLowRateMarkers.count();j++){
            QString& marker = idOrClassLowRateMarkers[j];
            if(stat.id.contains(marker) || stat.classId.contains(marker)){
                changeVisibilityWithChilds(i, -maxRate);
                break;
            }
        }
    }
}

void BlockAnalyzer::applySimilarIdBadRate(int maxRate){
    if(maxRate <= 0)
        return;
    QStringList ids;
    for(int i=0;i < data.count();i++){
        ExtendedStatistics& stat = data[i];
        if(stat.id.length() > 4){
            ids.append(stat.id);
        }
    }
    applySimilarIdBadRateSub(ids, 4, maxRate);
}

void BlockAnalyzer::applySimilarIdBadRateSub(const QStringList& strings, int startLength, int maxRate){
    QMap<QString, QStringList> groups;
    for(int i=0;i < strings.count();i++){
        QString s = strings[i];
        if(s.length() < startLength){
            continue;
        }
        s = s.left(startLength);
        for(int j=0;j < strings.count();j++){
            if(i == j){
                continue;
            }
            QString s1 = strings[j];
            if(s1.startsWith(s)){
                if(!groups.contains(s)){
                    groups.insert(s, QStringList(strings[i]));
                }
                groups[s].append(s1);
            }
        }
    }
    for(QMap<QString, QStringList>::const_iterator it = groups.begin();it != groups.end();it++){
        const QStringList& group = it.value();
        for(int i=0;i < data.count();i++){
            if(group.contains(data[i].id)){
                changeVisibilityWithChilds(i, -maxRate);
            }
        }
        applySimilarIdBadRateSub(group, startLength+1, maxRate);
    }
}
