#include "highlighter.h"

#include <QDebug>

Highlighter::Highlighter(const QString _filename, QObject *parent) : QSyntaxHighlighter(parent) {
    xml_filename=_filename;
    QFile file(xml_filename);
    QStringList keywords;
    if (file.open(QIODevice::ReadOnly)) {
        HighlightFormat my_format;
        QDomDocument doc;
        doc.setContent(&file);
        auto languages = doc.elementsByTagName("language");
        for (int i = 0; i < languages.length(); ++i) {
            auto ext = languages.at(i).toElement();

            auto file_extension = QRegExp(ext.attribute("extentions"));

            HighlightLang data = {};

            QDomNodeList list = ext.elementsByTagName("keyword");
            int size = list.size();
            for (int i = 0; i < size; ++i){
                keywords.append(list.at(i).toElement().text());
            }


            data.keyword_format.setForeground(Qt::darkBlue);
            data.keyword_format.setFontWeight(QFont::Bold);
            foreach (const QString& _pattern, keywords) {

                    my_format.pattern = QRegExp("\\b"+_pattern+"\\b");
                    my_format.format = data.keyword_format;
                    data.for_keywords.append(my_format);
            }
            keywords.clear();
            list = ext.elementsByTagName("command");
            size = list.size();
            data.preproc_format.setForeground(Qt::darkBlue);
            for (int i = 0; i < size; ++i)
                keywords.append(list.at(i).toElement().text());

                    foreach(const QString& _pattern, keywords){
                    my_format.pattern = QRegExp(_pattern);
                    my_format.format = data.preproc_format;
                    data.for_keywords.append(my_format);
                }

            data.classname_format.setForeground(Qt::darkMagenta);
            data.classname_format.setFontWeight(QFont::Bold);
            my_format.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
            my_format.format = data.classname_format;
            data.for_keywords.append(my_format);
            data.multi_comment.setForeground(Qt::darkGreen);
            QDomNode beg_com = ext.elementsByTagName("multiline_comment").at(0);
            data.comment_beg_exp=QRegExp(beg_com.toElement().attribute("begin"));
            data.comment_end_exp=QRegExp(beg_com.toElement().attribute("end"));

            data.single_comment.setForeground(Qt::darkGreen);
            QDomNode single_com = ext.elementsByTagName("single_comment").at(0);
            my_format.pattern = QRegExp(single_com.toElement().attribute("expression"));
            my_format.format = data.single_comment;
            data.for_keywords.append(my_format);

            data.quotation_format.setForeground(Qt::darkGreen);
            my_format.pattern = QRegExp("\".*\"");
            my_format.format = data.quotation_format;
            data.for_keywords.append(my_format);

            data.function_format.setFontItalic(true);
            data.function_format.setForeground(Qt::blue);
            my_format.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
            my_format.format = data.function_format;
            data.for_keywords.append(my_format);

            langs.insert(file_extension, data);


        }
    }
}

void Highlighter::highlightBlock(const QString& text) {
    auto &data = langs.value(current_extension);


    foreach (const HighlightFormat& rule_, data.for_keywords) {
        QRegExp expression(rule_.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int len = expression.matchedLength();
            setFormat(index, len, rule_.format);
            index = expression.indexIn(text, index+len);
        }
    }
    setCurrentBlockState(0);
    int begin = 0;
    if (previousBlockState() != 1)
        begin = data.comment_beg_exp.indexIn(text);

    while (begin >= 1) {
        int end = data.comment_end_exp.indexIn(text);
        int com_len;
        if (end == -1) {
            setCurrentBlockState(1);
            com_len = text.length() - begin + data.comment_end_exp.matchedLength();
        }
        else {
            com_len = end - begin + data.comment_end_exp.matchedLength();
        }
        setFormat(begin, com_len, data.multi_comment);
        begin = data.comment_beg_exp.indexIn(text, begin + com_len);
    }
}

bool Highlighter::setExtension(const QString &_filename) {
    for (int i = 0; i < langs.size(); ++i) {
        if (langs.keys().at(i).indexIn(_filename) != -1){
            current_extension = langs.keys().at(i);
            return true;
        }
    }

    return false;
}
