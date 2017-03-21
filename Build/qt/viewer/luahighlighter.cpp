#include "LuaHighlighter.h"
#include "CodeEditor.h"

LuaHighlighter::LuaHighlighter(QTextDocument *document)
: QSyntaxHighlighter(document)
{
    QTextCharFormat keywordFormat;
    QTextCharFormat xmlElementFormat;
    QTextCharFormat xmlAttributeFormat;
    QTextCharFormat singleLineCommentFormat;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\band\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bbreak\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bdo\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\belse\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\belseif\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bend\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bfalse\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bfor\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bfunction\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bif\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bin\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\blocal\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bnil\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bnot\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bor\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\brepeat\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\breturn\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bthen\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\btrue\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\buntil\\b"), keywordFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("\\bwhile\\b"), keywordFormat));

    valueFormat.setForeground(Qt::darkRed);
    valueStartExpression = QRegExp("\"");
    valueEndExpression = QRegExp("\"(?=[\\s,()\\n\\r])");

    singleLineCommentFormat.setForeground(Qt::darkGreen);

    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("--[^\n]*"), singleLineCommentFormat));
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp("placeholder_rule_for_search_highlight"), keywordFormat));
}

void LuaHighlighter::setHighlightedString(const QString &str)
{
    QTextCharFormat keywordFormat;

    keywordFormat.setBackground(Qt::green);
    
    highlightingRules.removeLast();
    highlightingRules.push_back(QPair<QRegExp,QTextCharFormat>(QRegExp(str), keywordFormat));
    rehighlight();
}

void LuaHighlighter::highlightBlock(const QString &text)
{
    int index;
    int length;
    int startIndex;
    int endIndex;
    int commentLength;
    QTextCharFormat user_format;
    QTextCharFormat new_format;

    if(currentBlockUserData())
    {
        user_format = ((CodeEditor::BlockData *)currentBlockUserData())->format;
    }
    
    for(int i=0; i<highlightingRules.length();++i)
    {
        const QRegExp & expression = highlightingRules[i].first;
        const QTextCharFormat & format = highlightingRules[i].second;

        setCurrentBlockState(0);
        startIndex = 0;

        if(previousBlockState() != 1)
        {
            startIndex = valueStartExpression.indexIn(text);
        }

        while(startIndex >= 0)
        {
            endIndex = valueEndExpression.indexIn(text, startIndex);

            if(endIndex == -1)
            {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            }
            else
            {
                commentLength = endIndex - startIndex + valueEndExpression.matchedLength();
            }
            new_format = valueFormat;
            if(currentBlockUserData())
                new_format.merge(user_format);
            setFormat(startIndex, commentLength, new_format);

            startIndex = valueStartExpression.indexIn(text, startIndex + commentLength);
        }

        index = expression.indexIn(text);

        while(index >= 0)
        {
            length = expression.matchedLength();
            new_format = format;
            if(currentBlockUserData())
                new_format.merge(user_format);
            setFormat(index, length, new_format);
            index = expression.indexIn(text, index + length);
        }
    }
}