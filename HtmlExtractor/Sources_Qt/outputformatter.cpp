#include "outputformatter.h"

OutputFormatter::OutputFormatter(QObject *parent) : QObject(parent)
{
    m_nStrLength = 80;
}

void OutputFormatter::format(QString& text){
    int cursor = 0;
    int lastBreak = 0;
    while(cursor < text.length()){
        if(text[cursor] == '\n'){
            lastBreak = cursor;
        }
        if(cursor > lastBreak + m_nStrLength){
            while(cursor > lastBreak){
                if(text[cursor].isSpace()){
                    break;
                }
                cursor--;
            }
            if(cursor == lastBreak){
                while(cursor < text.length()){
                    if(text[cursor].isSpace() || text[cursor] == '\n'){
                        break;
                    }
                    cursor++;
                }
            }
            if(cursor >= text.length()){
                break;
            }
            if(text[cursor] != '\n'){
                text.replace(cursor, 1, '\n');
            }
            lastBreak = cursor;
        }
        cursor++;
    }
}
