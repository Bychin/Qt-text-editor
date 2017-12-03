#include "highlighter.h"


Highlighter::Highlighter(const QString _filename, QObject *parent) : QSyntaxHighlighter(parent) {
    filename=_filename;
    QFile file(filename);
    QStringList keywords;
    if (file.open(QIODevice::ReadOnly)) {
        HighlightFormat my_format;
        QDomDocument doc;
        doc.setContent(&file);
        QDomElement ext = doc.elementsByTagName("language").at(0).toElement();
        file_extension = QRegExp(ext.attribute("extentions"));

        QDomNodeList list = doc.elementsByTagName("keyword");
        int size = list.size();
        for (int i = 0; i < size; ++i)
            keywords.append(list.at(i).toElement().text());

        keyword_format.setForeground(Qt::darkBlue);
        keyword_format.setFontWeight(QFont::Bold);
        foreach (const QString& _pattern, keywords) {
            my_format.pattern = QRegExp(_pattern);
            my_format.format = keyword_format;
            for_keywords.append(my_format);
        }
        keywords.clear();
        list = doc.elementsByTagName("command");
        size = list.size();
        preproc_format.setForeground(Qt::darkBlue);
        for (int i = 0; i < size; ++i)
            keywords.append(list.at(i).toElement().text());

        foreach(const QString& _pattern, keywords){
            my_format.pattern = QRegExp(_pattern);
            my_format.format = preproc_format;
            for_keywords.append(my_format);
        }

        classname_format.setForeground(Qt::darkMagenta);
        classname_format.setFontWeight(QFont::Bold);
        my_format.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
        my_format.format = classname_format;
        for_keywords.append(my_format);
        multi_comment.setForeground(Qt::darkGreen);
        QDomNode beg_com = doc.elementsByTagName("multiline_comment").at(0);
        comment_beg_exp=QRegExp(beg_com.toElement().attribute("begin"));
        comment_end_exp=QRegExp(beg_com.toElement().attribute("end"));

        single_comment.setForeground(Qt::darkGreen);
        QDomNode single_com = doc.elementsByTagName("single_comment").at(0);
        my_format.pattern = QRegExp(single_com.toElement().attribute("expression"));
        my_format.format = single_comment;
        for_keywords.append(my_format);

        quotation_format.setForeground(Qt::darkGreen);
        my_format.pattern = QRegExp("\".*\"");
        my_format.format = quotation_format;
        for_keywords.append(my_format);

        function_format.setFontItalic(true);
        function_format.setForeground(Qt::blue);
        my_format.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
        my_format.format = function_format;
        for_keywords.append(my_format);
    }
}

void Highlighter::highlightBlock(const QString& text) {
    foreach (const HighlightFormat& rule_, for_keywords) {
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
        begin = comment_beg_exp.indexIn(text);

    while (begin >= 1) {
        int end = comment_end_exp.indexIn(text);
        int com_len;
        if (end == -1) {
            setCurrentBlockState(1);
            com_len = text.length() - begin + comment_end_exp.matchedLength();
        }
        else {
            com_len = end - begin + comment_end_exp.matchedLength();
        }
        setFormat(begin, com_len, multi_comment);
        begin = comment_beg_exp.indexIn(text, begin + com_len);
    }
}

bool Highlighter::getRequest(const QString _filename) {
    int ind = file_extension.indexIn(_filename);
    if (ind == -1)
        return false;
    return true;
}
