#ifndef CSYNTAXHIGHLIGHTER_H
#define CSYNTAXHIGHLIGHTER_H

#include <QRegExp>
#include <QTextCharFormat>
#include <QVector>
#include <QSyntaxHighlighter>
#include <QDebug>

#include "CLexic.h"

// Implemented based on the tutorial found at: http://doc.qt.digia.com/4.6/richtext-syntaxhighlighter.html

class CSyntaxHighlighter : public QSyntaxHighlighter
{
private:
    struct HighlightRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    // rules used in highlighting
    QVector<HighlightRule> highlightingRules;

    // formats
    QTextCharFormat keywordFormat;
    QTextCharFormat stringLiteralFormat;
    QTextCharFormat charLiteralFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat preprocessFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

public:
    CSyntaxHighlighter(QTextDocument* parent);
    void highlightBlock(const QString &text);
};

#endif // CSYNTAXHIGHLIGHTER_H
