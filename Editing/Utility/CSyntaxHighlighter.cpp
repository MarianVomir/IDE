#include "CSyntaxHighlighter.h"

CSyntaxHighlighter::CSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightRule rule;
    QString pattern;

    // NORMAL FONT
    QTextCharFormat normalFormat;
    normalFormat.setForeground(Global::Visual.NormalTextColor);
    normalFormat.setFontItalic(false);
    normalFormat.setFontWeight(QFont::Normal);
    rule.format = normalFormat;
    rule.pattern = QRegExp(".");
    highlightingRules.append(rule);

    // FUNCTIONS
    functionFormat.setForeground(QBrush(QColor(50, 175, 20)));
    functionFormat.setFontWeight(QFont::Bold);
    pattern = functionPattern;
    rule.pattern = QRegExp(pattern);
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // KEYWORDS
    keywordFormat.setForeground(QBrush(QColor(225, 140, 15)));
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
    preprocessFormat.setForeground(QBrush(QColor(235, 30, 110)));

    for (auto& directive : CpreprocessDirectives)
    {
        pattern = directive;
        rule.pattern = QRegExp(pattern);
        rule.format = preprocessFormat;

        highlightingRules.append(rule);
    }

    // SINGLE LINE COMMENTS
    singleLineCommentFormat.setForeground(QBrush(QColor(150, 5, 235)));
    rule.format = singleLineCommentFormat;
    pattern = singleLineCommentPattern;
    rule.pattern = QRegExp(pattern);

    highlightingRules.append(rule);

    // MULTI LINE COMMENTS
    multiLineCommentFormat = singleLineCommentFormat; // multi line comment looks the same as single line
    rule.format = multiLineCommentFormat;

    commentStartExpression = QRegExp(commentStartPattern);
    commentEndExpression = QRegExp(commentEndPattern);

    // STRING LITERALS
    stringLiteralFormat.setForeground(QBrush(QColor(120, 135, 225)));
    stringLiteralFormat.setFontWeight(QFont::Bold);
    pattern = stringLiteralPattern;
    rule.pattern = QRegExp(pattern);
    rule.format = stringLiteralFormat;
    highlightingRules.append(rule);

    // CHAR LITERALS
    charLiteralFormat.setForeground(QBrush(QColor(50, 70, 155)));
    charLiteralFormat.setFontWeight(QFont::Bold);
    pattern = charLiteralPattern;
    rule.pattern = QRegExp(pattern);
    rule.format = charLiteralFormat;
    highlightingRules.append(rule);
}
void CSyntaxHighlighter::highlightBlock(const QString &text)
{
    static int NORMAL_STATE = 0;
    static int INSIDE_MULTILINE_COMMENT = 1;
    static int INSIDE_MULTILINE_MACRO = 2;

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

    setCurrentBlockState(NORMAL_STATE); // used to signal that nothing special should be done

    int startIndex = 0;
    if (previousBlockState() != INSIDE_MULTILINE_COMMENT) // if prev block state is not 1, we are not in a multiline comment, so we check to see if such a comment begins somewhere in the current block
    {
        startIndex = commentStartExpression.indexIn(text);
    }
    else if (previousBlockState() != INSIDE_MULTILINE_MACRO)
    {
    }

    while (startIndex >= 0) // if start of multiline comment found
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex); // search for end
        int commentLength = 0;
        if (endIndex == -1) // if end of multiline comment NOT FOUND
        {
            setCurrentBlockState(INSIDE_MULTILINE_COMMENT); // still in comment block
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
