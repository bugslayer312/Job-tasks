#include <QApplication>
#include "webpageextractor.h"
#include "htmlsourceproviderfromweb.h"
#include "webkithtmldomparser.h"
#include "textsavertofile.h"
#include "blockanalyzer.h"
#include "outputformatter.h"
#include "analyzeroptions.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(argc < 2){
        qDebug() << "Command line fail! first argument must be url.";
        return 0;
    }

    QString url(argv[1]);

    qDebug() << "Starting get article from " + url;

    WebPageExtractor extractor;

    TextSaverToFile* pSaver = new TextSaverToFile(url, &extractor);
    HtmlSourceProviderFromWeb* pProvider = new HtmlSourceProviderFromWeb(url, &extractor);
    WebKitHtmlDomParser* pParser = new WebKitHtmlDomParser(&extractor);
    BlockAnalyzer* pAnalyzer = new BlockAnalyzer(&extractor);
    OptionsProvider optProv("options.xml");
    int pos = url.indexOf("://");
    if(pos >= 0){
        int fin = url.indexOf('/', pos+3);
        if(fin >= 0){
            QString startAddress = url.mid(pos+3, fin - pos - 3);
            IAnalyzerOptions* opt = optProv.getOptions(startAddress);
            pAnalyzer->setOptions(opt);
        }
    }
    OutputFormatter* pFormatter = new OutputFormatter(&extractor);

    extractor.init(pProvider, pParser, pAnalyzer, pSaver, pFormatter);
    extractor.run();

    qDebug() << "Complete!";

    return 0;
}
