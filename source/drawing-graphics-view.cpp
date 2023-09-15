// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "../include/drawing-graphics-view.h"

DrawingGraphicsView::DrawingGraphicsView(QGraphicsScene* scene,
                                       QColor fillColor,
                                       QColor strokeColor,
                                       int strokeWidth)
    : QGraphicsView(scene),
      fillColor_(fillColor),
      strokeColor_(strokeColor),
      strokeWidth_(strokeWidth) {}

DrawingGraphicsView::~DrawingGraphicsView() = default;

QColor DrawingGraphicsView::getFillColor() const noexcept {
    if (fillColor_.isValid()) return fillColor_.value();
    else return QColor{qRgba(0,0,0,0)};
}

QColor DrawingGraphicsView::getStrokeColor() const noexcept {
    if (strokeColor_.isValid()) return strokeColor_;
    else return QColor{qRgba(0,0,0,0)};
}

int DrawingGraphicsView::getStrokeWidth() const noexcept {
    return static_cast<int>(strokeWidth_);
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
