// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "custom-graphics-view.h"

class BrushModeView : public CustomGraphicsView {
    Q_OBJECT

 public:
    explicit BrushModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 signals:
    void changeStateOfScene();

 private:
    QList<QGraphicsLineItem*> temporaryLines_;
    QGraphicsEllipseItem* startEllipseItem_;
    QPointF startCursorPos_;
    QPointF previousCursorPos_;
};
