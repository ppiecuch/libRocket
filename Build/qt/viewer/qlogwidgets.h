#ifndef QLOGWIDGETS_H
#define QLOGWIDGETS_H

#include <QAbstractScrollArea>
#include <QTextStream>

class QLogText : public QAbstractScrollArea
{
    Q_OBJECT

    public:
        enum Limit {
            NoLimit = -1
        };

        QLogText ( QWidget * parent = 0 );
        ~QLogText();

        void setFont(QFont &);
        void setLimit(int l);
        void setWrapMode(bool w);
        void removeLastChar();

    friend QTextStream& operator<<(QTextStream&, const QLogText&);
    
    protected:
        virtual void paintEvent ( QPaintEvent * event );
        virtual void resizeEvent ( QResizeEvent * event );
        virtual void keyPressEvent ( QKeyEvent * e );

        virtual void paint (QPainter *p,  QPaintEvent * e );
        
        void updateNumberOfLines();
        void updateScrollbar();
    
    public slots:
        void clearBuffer();
        void append(const QString &);
        void appendLine(const QString &);
        
    protected:
        QStringList content; // widget data
        QFont curFont; // font currently used by the widget
        int fontsize; // font size get from curFont (internally used)
        bool wrapMode; // wrap or not
        int numberOfLines; // number of lines (computed from content)
        int numberOfColumns; // number of columns (computed from content)
        int limit; // limit for number of lines (not implemented)

    private:
        void manageLimit();
};


class QLogTable : public QAbstractScrollArea
{
    Q_OBJECT

    public:
        enum Limit {
            NoLimit = -1
        };

        QLogTable (int numCols, QWidget * parent = 0 );
        ~QLogTable();

        void setLimit(int l);
        void setFont(QFont &);
        void setColumnsNumber(int);
    
    friend QTextStream& operator<<(QTextStream&, const QLogTable&);
    
    protected:
        virtual void paintEvent ( QPaintEvent * event );
        virtual void resizeEvent ( QResizeEvent * event );
        virtual void keyPressEvent ( QKeyEvent * e );
        virtual void paint (QPainter *p,  QPaintEvent * e );
        

        void updateScrollbar();
    
    public slots:
        void clearBuffer();
        void appendRow(const QStringList &);
        
    protected:
        QList< QList<QString> > *content;  // widget data
        int *colWidth; // columns width
        QFont curFont; // current font
        int rowHeight; // column height get from curFont (internally used)
        int numberOfLines; // number of lines/rows
        int numberOfColumns; // number of columns
        int limit; // limit for number of lines (not implemented)

    private:
        void manageLimit();
};

#endif // QLOGWIDGETS_H
