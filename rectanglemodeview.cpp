#include "rectanglemodeview.h"
#include "detail.h"

namespace {
    constexpr QSizeF kZeroSize{0, 0};
    constexpr QPointF kZeroPoint{0, 0};
}

bool shouldDeleteZeroSizeItem(QGraphicsRectItem* currentItem, QPointF startPos);

RectangleModeView::RectangleModeView(QGraphicsScene *scene)
    : QGraphicsView(scene),
      currentItem_(nullptr),
      startCursorPos_(kZeroPoint),
      fillColor_(Qt::yellow),
      strokeColor_(Qt::black) {}

void RectangleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(QRectF{startCursorPos_, kZeroSize},
                                        QPen{strokeColor_},
                                        QBrush{fillColor_});
        detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void RectangleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        QPointF currentCursorPos = mapToScene(event->pos());
        QRectF updatedRectangle = detail::updateRectangleSize(startCursorPos_, currentCursorPos);
        currentItem_->setRect(updatedRectangle.normalized());
        emit changeStateOfScene();
    }
}

void RectangleModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (shouldDeleteZeroSizeItem(currentItem_, startCursorPos_)) {
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
