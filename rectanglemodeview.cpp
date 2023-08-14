#include "rectanglemodeview.h"

RectangleModeView::RectangleModeView(QGraphicsScene *scene, bool& isModified)
    : QGraphicsView(scene), currentItem_(nullptr), drawing_(false), isModified_(isModified) {}

void RectangleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && !drawing_) {
        startPos_ = mapToScene(event->pos());

        currentItem_ = scene()->addRect(QRectF(startPos_, QSizeF(0, 0)), QPen(Qt::black), QBrush(Qt::yellow));
        currentItem_->setFlag(QGraphicsItem::ItemIsSelectable, true);
        currentItem_->setFlag(QGraphicsItem::ItemIsMovable, true);

        drawing_ = true;
    }
}

void RectangleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (drawing_ && currentItem_) {
        QPointF currentPos = mapToScene(event->pos());
        qreal width = currentPos.x() - startPos_.x();
        qreal height = currentPos.y() - startPos_.y();

        QRectF rect(startPos_, QSizeF(width, height));
        currentItem_->setRect(rect.normalized());
        isModified_ = true;
    }
}

void RectangleModeView::mouseReleaseEvent(QMouseEvent*) {
    if (drawing_ && currentItem_) {
        drawing_ = false;
        currentItem_ = nullptr;
    }
}
