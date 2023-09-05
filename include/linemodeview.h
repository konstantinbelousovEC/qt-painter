// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class LineModeView : public QGraphicsView {
 Q_OBJECT

 public:
    explicit LineModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 private slots:
    void changeLineColor(const QColor& color);
    void changeLineWidth(int width);

 signals:
    void changeStateOfScene();

 private:
    QGraphicsLineItem* currentItem_;
    QPointF startCursorPos_;
    QColor lineColor_;
    int lineWidth_;
};