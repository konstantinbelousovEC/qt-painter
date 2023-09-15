// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class ModificationModeView : public QGraphicsView {
    Q_OBJECT

 public:
    explicit ModificationModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

 signals:
    void changeStateOfScene();

 private:
    void setSelectionAreaProperties();
    void updateItemsSelection(QMouseEvent* event, const QRectF& rect);
    void moveSelectedItems(const QPointF& mousePos);
    void rotateSelectedItems(QMouseEvent* event);
    void rotateItem(QMouseEvent *event, QGraphicsItem* item, qreal startAngle);
    void updateSelectionArea(QMouseEvent* event, const QPointF& mouseCurrentPos);
    void handleMiddleButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentCursorPos);
    void handleRightButtonClick(QMouseEvent* event, QGraphicsItem* itemUnderCursor);
    void handleLeftButtonClick(QMouseEvent* event,
                               QGraphicsItem* itemUnderCursor,
                               const QPointF& currentCursorPos);

 private:
    /*
     An auxiliary class for performing rotation of graphic elements,
     storing selected scene elements and their corresponding initial rotation angles.
     If you do not store this information, then each new rotation begins without taking into account the previous one,
     which leads to a visual "jump" of the figure and does not meet the requirements of the application functionality.

     This information is filled in when the right mouse button is pressed and cleared when the right mouse button is released.
    */
    class RotationInfo {
     public:
        void clear();
        bool fillInfo(const QList<QGraphicsItem*>& items);
        [[nodiscard]] qsizetype size() const noexcept;
        [[nodiscard]] bool isEmpty() const noexcept;
        [[nodiscard]] const QList<QGraphicsItem*>& getItems() const noexcept;
        [[nodiscard]] const QList<qreal>& getAngles() const noexcept;

     private:
        QList<QGraphicsItem*> items_;
        QList<qreal> angles_;
    };

 private:
    RotationInfo rotationInfo_;
    QGraphicsRectItem* selectionArea_;
    QPointF selectionStartPos_;
    QPointF lastClickPos_;
    QPointF initialCursorPosA_;
    bool isMoving_;
};
