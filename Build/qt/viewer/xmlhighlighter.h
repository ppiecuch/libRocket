#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include <QtGui/QSyntaxHighlighter>
#include <QList>
#include <QPair>
#include <QRegExp>

class XMLHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    XMLHighlighter(QTextDocument *document);
    void setHighlightedString(const QString &str);


protected:
    void highlightBlock(const QString &text);

private:
    typedef QPair<QRegExp,QTextCharFormat> RulePair;
    QList<RulePair> highlightingRules;
    QTextCharFormat valueFormat;
    QRegExp valueStartExpression;
    QRegExp valueEndExpression;
};

#endif
