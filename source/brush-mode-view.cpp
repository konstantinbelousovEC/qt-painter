// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QPainterPath>
#include "../include/brush-mode-view.h"
#include "../include/detail.h"
#include "../include/constants.h"

namespace {
    constexpr qreal kDefaultBrushWidth{10.0};
}

BrushModeView::BrushModeView(QGraphicsScene* scene)
        : DrawingGraphicsView(scene, kDefaultBrushWidth),
          startEllipseItem_(nullptr),
          startCursorPos_(constants::kZeroPointF),
          previousCursorPos_(constants::kZeroPointF)
{
    setRenderHint(QPainter::Antialiasing);
}

void BrushModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        startEllipseItem_ = scene()->addEllipse(startCursorPos_.x() - strokeWidth_ / 2.0,
                                                startCursorPos_.y() - strokeWidth_ / 2.0,
                                                strokeWidth_,
                                                strokeWidth_,
                                                QPen{Qt::NoPen},
                                                QBrush{strokeColor_});

        if (startEllipseItem_ != nullptr) {
            detail::makeItemSelectableAndMovable(startEllipseItem_);
            emit changeStateOfScene();
        }

        previousCursorPos_ = startCursorPos_;
    }
}

void BrushModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    emit cursorPositionChanged(currentCursorPos);
    if (startEllipseItem_ != nullptr && event->buttons() & Qt::LeftButton) {
        auto* currentTemporaryLine = scene()->addLine(previousCursorPos_.x(),
                                                      previousCursorPos_.y(),
                                                      currentCursorPos.x(),
                                                      currentCursorPos.y(),
                                                      QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::RoundCap});

        if (currentTemporaryLine != nullptr) {
            temporaryLines_.push_back(currentTemporaryLine);
            previousCursorPos_ = currentCursorPos;
        }
        emit changeStateOfScene();
    }
}

void BrushModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (startEllipseItem_ != nullptr && event->button() == Qt::LeftButton) {
        if (!temporaryLines_.empty()) {
            QPainterPath path;
            path.moveTo(startCursorPos_);

            for (auto* line : temporaryLines_) {
                path.moveTo(line->line().p1());
                path.lineTo(line->line().p2());
                detail::deleteItem(scene(), line);
            }
            temporaryLines_.clear();

            auto* scenePath = scene()->addPath(path, QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::RoundCap});
            if (scenePath != nullptr) detail::makeItemSelectableAndMovable(scenePath);

            detail::deleteItem(scene(), startEllipseItem_);
            startEllipseItem_ = nullptr;
        }
    }
}
