// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "custom-graphics-view.h"

class RectangleModeView : public DrawingGraphicsView {
    Q_OBJECT

 public:
    explicit RectangleModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 signals:
    void changeStateOfScene();

 private:
    QGraphicsRectItem* currentItem_;
    QPointF startCursorPos_;
};
