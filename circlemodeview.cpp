#include "circlemodeview.h"

CircleModeView::CircleModeView(QGraphicsScene* scene, bool& isModified)
    : QGraphicsView(scene), currentItem_(nullptr), drawing_(false), isModified_(isModified)
{
    setRenderHint(QPainter::Antialiasing);
}

void CircleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && !drawing_) {

        centerPos_ = mapToScene(event->pos());

        currentItem_ = scene()->addEllipse(QRectF(centerPos_, QSizeF(0, 0)), QPen(Qt::black), QBrush(Qt::lightGray));
        currentItem_->setFlag(QGraphicsItem::ItemIsSelectable, true);
        currentItem_->setFlag(QGraphicsItem::ItemIsMovable, true);

        drawing_ = true;
    }
}

void CircleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (drawing_ && currentItem_) {
        QPointF currentPos = mapToScene(event->pos());
        qreal radius = QLineF(centerPos_, currentPos).length();
        currentItem_->setRect(QRectF(centerPos_.x() - radius, centerPos_.y() - radius, radius * 2, radius * 2));
        isModified_ = true;
    }
}

void CircleModeView::mouseReleaseEvent(QMouseEvent*) {
    if (drawing_ && currentItem_) {
        drawing_ = false;
        currentItem_ = nullptr;
    }
}
