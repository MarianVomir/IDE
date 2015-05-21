#include "VisualStyles.h"

VisualStyles::VisualStyles()
{
    WindowStyle =

          /*  "background-color: #222222;"
            "color: #E6F096;"
            "border-color: #000000;"
            "selection-background-color: #AAAAAA;"*/

            ""

            ;

    NormalTextColor = QColor(0, 0, 0);
    CurrentLineColor = QColor(240, 240, 240);
    LineNumberAreaBackground = QColor(200, 200, 200);
    LineNumberAreaForeground = QColor(90, 90, 90);

}

VisualStyles::~VisualStyles()
{

}

