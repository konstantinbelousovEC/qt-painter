#include "squaremodeview.h"

SquareModeView::SquareModeView(QGraphicsScene* scene, bool& isModified)
    : QGraphicsView(scene), currentItem_(nullptr), drawing_(false), isModified_(isModified) {}

void SquareModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && !drawing_) {
        centerPos_ = mapToScene(event->pos());

        currentItem_ = scene()->addRect(QRectF(centerPos_, QSizeF(0, 0)), QPen(Qt::black), QBrush(Qt::red));
        currentItem_->setFlag(QGraphicsItem::ItemIsSelectable, true);
        currentItem_->setFlag(QGraphicsItem::ItemIsMovable, true);

        drawing_ = true;
    }
}

void SquareModeView::mouseMoveEvent(QMouseEvent* event) {
    if (drawing_ && currentItem_) {
        QPointF currentPos = mapToScene(event->pos());
        qreal distance = QLineF(centerPos_, currentPos).length();

        qreal halfDistance = distance / 2;

        QRectF rect(centerPos_.x() - halfDistance / 2, centerPos_.y() - halfDistance / 2, halfDistance, halfDistance);
        currentItem_->setRect(rect.normalized());
        isModified_ = true;
    }
}

void SquareModeView::mouseReleaseEvent(QMouseEvent *) {
    if (drawing_ && currentItem_) {
        drawing_ = false;
        currentItem_ = nullptr;
    }
}
