#include "CSyntaxHighlighter.h"

CSyntaxHighlighter::CSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightRule rule;
    QString pattern;

    // NORMAL FONT
    QTextCharFormat normalFormat;
    normalFormat.setForeground(Qt::black);
    normalFormat.setFontItalic(false);
    normalFormat.setFontWeight(0);

    rule.format = normalFormat;
    rule.pattern = QRegExp(".");

    highlightingRules.append(rule);

    // STRING LITERALS
    stringLiteralFormat.setForeground(Qt::red);
    stringLiteralFormat.setFontWeight(QFont::Bold);
    pattern = stringLiteralPattern;
    rule.pattern = QRegExp(pattern);
    rule.format = stringLiteralFormat;
    highlightingRules.append(rule);

    // CHAR LITERALS
    charLiteralFormat.setForeground(Qt::green);
    charLiteralFormat.setFontWeight(QFont::Bold);
    pattern = charLiteralPattern;
    rule.pattern = QRegExp(pattern);
    rule.format = charLiteralFormat;
    highlightingRules.append(rule);

    // FUNCTIONS
    functionFormat.setForeground(Qt::magenta);
    functionFormat.setFontWeight(QFont::Bold);
    pattern = functionPattern;
    rule.pattern = QRegExp(pattern);
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // KEYWORDS
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;

    for (auto& keyword : Ckeywords)
    {
        pattern = "\\b" + keyword + "\\b"; // match whole word only

        keywordPatterns << pattern;

        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;

        highlightingRules.append(rule); // append rule to highlight current keyword
    }

    // PREPROCESSOR DIRECTIVES
    preprocessFormat.setForeground(QBrush(QColor(20, 200, 40)));

    for (auto& directive : CpreprocessDirectives)
    {
        pattern = directive;
        rule.pattern = QRegExp(pattern);
        rule.format = preprocessFormat;

        highlightingRules.append(rule);
    }

    // SINGLE LINE COMMENTS
    singleLineCommentFormat.setForeground(QBrush(QColor(0x80, 0x80, 0x80)));
    rule.format = singleLineCommentFormat;
    pattern = singleLineCommentPattern;
    rule.pattern = QRegExp(pattern);

    highlightingRules.append(rule);

    // MULTI LINE COMMENTS
    multiLineCommentFormat = singleLineCommentFormat; // multi line comment looks the same as single line
    rule.format = multiLineCommentFormat;

    commentStartExpression = QRegExp(commentStartPattern);
    commentEndExpression = QRegExp(commentEndPattern);
}
void CSyntaxHighlighter::highlightBlock(const QString &text)
{
    // Block states
    // ------------
    // 0 - nothing to be done
    // 1 - inside multiline comment


    for (auto& rule : highlightingRules) // take each rule
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) // if rule matches current textblock
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format); // set text format from beginning of match to end of match
            index = expression.indexIn(text, index + length); // look for other matches from where previous one left off
        }
    }

    setCurrentBlockState(0); // used to signal that nothing special should be done

    int startIndex = 0;
    if (previousBlockState() != 1) // if prev block state is not 1, we are not in a multiline comment, so we check to see if such a comment begins somewhere in the current block
    {
        startIndex = commentStartExpression.indexIn(text);
    }

    while (startIndex >= 0) // if start of multiline comment found
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex); // search for end
        int commentLength = 0;
        if (endIndex == -1) // if end of multiline comment NOT FOUND
        {
            setCurrentBlockState(1); // still in comment block
            commentLength = text.length() - startIndex;
        }
        else // end of multiline comment found
        {
            commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength); // check if there is another comment beginning in the remainder of the current block
    }
}
