#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QString>
#include <QVector>
#include <QRegExp>
#include <QTextCharFormat>
#include <QDomDocument>


class Highlighter : public QSyntaxHighlighter {
public:
    Highlighter(const QString _filename, QObject *parent = 0);
    void highlightBlock(const QString &text);
    bool getRequest(const QString _filename);
private:
    struct HighlightFormat{
        QRegExp pattern;
        QTextCharFormat format;
    };
    QString filename;
    QRegExp file_extension;
    bool request;
    QVector <HighlightFormat> for_keywords;
    QRegExp comment_beg_exp;
    QRegExp comment_end_exp;
    QTextCharFormat single_comment;
    QTextCharFormat multi_comment;
    QTextCharFormat keyword_format;
    QTextCharFormat classname_format;
    QTextCharFormat function_format;
    QTextCharFormat quotation_format;
    QTextCharFormat preproc_format;
};

#endif // HIGHLIGHTER_H
