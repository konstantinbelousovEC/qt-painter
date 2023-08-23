// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "trianglemode.h"
#include "detail.h"


TriangleModeView::TriangleModeView(QGraphicsScene* scene) {

}

void TriangleModeView::mousePressEvent(QMouseEvent* event) {

}

void TriangleModeView::mouseMoveEvent(QMouseEvent* event) {

}

void TriangleModeView::mouseReleaseEvent(QMouseEvent* event) {

}

void TriangleModeView::changeFillColor(const QColor& color) {
    fillColor_ = color;
}

void TriangleModeView::changeStrokeColor(const QColor& color) {
    strokeColor_ = color;
}
