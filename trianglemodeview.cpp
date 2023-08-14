#include "trianglemodeview.h"

TriangleModeView::TriangleModeView(QGraphicsScene* scene, bool& isModified)
    : QGraphicsView(scene), lineItem1_(nullptr), lineItem2_(nullptr), isModified_(isModified)
{
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
}

void TriangleModeView::mousePressEvent(QMouseEvent* event) {
    isModified_ = true;
    if (event->button() == Qt::LeftButton) {
        if (clickCount_ == 0) {
            startPoint_ = mapToScene(event->pos());
            lineItem1_ = scene()->addLine(QLineF(startPoint_, startPoint_));
            lineItem1_->setPen(QPen(Qt::black));
        } else if (clickCount_ == 1) {
            secondPoint_ = mapToScene(event->pos());
            lineItem2_ = scene()->addLine(QLineF(secondPoint_, secondPoint_));
            lineItem2_->setPen(QPen(Qt::black));
        } else if (clickCount_ == 2) {
            resetTemporaryItems();
            QPointF thirdPoint = mapToScene(event->pos());
            createTriangle(thirdPoint);
        }

        ++clickCount_;
        if (clickCount_ > 2) clickCount_ = 0;
    }
}

void TriangleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (clickCount_ == 1 && lineItem1_ != nullptr) {
        QPointF currentPoint = mapToScene(event->pos());
        lineItem1_->setLine(QLineF(startPoint_, currentPoint));
        lineItem1_->setPen(QPen(Qt::black, 1));
    } else if (clickCount_ == 2 && lineItem2_) {
        QPointF currentPoint = mapToScene(event->pos());
        lineItem2_->setLine(QLineF(secondPoint_, currentPoint));
        lineItem2_->setPen(QPen(Qt::black, 1));
    }
}

void TriangleModeView::resetTemporaryItems() {
    scene()->removeItem(lineItem1_);
    scene()->removeItem(lineItem2_);
    delete lineItem1_;
    delete lineItem2_;
    lineItem1_ = nullptr;
    lineItem2_ = nullptr;
}

void TriangleModeView::createTriangle(const QPointF &thirdPoint) {
    QPolygonF triangle;
    triangle << startPoint_ << secondPoint_ << thirdPoint;

    QGraphicsPolygonItem *polygonItem = new QGraphicsPolygonItem(triangle);
    polygonItem->setBrush(QBrush{Qt::cyan});
    polygonItem->setPen(QPen{Qt::black});
    polygonItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    polygonItem->setFlag(QGraphicsItem::ItemIsMovable, true);

    scene()->addItem(polygonItem);
}
