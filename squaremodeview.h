// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class SquareModeView : public QGraphicsView {
    Q_OBJECT

public:
    SquareModeView(QGraphicsScene* scene);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void changeFillColor(const QColor& color);
    void changeStrokeColor(const QColor& color);

signals:
    void changeStateOfScene();

private:
    QGraphicsRectItem* currentItem_;
    QPointF centerPos_;
    QColor fillColor_;
    QColor strokeColor_;

};
