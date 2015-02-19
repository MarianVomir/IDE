#include "CLexic.h"

QVector<QString> Ckeywords (QVector<QString>()

    << "auto"
    << "break"
    << "case"
    << "char"
    << "const"
    << "continue"
    << "default"
    << "do"
    << "double"
    << "else"
    << "enum"
    << "extern"
    << "float"
    << "for"
    << "goto"
    << "if"
    << "int"
    << "long"
    << "register"
    << "return"
    << "short"
    << "signed"
    << "sizeof"
    << "static"
    << "struct"
    << "switch"
    << "typedef"
    << "union"
    << "unsigned"
    << "void"
    << "volatile"
    << "while"

);

QString functionPattern = "\\b[A-Za-z_][A-Za-z0-9_]*(?=[\\s\\r\\n\\t]*\\()";

QString stringLiteralPattern = "\"([^\"\\\\]|\\\\.)*\"";

QString charLiteralPattern = "\'([^\\\\]|\\\\.)\'";

QVector<QString> CpreprocessDirectives (QVector<QString> ()

    << "#include <.+>"
    << "#include \".+\""
    << "#define [^\\\\]+|[\\\n]+"
    << "#ifndef .+"
    << "#ifdef .+"
    << "#endif"
    << "#pragma .+"

);

QString singleLineCommentPattern = "//[^\\n]*";

QString commentStartPattern = "/\\*";
QString commentEndPattern = "\\*/";
