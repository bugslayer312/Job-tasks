#include "textsavertofile.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

TextSaverToFile::TextSaverToFile(const QString& url, QObject *parent) : QObject(parent)
  , m_txtUrl(url)
{
}

bool TextSaverToFile::saveToDevice(const QString& text){
    QString relpath(m_txtUrl);
    // making relative folders path according to url
    int pos = relpath.indexOf("://");
    if(pos >= 0){
        relpath.remove(0, pos+3);
    }
    QString filename("result.txt"); // name by default
    pos = relpath.lastIndexOf('/');
    while(pos == relpath.length()-1){
        relpath.chop(1);
        pos = relpath.lastIndexOf('/');
    }
    if(pos >= 0){
        filename = relpath.right(relpath.length() - pos - 1);
        relpath.remove(pos, relpath.length() - pos);
        pos = filename.lastIndexOf('.');
        if(pos >= 0){
            filename.replace(pos+1, filename.length() - pos - 1, "txt");
        }
        else{
            filename += ".txt";
        }
    }
    QDir dir = QDir::current();
    QString path = dir.absolutePath();
    if(relpath.length() > 0){
        if(dir.mkpath(relpath)){
            path += "/" + relpath;
        }
    }
    dir.setPath(path);
    QFile file(dir.absoluteFilePath(filename));
    if(file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text)){
        QTextStream stream(&file);
        stream << text;
        return true;
    }
    return false;
}
