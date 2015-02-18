#ifndef CLEXIC_H
#define CLEXIC_H

#include <QVector>
#include <QString>

extern QVector<QString> Ckeywords;
extern QString functionPattern;
extern QString stringLiteralPattern;
extern QString charLiteralPattern;
extern QVector<QString> CpreprocessDirectives;
extern QString singleLineCommentPattern;
extern QString commentStartPattern;
extern QString commentEndPattern;

#endif // CLEXIC_H
