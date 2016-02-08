#ifndef INTERFACES_H
#define INTERFACES_H

#include <QDebug>

struct PrimaryStatistics{       // характеристики блока (за вычетом вложенных блоков)
    quint16 xmlLength;          // длина XML
    quint16 plainLength;        // длина отображаемого текста
    quint16 dotCount;           // кол-во точек в отображаемом тесте
    quint16 paragraphCount;     // кол-во параграфов
    quint16 refLength;          // сумма длин XML от всех ссылок внутри узла
    int     maxHeadLevel;       // макс уровень тегов <H1> - <H6>, где H1 - 6, H6 - 1, отсутствие тега - 0
    int     parent;             // индекс блока-родителя
    QString id;
    QString classId;
    QString tag;

    PrimaryStatistics(){
        parent = -1;
        maxHeadLevel = 0;
        xmlLength = plainLength = dotCount = paragraphCount = refLength = 0;
    };
};

class IHtmlSourceProvider{
public:
    virtual void doRequest(int msec) = 0;
    virtual bool isRequestComplete() = 0;
    virtual QString getHtml() = 0;
};

Q_DECLARE_INTERFACE(IHtmlSourceProvider, "Task.IHtmlSourceProvider.1.0")

class IDomParser{
public:
    virtual void setHtml(const QString& txt)=0;
    virtual void calcPrimaryStat()=0;
    virtual void getPrimaryStat(QVector<PrimaryStatistics>& stat)=0;
    virtual QString getPlainText(const QSet<int>& visibleNodes)=0;
};

Q_DECLARE_INTERFACE(IDomParser, "Task.IDomParser.1.0")

class IAnalyzerOptions;

class IBlockAnalyzer{
public:
    virtual void setOptions(IAnalyzerOptions* options)=0;
    virtual void analyze(const QVector<PrimaryStatistics>& stat)=0;
    virtual void getVisibleBlocks(QSet<int>& visIdxs)=0;
};

Q_DECLARE_INTERFACE(IBlockAnalyzer, "Task.IBlockAnalyzer.1.0")

class IPlainTextSaver{
public:
    virtual bool saveToDevice(const QString& text)=0;
};

Q_DECLARE_INTERFACE(IPlainTextSaver, "Task.IPlainTextSaver.1.0")

class ITextFormatter{
public:
    virtual void format(QString& text)=0;
};

Q_DECLARE_INTERFACE(ITextFormatter, "Task.ITextFormatter.1.0")

class IAnalyzerOptions{
public:
    virtual int plainLengthRate()=0;
    virtual int paragraphRate()=0;
    virtual int dotRate()=0;
    virtual int headRate()=0;
    virtual int blockTypeRate()=0;
    virtual int refDensityRate()=0;
    virtual int tagDensityRate()=0;
    virtual int idClassBadRate()=0;
    virtual int similarIdBadRate()=0;
    virtual int visibleCriterio()=0;
};

Q_DECLARE_INTERFACE(IAnalyzerOptions, "Task.IAnalyzerOptions.1.0")

struct ITst{
    virtual void m1()=0;
};

Q_DECLARE_INTERFACE(ITst, "Task.ITst.1.0")

#endif // INTERFACES_H
