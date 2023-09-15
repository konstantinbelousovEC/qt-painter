// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QGraphicsLineItem>
#include <QMouseEvent>
#include "../include/line-mode-view.h"
#include "../include/detail.h"
#include "../include/constants.h"

namespace {
    constexpr qreal kDefaultLineWidth{5.0};
}

LineModeView::LineModeView(QGraphicsScene *scene)
    : DrawingGraphicsView(scene, kDefaultLineWidth),
      currentItem_(nullptr),
      startCursorPos_(constants::kZeroPointF)
{
    setRenderHint(QPainter::Antialiasing);
}

void LineModeView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addLine(QLineF{startCursorPos_,
                                               startCursorPos_},
                                        QPen{strokeColor_,
                                             strokeWidth_,
                                             Qt::SolidLine,
                                             Qt::SquareCap,
                                             Qt::MiterJoin});

        if (currentItem_ != nullptr)
            detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void LineModeView::mouseMoveEvent(QMouseEvent *event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    emit cursorPositionChanged(currentCursorPos);
    if (currentItem_ != nullptr && (event->buttons() & Qt::LeftButton)) {
        currentItem_->setLine(QLineF{startCursorPos_, currentCursorPos});
        emit changeStateOfScene();
    }
}

void LineModeView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (currentItem_->line().p1() == currentItem_->line().p2()) {
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}
