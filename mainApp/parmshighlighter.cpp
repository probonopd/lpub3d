/****************************************************************************
**
** Copyright (C) 2015 Trevor SANDY. All rights reserved.
** Copyright (C) 2015 Trevor SANDY. All rights reserved.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/****************************************************************************
 *
 * This implements a syntax highlighter class that works with the editwindow
 * to display LDraw files with syntax highlighting.
 *
 * Please see lpub.h for an overall description of how the files in LPub
 * make up the LPub program.
 *
 ***************************************************************************/

#include <QtGui>
#include "parmshighlighter.h"

ParmsHighlighter::ParmsHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    officialMetaFormat.setForeground(Qt::darkGreen);
    officialMetaFormat.setFontWeight(QFont::Bold);

    QStringList officialMetaPatterns;
    officialMetaPatterns
//       << "\\bFILE[^\n]*"
       << "\\b//[^\n]*";

    foreach (QString pattern, officialMetaPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = officialMetaFormat;

        highlightingRules.append(rule);
    }

    LPubParmsFormat.setForeground(Qt::darkGreen);
    LPubParmsFormat.setFontWeight(QFont::Bold);

    QStringList LPubParmsPatterns;
    LPubParmsPatterns
//        << "\\bCUSTOM[^\n]*"
        << "\\B#[^\n]*"
           ;

    foreach (QString pattern, LPubParmsPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = LPubParmsFormat;

        highlightingRules.append(rule);
    }

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    commentStartExpression = QRegExp("\\b#\\b+[^\n]*");
    commentEndExpression   = QRegExp("\\#!\\b+[^\n]*");
}

void ParmsHighlighter::highlightBlock(const QString &text)
{
    foreach (HighlightingRule rule, highlightingRules) {
        QRegExp expression(rule.pattern);

        int index = text.indexOf(expression);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        int endIndex = text.indexOf(commentEndExpression, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression,
                                                startIndex + commentLength);
    }
}

