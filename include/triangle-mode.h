// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "polygon-interface.h"

class TriangleModeView : public Polygon {
 public:
    explicit TriangleModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};
