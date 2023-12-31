// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class ApplicationGraphicsView : public QGraphicsView {
    Q_OBJECT

 public:
    ApplicationGraphicsView(QGraphicsScene* scene, QSize viewSize);
    ~ApplicationGraphicsView() override = default;

 protected:
    bool event(QEvent* event) override;

 signals:
    void cursorHasLeavedView();
    void changeStateOfScene();
    void cursorPositionChanged(QPointF position);

 protected:
    QSize viewSize_;
};
