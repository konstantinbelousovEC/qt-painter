// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "custom-graphics-view.h"

class LineModeView : public CustomGraphicsView {
 Q_OBJECT

 public:
    explicit LineModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 signals:
    void changeStateOfScene();

 private:
    QGraphicsLineItem* currentItem_;
    QPointF startCursorPos_;
};