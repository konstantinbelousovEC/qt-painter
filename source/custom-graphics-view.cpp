// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "../include/custom-graphics-view.h"

DrawingGraphicsView::DrawingGraphicsView(QGraphicsScene* scene,
                                       std::optional<QColor> fillColor,
                                       std::optional<QColor> strokeColor,
                                       int strokeWidth)
    : QGraphicsView(scene),
      fillColor_(fillColor),
      strokeColor_(strokeColor),
      strokeWidth_(strokeWidth) {}

DrawingGraphicsView::~DrawingGraphicsView() = default;

QColor DrawingGraphicsView::getFillColor() const noexcept {
    if (fillColor_.has_value()) return fillColor_.value();
    else return QColor{qRgba(0,0,0,0)};
}

QColor DrawingGraphicsView::getStrokeColor() const noexcept {
    if (strokeColor_.has_value()) return strokeColor_.value();
    else return QColor{qRgba(0,0,0,0)};
}

int DrawingGraphicsView::getStrokeWidth() const noexcept {
    return strokeWidth_;
}

void DrawingGraphicsView::setFillColor(const QColor& color) {
    fillColor_ = color;
}

void DrawingGraphicsView::setStrokeColor(const QColor& color) {
    strokeColor_ = color;
}

void DrawingGraphicsView::setStrokeWidth(int width) {
    assert(width >= 0); // this condition must be guaranteed by QSpinBox constraints of minimum value;
    strokeWidth_ = width;
}
