// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "polygon-interface.h"

class TriangleModeView : public Polygon {
    Q_OBJECT

 public:
    explicit TriangleModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

 signals:
    void changeStateOfScene();
};
