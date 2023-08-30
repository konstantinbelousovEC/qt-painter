// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "../include/rectanglemodeview.h"
#include "../include/detail.h"

#include <QMouseEvent>

namespace {
    constexpr Qt::GlobalColor kDefaultRectFillColor{Qt::yellow};
    constexpr Qt::GlobalColor kDefaultRectStrokeColor{Qt::black};
}

RectangleModeView::RectangleModeView(QGraphicsScene* scene)
    : QGraphicsView(scene),
      currentItem_(nullptr),
      startCursorPos_(detail::kZeroPointF),
      fillColor_(kDefaultRectFillColor),
      strokeColor_(kDefaultRectStrokeColor)
{
    setRenderHint(QPainter::Antialiasing);
}

void RectangleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(QRectF{startCursorPos_, detail::kZeroSizeF},
                                        QPen{strokeColor_},
                                        QBrush{fillColor_});

        if (currentItem_ != nullptr) detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void RectangleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (currentItem_ != nullptr && (event->buttons() & Qt::LeftButton) ) {
        QPointF currentCursorPos = mapToScene(event->pos());
        QRectF updatedRectangle = detail::updateRectangleSize(startCursorPos_, currentCursorPos);
        currentItem_->setRect(updatedRectangle.normalized());
        emit changeStateOfScene();
    }
}

void RectangleModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (detail::shouldDeleteZeroSizeItem(currentItem_, startCursorPos_)) {
            qDebug() << "deleting zero rectangle";
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
