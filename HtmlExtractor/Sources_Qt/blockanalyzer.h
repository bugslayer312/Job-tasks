#ifndef BLOCKANALYZER_H
#define BLOCKANALYZER_H

#include <QObject>
#include "interfaces.h"

struct ExtendedStatistics : PrimaryStatistics{
    int visibility;
    float tagDensity;
    float refDensity;

    ExtendedStatistics():PrimaryStatistics(){
        visibility = 0;
    };

    ExtendedStatistics(const PrimaryStatistics& prim){
        this->xmlLength = prim.xmlLength;
        this->plainLength = prim.plainLength;
        this->dotCount = prim.dotCount;
        this->paragraphCount = prim.paragraphCount;
        this->refLength = prim.refLength;
        this->id = prim.id;
        this->classId = prim.classId;
        this->tag = prim.tag;
        this->parent = prim.parent;
        this->maxHeadLevel = prim.maxHeadLevel;
        visibility = 0;
        refDensity = 0.0f;
        tagDensity = 0.0f;
    }
};

class BlockAnalyzer : public QObject, public IBlockAnalyzer
{
    Q_OBJECT
    Q_INTERFACES(IBlockAnalyzer)

private:
    QVector<ExtendedStatistics> data;
    QSet<QString> blockTagsLowRate;     // теги понижающие рейт узла
    QSet<QString> blockTagsHighRate;    // теги повышающие рейт узла
    QVector<QString> idOrClassLowRateMarkers;

    QString m_testIdFilter;

    int plainLengthRate;
    int paragraphRate;
    int dotRate;
    int headRate;
    int blockTypeRate;
    int refDensityRate;
    int tagDensityRate;
    int idClassBadRate;
    int similarIdBadRate;
    int visCriterio;

    void calcExtendedStats();
    // в основном повышающие тесты
    void applyPlainLengthRate(int maxRate);
    void applyParagraphRate(int maxRate);
    void applyDotRate(int maxRate);
    void applyHeadRate(int maxRate);
    // повышающие либо понижающие тесты
    void applyBlockTypeRate(int maxRate);
    // в основном понижающие тесты
    void applyRefDensityRate(int maxRate);
    void applyTagDensityRate(int maxRate);
    void applyIdClassBadRate(int maxRate);
    void applySimilarIdBadRate(int maxRate);

    void changeVisibilityWithChilds(int idx, int visDelta);
    void applySimilarIdBadRateSub(const QStringList& strings, int startLength, int maxRate);

public:
    explicit BlockAnalyzer(QObject *parent = 0);

    void analyze(const QVector<PrimaryStatistics>& stat);
    void getVisibleBlocks(QSet<int>& visIdxs);
    void setOptions(IAnalyzerOptions* options);

signals:

public slots:
};

#endif // BLOCKANALYZER_H
