#include "rectanglemodeview.h"
#include "detail.h"

bool shouldDeleteZeroSizeItem(QGraphicsRectItem* currentItem, QPointF startPos);

RectangleModeView::RectangleModeView(QGraphicsScene *scene)
    : QGraphicsView(scene),
      currentItem_(nullptr),
      fillColor_(Qt::black),
      strokeColor_(Qt::black) {}

void RectangleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(QRectF{startPos_, QSizeF(0, 0)},
                                        QPen{strokeColor_},
                                        QBrush{fillColor_});
        detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void RectangleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        QPointF currentCursorPos = mapToScene(event->pos());
        qreal currentWidth = currentCursorPos.x() - startPos_.x();
        qreal currentHeight = currentCursorPos.y() - startPos_.y();
        QRectF rect{startPos_, QSizeF{currentWidth, currentHeight}};
        currentItem_->setRect(rect.normalized());
        emit changeStateOfScene();
    }
}

void RectangleModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (shouldDeleteZeroSizeItem(currentItem_, startPos_)) {
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}

void RectangleModeView::changeFillColor(const QColor& color) {
    fillColor_ = color;
}

void RectangleModeView::changeStrokeColor(const QColor& color) {
    strokeColor_ = color;
}

bool shouldDeleteZeroSizeItem(QGraphicsRectItem* currentItem, QPointF startPos) {
    auto rect = currentItem->rect();
    return rect.topLeft() == startPos && rect.size() == QSizeF(0, 0);
}
