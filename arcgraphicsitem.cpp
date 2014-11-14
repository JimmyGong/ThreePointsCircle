#include "arcgraphicsitem.h"
#include "qmath.h"
#include <QPen>
#include <QDebug>
#include <QPainterPath>

ArcGraphicsItem::ArcGraphicsItem(int i,
                                 QPointF point1,
                                 QPointF point2,
                                 QPointF point3)
    : m_id(i), p1(point1), p2(point2), p3(point3)

{
    init();
}


ArcGraphicsItem::ArcGraphicsItem()
{
    p1 = QPointF(0,0);
    p2 = QPointF(0,1);
    p3 = QPointF(1,1);
    m_id = -1;
    init();
}

ArcGraphicsItem::~ArcGraphicsItem()
{

}

void ArcGraphicsItem::init()
{
    lineBC = QLineF(p2, p3);
    lineAC = QLineF(p1, p3);
    lineBA = QLineF(p2, p1);

    rad = qAbs(lineBC.length()/(2*qSin(qDegreesToRadians(lineAC.angleTo(lineBA)))));

    bisectorBC = QLineF(lineBC.pointAt(0.5), lineBC.p2());
    bisectorBC.setAngle(lineBC.normalVector().angle());

    bisectorBA = QLineF(lineBA.pointAt(0.5), lineBA.p2());
    bisectorBA.setAngle(lineBA.normalVector().angle());
    bisectorBA.intersect(bisectorBC, &center);

    circle = QRectF(center.x() - rad, center.y() - rad, rad*2, rad*2);

    lineOA = QLineF(center, p1);
    lineOB = QLineF(center, p2);
    lineOC = QLineF(center, p3);


    startAngle = lineOA.angle();
    spanAngle = lineOA.angleTo(lineOC);
    // Make sure that the span angle covers all three points with the second point in the middle
    if(qAbs(spanAngle) < qAbs(lineOA.angleTo(lineOB)) || qAbs(spanAngle) < qAbs(lineOB.angleTo(lineOC)))
    {
        // swap the end point and invert the spanAngle
        startAngle = lineOC.angle();
        spanAngle = 360 - spanAngle;
    }

    int w = 10;
    boundingRectTemp = circle.adjusted(-w, -w, w, w);
}

QRectF ArcGraphicsItem::boundingRect() const
{
    // outer most edges
    return boundingRectTemp;
}

void ArcGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen paintpen;
    painter->setRenderHint(QPainter::Antialiasing);
    paintpen.setWidth(1);
    // Draw arc

    if(isSelected())
    {
        painter->setBrush(Qt::SolidPattern);
        paintpen.setColor(Qt::black);
        painter->setPen(paintpen);
    }
    else
    {
        paintpen.setStyle(Qt::DashLine);
        paintpen.setColor(Qt::black);
        painter->setPen(paintpen);
    }

    QPainterPath path;
    path.arcMoveTo(circle,startAngle);
    path.arcTo(circle, startAngle, spanAngle);

    // Draw points

    if (isSelected())
    {
        // sets brush for end points
        painter->setBrush(Qt::SolidPattern);
        paintpen.setColor(Qt::red);
        painter->setPen(paintpen);

        qreal ptRad = 10;
        painter->drawEllipse(p1, ptRad, ptRad);
        painter->drawEllipse(p2, ptRad, ptRad);
        painter->drawEllipse(p3, ptRad, ptRad);
    }

    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
}

QPainterPath ArcGraphicsItem::shape() const
{
    QPainterPath path;
    path.arcMoveTo(circle,startAngle);
    path.arcTo(circle, startAngle, spanAngle);
    return path;
}
