#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

#define RULER_BREADTH 20

class QDRuler : public QWidget
{
    Q_OBJECT
    Q_ENUMS(RulerType)
    Q_PROPERTY(qreal origin READ origin WRITE setOrigin)
    Q_PROPERTY(qreal rulerUnit READ rulerUnit WRITE setRulerUnit)
    Q_PROPERTY(qreal rulerZoom READ rulerZoom WRITE setRulerZoom)

public:
    enum RulerType { Horizontal, Vertical };
    QDRuler(QDRuler::RulerType rulerType, QWidget* parent) : QWidget(parent), mRulerType(rulerType), mOrigin(0.), mRulerUnit(1.),
    mRulerZoom(1.), mMouseTracking(true), mDrawText(false)
    {
        setMouseTracking(true);
    }

    QSize minimumSizeHint() const
    {
        return QSize(RULER_BREADTH,RULER_BREADTH);
    }

    QDRuler::RulerType rulerType() const
    {
        return mRulerType;
    }

    qreal origin() const
    {
        return mOrigin;
    }

    qreal rulerUnit() const
    {
        return mRulerUnit;
    }

    qreal rulerZoom() const
    {
        return mRulerZoom;
    }

public slots:

    void setOrigin(const qreal origin)
    {
        if (mOrigin != origin)
        {
            mOrigin = origin;
            update();
        }
    }

    void setRulerUnit(const qreal rulerUnit)
    {
        if (mRulerUnit != rulerUnit)
        {
            mRulerUnit = rulerUnit;
            update();
        }
    }

    void setRulerZoom(const qreal rulerZoom)
    {
        if (mRulerZoom != rulerZoom)
        {
            mRulerZoom = rulerZoom;
            update();
        }
    }


    void setCursorPos(const QPoint cursorPos)
    {
        mCursorPos = this->mapFromGlobal(cursorPos);
        // seems to be not needed:
        //mCursorPos += QPoint(RULER_BREADTH, RULER_BREADTH);
        update();
    }

    void setMouseTrack(const bool track)
    {
        if (mMouseTracking != track)
        {
            mMouseTracking = track;
            update();
        }
    }

protected:
    void mouseMoveEvent(QMouseEvent* event)
    {
        mCursorPos = event->pos();
        update();
        QWidget::mouseMoveEvent(event);
    }

    void paintEvent(QPaintEvent* /*event*/)
    {
        QPainter painter(this);
        painter.setRenderHints(QPainter::TextAntialiasing);
        painter.setFont(QFont("System", 9, QFont::Normal));
        painter.setPen(QPen(Qt::black, 0));// zero width pen is cosmetic pen
        // We want to work with floating point, so we are considering
        // the rect as QRectF
        QRectF rulerRect = this->rect();

        // at first fill the rect
        painter.fillRect(rulerRect,QColor(236,233,216));

        // drawing a scale of 25
        drawAScaleMeter(&painter,rulerRect,25,(Horizontal == mRulerType ? rulerRect.height()
            : rulerRect.width())/2);
        // drawing a scale of 50
        drawAScaleMeter(&painter,rulerRect,50,(Horizontal == mRulerType ? rulerRect.height()
            : rulerRect.width())/4);
        // drawing a scale of 100
        mDrawText = true;
        drawAScaleMeter(&painter,rulerRect,100,0);
        mDrawText = false;

        // drawing the current mouse position indicator
        painter.setOpacity(0.7);
        drawMousePosTick(&painter);
        painter.setOpacity(1.0);

        // drawing no man's land between the ruler & view
        QPointF starPt = Horizontal == mRulerType ? rulerRect.bottomLeft()
            : rulerRect.topRight();
        QPointF endPt = Horizontal == mRulerType ? rulerRect.bottomRight()
            : rulerRect.bottomRight();
        painter.setPen(QPen(Qt::black,2));
        painter.drawLine(starPt,endPt);
    }

private:
    void drawAScaleMeter(QPainter* painter, QRectF rulerRect, qreal scaleMeter, qreal startPositoin)
    {
        // Flagging whether we are horizontal or vertical only to reduce
        // to cheching many times
        bool isHorzRuler = Horizontal == mRulerType;

        scaleMeter  = scaleMeter * mRulerUnit * mRulerZoom;

        // Ruler rectangle starting mark
        qreal rulerStartMark = isHorzRuler ? rulerRect.left() : rulerRect.top();
        // Ruler rectangle ending mark
        qreal rulerEndMark = isHorzRuler ? rulerRect.right() : rulerRect.bottom();

        // Condition A # If origin point is between the start & end mard,
        //we have to draw both from origin to left mark & origin to right mark.
        // Condition B # If origin point is left of the start mark, we have to draw
        // from origin to end mark.
        // Condition C # If origin point is right of the end mark, we have to draw
        // from origin to start mark.
        if (mOrigin >= rulerStartMark && mOrigin <= rulerEndMark)
        {
            drawFromOriginTo(painter, rulerRect, mOrigin, rulerEndMark, 0, scaleMeter, startPositoin);
            drawFromOriginTo(painter, rulerRect, mOrigin, rulerStartMark, 0, -scaleMeter, startPositoin);
        }
        else if (mOrigin < rulerStartMark)
        {
            int tickNo = int((rulerStartMark - mOrigin) / scaleMeter);
            drawFromOriginTo(painter, rulerRect, mOrigin + scaleMeter * tickNo,
                             rulerEndMark, tickNo, scaleMeter, startPositoin);
        }
        else if (mOrigin > rulerEndMark)
        {
            int tickNo = int((mOrigin - rulerEndMark) / scaleMeter);
            drawFromOriginTo(painter, rulerRect, mOrigin - scaleMeter * tickNo,
                             rulerStartMark, tickNo, -scaleMeter, startPositoin);
        }
    }

    void drawFromOriginTo(QPainter* painter, QRectF rulerRect, qreal startMark, qreal endMark, int startTickNo, qreal step, qreal startPosition)
    {
        bool isHorzRuler = Horizontal == mRulerType;
        int iterate = 0;

        for (qreal current = startMark;
             (step < 0 ? current >= endMark : current <= endMark); current += step)
        {
            qreal x1 = isHorzRuler ? current : rulerRect.left() + startPosition;
            qreal y1 = isHorzRuler ? rulerRect.top() + startPosition : current;
            qreal x2 = isHorzRuler ? current : rulerRect.right();
            qreal y2 = isHorzRuler ? rulerRect.bottom() : current;
            painter->drawLine(QLineF(x1,y1,x2,y2));
            if (mDrawText)
            {
                painter->setRenderHint(QPainter::Antialiasing, true);
    #ifdef SCALE_TICKS
                painter->drawText(x1 + 1,y1 + (isHorzRuler ? 7 : -2),QString::number(qAbs(int(step) * startTickNo++)));
    #else
                painter->drawText(x1 + 1,y1 + (isHorzRuler ? 7 : -2),QString::number(qAbs(startTickNo++)));
    #endif
                painter->setRenderHint(QPainter::Antialiasing, false);
                iterate++;
            }
        }
    }

    void drawMousePosTick(QPainter* painter)
    {
        if (mMouseTracking)
        {
            QPoint starPt = mCursorPos;
            QPoint endPt;
            if (Horizontal == mRulerType)
            {
                starPt.setY(this->rect().top());
                endPt.setX(starPt.x());
                endPt.setY(this->rect().bottom());
            }
            else
            {
                starPt.setX(this->rect().left());
                endPt.setX(this->rect().right());
                endPt.setY(starPt.y());
            }
            painter->setPen(QPen(Qt::darkGray,0,Qt::SolidLine));
            painter->drawLine(starPt,endPt);
        }
    }
private:
    RulerType mRulerType;
    qreal mOrigin;
    qreal mRulerUnit;
    qreal mRulerZoom;
    QPoint mCursorPos;
    bool mMouseTracking;
    bool mDrawText;
};
