#include <QtWidgets>
#include <QFont>

#include "qlogwidgets.h"

#define PART_OF_CONTENT_TO_KEEP 0.75

/*!
    \class QLogText
    \brief The QLogText class is a very basic widget
    that is used to display only plain text (like a log).
    
    - The widget can only use a proportionnal font
    - Scrollbars are automatically updated when text is added
    - Copy/paste/cur and selection are not implemented

    This widget was tested with around 100,000 lines.

*/

/*!
    \enum QLogText::Limit
    Not yet implemented
*/

/*!
    \var QLogText::Limit QLogText::NoLimit
    Not yet implemented
*/

/*!
    Constructs an empty QTextEdit with parent \a
    parent.
    
    The default font will be a 10pt Courier and the text will be wrapped.
*/
QLogText::QLogText( QWidget * parent ) : QAbstractScrollArea(parent), wrapMode(true), numberOfLines(0), numberOfColumns(0), limit(NoLimit)
{
    curFont = QFont("Courier", 10, QFont::Normal);
    fontsize = QFontMetrics(curFont).lineSpacing();

    setAttribute(Qt::WA_StaticContents);
    setAttribute(Qt::WA_NoBackground);
    setUpdatesEnabled(true);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    if (!wrapMode)
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    else 
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    verticalScrollBar ()->setMaximum(content.size() * fontsize);
    verticalScrollBar ()->setPageStep(int(height() * 0.8));
    verticalScrollBar ()->setSingleStep(fontsize);   
}

/*!
    Destructor.
*/
QLogText::~QLogText()
{
}

/*!
    Sets the wrapping mode.
*/
void QLogText::setWrapMode(bool w)
{
    wrapMode = w;
    if (!wrapMode)
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    else 
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

/*!
    Set the content limit
*/
void QLogText::setLimit(int l)
{
    limit = l;
}

/*!
    Remove all text and clear the widget
*/
void QLogText::clearBuffer() 
{
    content.clear();
    updateNumberOfLines();
    numberOfColumns = 0;
    viewport()->update();
}

/*!
    Set the font used by the widget
    This font must be a proportionnal font (like Courier)
*/ 
void QLogText::setFont(QFont &f)
{
    if (f.styleHint() == QFont::TypeWriter || f.styleHint() == QFont::Courier) {
        curFont = f;
        fontsize = QFontMetrics(curFont).lineSpacing();
        verticalScrollBar ()->setSingleStep(fontsize);
    }
}

/*!
    Append text to the widget.
    If \a str contains several lines, \a str is splitted.
    Scrollbars are updated.
*/
void QLogText::append(const QString &str)
{
    if (!str.isEmpty()) {
        QStringList subList;
        int previous_index;
        int charPerLines = viewport()->width() / QFontMetrics(curFont).maxWidth();
    
        subList = str.split(QChar(0x000D));

        /*! \todo Real special case : manage backspace only if it's the first char
                  (which is usually the case) */
        if ((subList[0])[0].unicode() == 0x0008) { // Backspace
            removeLastChar(); 
            (subList[0]).remove(0, 1);
        }
        
        manageLimit();
        
        previous_index = content.size() == 0 ? 0 : content.size() - 1;
        if (content.size() == 0) {
            content << subList;
        } else {
            if (wrapMode) {
                numberOfLines -= (content[previous_index].size() / charPerLines) + 1;
            }
            content[content.size() - 1].append(subList.takeFirst());
            content << subList;
        }
    
        if (!wrapMode) {
            numberOfLines = content.size();
            for (int i = previous_index; i < content.size(); i++)
                if (content[i].size() > numberOfColumns) numberOfColumns = content[i].size();
        } else {
            for (int i = previous_index; i < content.size(); i++)
                numberOfLines += (content[i].size() / charPerLines) + 1;
        }
    
        updateScrollbar();
        // Set the scrollbar position at the bottom of the widget
        if (!verticalScrollBar()->isSliderDown() && !horizontalScrollBar()->isSliderDown()) {
            verticalScrollBar()->setValue(verticalScrollBar()->maximum());
            if (!wrapMode) {
                int val = content[content.size() - 1].size() * QFontMetrics(curFont).maxWidth() - viewport()->width();
                horizontalScrollBar()->setValue(val < 0 ? 0 : val);
            }
            viewport()->update();
        }   
    }
}

/*!
    Append only one text line to the widget.
    Scrollbars are updated.
*/
void QLogText::appendLine(const QString &str)
{
    if (!str.isEmpty()) {
        //QStringList subList;
        int previous_index;
        int charPerLines = viewport()->width() / QFontMetrics(curFont).maxWidth();

        manageLimit();
    
        previous_index = content.size() == 0 ? 0 : content.size() - 1;
        content << str;
    
        if (!wrapMode) {
            numberOfLines = content.size();
            if (str.size() > numberOfColumns) numberOfColumns = str.size();
        } else {
            numberOfLines += (str.size() / charPerLines) + 1;
        }
    
        updateScrollbar();
        // Set the scrollbar position at the bottom of the widget
        if (!verticalScrollBar()->isSliderDown() && !horizontalScrollBar()->isSliderDown()) {
            verticalScrollBar()->setValue(verticalScrollBar()->maximum());
            if (!wrapMode) {
                int val = content[content.size() - 1].size() * QFontMetrics(curFont).maxWidth() - viewport()->width();
                horizontalScrollBar()->setValue(val < 0 ? 0 : val);
            }
            viewport()->repaint();
        }    
    }
}

void QLogText::manageLimit()
{
    if ((limit != NoLimit) && (content.size() >= limit)) {
        // Remove a part of the content
        QList< QString >::iterator e = content.begin();
        e += (int)(limit * (1 - PART_OF_CONTENT_TO_KEEP));
        content.erase(content.begin(), e);
    }
}

/*!
    Remove the last char in the last line of the widget content.
    Not very useful in usual cases...
*/
void QLogText::removeLastChar()
{
    if (content.size() != 0) {
        content.last().remove ( content.last().size() - 1, 1 );
    }
}

/*! \internal
    Update scrollbar values (current and maximum)
*/
void QLogText::updateScrollbar()
{
    // Update the scrollbar
    int max = (numberOfLines * fontsize) - viewport()->height();
    verticalScrollBar ()->setMaximum(max < 0 ? 0 : max);
    if (!wrapMode) {
        max = (numberOfColumns * QFontMetrics(curFont).maxWidth()) - viewport()->width();
        horizontalScrollBar()->setMaximum(max < 0 ? 0 : max);
    }

}

/*! \internal
    Update the number of lines and update scrollbars
*/
void QLogText::updateNumberOfLines()
{
    if (!wrapMode) {
        numberOfLines = content.size();
    } else {
        int charPerLines = viewport()->width() / QFontMetrics(curFont).maxWidth();
        numberOfLines = 0;
        for (int i = 0; i < content.size(); ++i) {
            numberOfLines += (content.at(i).size() / charPerLines) + 1;
        }
    }
    
    int max = (numberOfLines * fontsize) - viewport()->height();
    verticalScrollBar ()->setMaximum(max < 0 ? 0 : max);
    
}

/*! \internal
    All input event are ignored
*/
void QLogText::keyPressEvent ( QKeyEvent * e )
{
    switch (e->key()) {
        case Qt::Key_PageUp:
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
            break;
        case Qt::Key_PageDown:
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
            break;
        default:
            e->ignore();
    }
}

/*! \internal
*/
void QLogText::resizeEvent ( QResizeEvent * event )
{
    QPainter p(viewport());

    updateNumberOfLines();
    verticalScrollBar ()->setPageStep(int(height() * 0.8));
}

/*! \internal
*/
void QLogText::paintEvent(QPaintEvent *event)
{
    QPainter p(viewport());
    paint(&p, event);
}

/*! \internal
    To paint the widget.
    Only lines displayed by the widget are painted. The very low QTextLayout API is used
    to optimize the display.
*/
void QLogText::paint (QPainter *p,  QPaintEvent * e)
{
    const int xOffset = horizontalScrollBar ()->value();
    const int yOffset = verticalScrollBar ()->value();

    QRect r = e->rect();
    p->translate(-xOffset, -yOffset);
    r.translate(xOffset, yOffset);
    p->setClipRect(r);

    p->setPen(Qt::black);
    
    if (!wrapMode) {
        for (int i = (r.top() / fontsize); i < (r.bottom() / fontsize) + 1; i++) {
            if (i < content.size()) {

                QTextLayout textLayout(content[i], curFont);
                textLayout.beginLayout();
                textLayout.createLine();
                textLayout.endLayout();

                textLayout.draw (p, QPointF(0, i * fontsize) );
            }
        }
    } else {
        int charPerLines = viewport()->width() / QFontMetrics(curFont).maxWidth();
        int i = 0, nbOfLines = 0, beginline = (r.top() / fontsize), endline = (r.bottom() / fontsize) + 1;
        // Search for the first line to display
        while (nbOfLines < beginline && i < content.size()) {
            nbOfLines += (content[i].size() / charPerLines) + 1;
            ++i;
        }
        
        if (i) {
            --i;
            nbOfLines -= (content[i].size() / charPerLines) + 1;
        }
        
        // Repaint the part of the screen
        while (nbOfLines < endline && i < content.size()) {
            int n = (content[i].size() / charPerLines) + 1;

            for (int j = 0; j < n; j++) {
                if ((nbOfLines + j >= beginline) && (nbOfLines + j + 1 <= endline)) {

                    QTextLayout textLayout(content.at(i).mid(j * charPerLines, charPerLines), curFont);
                    textLayout.beginLayout();
                    textLayout.createLine();
                    textLayout.endLayout();

                    textLayout.draw (p, QPointF(0, (nbOfLines + j /*! + 1 : \todo To check*/) * fontsize) );
                }
            }
            nbOfLines += n;
            
            ++i;
        }
    }

}

QTextStream& operator<<(QTextStream& out, const QLogText& text)
{
    for (int i = 0; i < text.content.size(); ++i) {
        out << text.content.at(i) << endl;
    }
    return out;
}



/*!
    \class QLogTable
    \brief The QLogTable class is a very basic widget
    that is used to display text in a table (like QTableWidget).
    
    - The widget can only use a proportionnal font
    - Scrollbars are automatically updated when text is added
    - Copy/paste/cur and selection are not implemented
    - The number of columns can't be changed after creation
*/

/*!
    Constructs an empty QLogTable with parent \a
    parent and \a numCols columns.
    
    The default font will be a 10pt Courier.
*/
QLogTable::QLogTable(int numCols,  QWidget * parent ) : QAbstractScrollArea(parent), numberOfColumns(numCols), limit(NoLimit)
{
    curFont = QFont("Courier", 10, QFont::Normal);
    rowHeight = QFontMetrics(curFont).lineSpacing() + 3;

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    content = new QList< QList<QString> >;
    colWidth = new int[numberOfColumns];
    for (int i = 0; i < numberOfColumns; i++) { colWidth[i] = 0; }
    
    verticalScrollBar ()->setMaximum(content->size() * rowHeight);
    verticalScrollBar ()->setSingleStep(rowHeight);
    verticalScrollBar ()->setPageStep((height() * 8) / 10);
    horizontalScrollBar ()->setSingleStep(QFontMetrics(curFont).maxWidth());
    horizontalScrollBar ()->setPageStep(10 * QFontMetrics(curFont).maxWidth());
}

/*!
    Destructor.
*/
QLogTable::~QLogTable()
{
    delete[] colWidth;
}

/*!
    Remove all text in the table and clear the widget
*/
void QLogTable::clearBuffer() 
{
    content->clear();
    viewport()->update();
}

/*!
    Set the font used by the widget
    This font must be a proportionnal font (like Courier)
*/ 
void QLogTable::setFont(QFont &f)
{
    if (f.styleHint() == QFont::TypeWriter || f.styleHint() == QFont::Courier) {
        curFont = f;
        rowHeight = QFontMetrics(curFont).lineSpacing() + 3;
        verticalScrollBar()->setSingleStep(rowHeight);
    }
}

/*!
    Set the content limit
*/
void QLogTable::setLimit(int l)
{
    limit = l;
}

/*!
    Set the number of columns of the widget
    Remove some columns or add some new empty columns
*/ 
void QLogTable::setColumnsNumber(int n)
{
    if ((n > 0) && (n != numberOfColumns)) {
        int *colw = new int[n];
        for (int i = 0; i < (n < numberOfColumns ? n : numberOfColumns); i++) { colw[i] = colWidth[i]; }
        if (n > numberOfColumns) {
            for (int i = numberOfColumns; i < n; i++) { colw[i] = 0; }
        }
        
        int *oldcolw = colWidth;
        
        colWidth = colw;
        numberOfColumns = n;
        
        delete [] oldcolw;
    }
}

/*!
    Append a row with \a str data
    \a str is a QStringList where each item is a cell of the row.
    
    So number of columns should equal to str.size()
*/ 
void QLogTable::appendRow(const QStringList &str)
{
    if (!str.isEmpty()) {
        
        manageLimit();
        
        (*content) << str;

        for (int i = 0; (i < numberOfColumns) && (i < str.size()); i++) {
            if (str[i].size() > colWidth[i]) colWidth[i] = str[i].size();
        }
        
        updateScrollbar();
        // Set the scrollbar position at the bottom of the widget
        if (!verticalScrollBar()->isSliderDown() && !horizontalScrollBar()->isSliderDown()) {
            verticalScrollBar()->setValue(verticalScrollBar()->maximum());
            viewport()->update();
        }
    }
}

void QLogTable::manageLimit()
{
    if ((limit != NoLimit) && (content->size() >= limit)) {
        // Remove a part of the content
        QList< QList<QString> >::iterator e = content->begin();
        e += (int)(limit * (1 - PART_OF_CONTENT_TO_KEEP));
        content->erase(content->begin(), e);
    }
}

/*! \internal
    Update scrollbar values (current and maximum)
*/
void QLogTable::updateScrollbar()
{
    // Update the scrollbar
    int max = (content->size() * rowHeight) - viewport()->height();
    verticalScrollBar()->setMaximum(max < 0 ? 0 : max);

    int maxlinelength = 0;

    for (int i = 0; i < numberOfColumns; i++) {
        maxlinelength += colWidth[i] * QFontMetrics(curFont).maxWidth() + 5;
    }
    
    max = (maxlinelength) - viewport()->width();
    horizontalScrollBar()->setMaximum(max < 0 ? 0 : max);
}

/*! \internal
    All input event are ignored
*/
void QLogTable::keyPressEvent ( QKeyEvent * e )
{
    switch (e->key()) {
        case Qt::Key_PageUp:
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
            break;
        case Qt::Key_PageDown:
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
            break;
        default:
            e->ignore();
    }
}

/*! \internal
*/
void QLogTable::resizeEvent ( QResizeEvent * event )
{
    QPainter p(viewport());
    verticalScrollBar ()->setPageStep((height() * 8)/ 10);
    updateScrollbar();
}

/*! \internal
*/
void QLogTable::paintEvent(QPaintEvent *event)
{
    QPainter p(viewport());
    paint(&p, event);
}

/*! \internal
    To paint the widget.
    Only lines displayed by the widget are painted. The very low QTextLayout API is used
    to optimize the display.
*/
void QLogTable::paint (QPainter *p,  QPaintEvent * e)
{
    bool lineDrawn = false;
    const int xOffset = horizontalScrollBar ()->value();
    const int yOffset = verticalScrollBar ()->value();

    QPen textPen(Qt::black);
    QPen linesPen(Qt::gray);
    linesPen.setStyle(Qt::DotLine);

    QRect r = e->rect();
    p->translate(-xOffset, -yOffset);
    r.translate(xOffset, yOffset);
    p->setClipRect(r);

    for (int i = (r.top() / rowHeight); i < (r.bottom() / rowHeight) + 1; i++) {
        if (i < content->size()) {
            int x = 0;
            for (int j = 0; j < (numberOfColumns < content->at(i).size() ? numberOfColumns : content->at(i).size()); j++) {
                
                QTextLayout textLayout(content->at(i).at(j), curFont);
                textLayout.beginLayout();
                textLayout.createLine();
                textLayout.endLayout();

                p->setPen(textPen);
                textLayout.draw (p, QPointF(x, i * rowHeight) );

                x += colWidth[j] * QFontMetrics(curFont).maxWidth() + 5;

                if (!lineDrawn) { // Draw the vertical line but only once
                    p->setPen(linesPen);
                    p->drawLine(x - 3, r.top(), x - 3, (content->size() * rowHeight < r.bottom() ? content->size() * rowHeight - 2 : r.bottom()));
                }
                
            }
            lineDrawn = true;

            // Draw horizontal lines (normally x = right limit of the table)
            p->setPen(linesPen);
            p->drawLine(r.left(), (i + 1) * rowHeight - 2, ((x - 3) < r.right() ? (x - 3) : r.right()), (i + 1) * rowHeight - 2);
        }
    }
}

QTextStream& operator<<(QTextStream& out, const QLogTable& table)
{
    for (int i = 0; i < table.content->size(); ++i) {
        for (int j = 0; j < (table.numberOfColumns < table.content->at(i).size() ? table.numberOfColumns : table.content->at(i).size()); j++) {
            QString str = table.content->at(i).at(j);
            str = str.leftJustified(1 + table.colWidth[j], ' ');
            out << str;
            
        }
        out << endl;
    }
    return out;
}
