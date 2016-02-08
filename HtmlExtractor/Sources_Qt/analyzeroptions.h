#ifndef ANALYZEROPTIONS_H
#define ANALYZEROPTIONS_H

#include <QObject>
#include "interfaces.h"

class QDomElement;

class AnalyzerOptions : public IAnalyzerOptions
{
public:
    int nplainLengthRate;
    int nparagraphRate;
    int ndotRate;
    int nheadRate;
    int nblockTypeRate;
    int nrefDensityRate;
    int ntagDensityRate;
    int nidClassBadRate;
    int nsimilarIdBadRate;
    int nvisCriterio;

public:
    explicit AnalyzerOptions();
    AnalyzerOptions(const AnalyzerOptions& rhs);
    AnalyzerOptions& operator=(const AnalyzerOptions& rhs);

    int plainLengthRate(){return nplainLengthRate;};
    int paragraphRate(){return nparagraphRate;};
    int dotRate(){return ndotRate;};
    int headRate(){return nheadRate;};
    int blockTypeRate(){return nblockTypeRate;};
    int refDensityRate(){return nrefDensityRate;};
    int tagDensityRate(){return ntagDensityRate;};
    int idClassBadRate(){return nidClassBadRate;};
    int similarIdBadRate(){return nsimilarIdBadRate;};
    int visibleCriterio(){return nvisCriterio;};

    void loadFromXml(QDomElement* el);

signals:

public slots:
};

class OptionsProvider : public QObject
{
    Q_OBJECT
private:
    QMap<QString, AnalyzerOptions*> options;
public:
    OptionsProvider(const QString& file, QObject* parent = 0);
    virtual ~OptionsProvider();

    IAnalyzerOptions* getOptions(const QString& url);
};

#endif // ANALYZEROPTIONS_H
