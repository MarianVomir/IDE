#ifndef VISUALSTYLES_H
#define VISUALSTYLES_H

#include <QString>
#include <QColor>

class VisualStyles
{
public:
    VisualStyles();
    ~VisualStyles();

    QString WindowStyle;
    QColor NormalTextColor;
    QColor CurrentLineColor;
    QColor LineNumberAreaBackground;
    QColor LineNumberAreaForeground;

    /*
    Default: dark style
        - dark grey background
        - yellow-ish foreground
        - light grey selection
        - a little darker grey current line
*/

};

#endif // VISUALSTYLES_H
