#include "modificationmodeview.h"
#include "detail.h"

#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <QKeyEvent>
#include <math.h>

namespace {
    constexpr QColor kSelectionAreaBrush{0, 0, 200, 15};
    constexpr QRectF kZeroSizeRectangle{0, 0, 0, 0};
    constexpr QPointF kZeroPoint{0, 0};
}

template<typename RectLikeItem>
QPointF getItemCenter(const RectLikeItem* rectItem) noexcept {
    qDebug() << rectItem->rect().center();
    return rectItem->rect().center();
}

QPointF getTriangleCenter(const QGraphicsPolygonItem *triangleItem) {
    QPolygonF trianglePolygon = triangleItem->polygon();
    if (trianglePolygon.size() != 3) throw std::runtime_error("App supports only triangles, see you later");
    return (trianglePolygon.at(0) + trianglePolygon.at(1) + trianglePolygon.at(2)) / 3.0;
}

QPointF getGraphicsItemCenterPos(const QGraphicsItem* const item) {
    if (const auto* rectItem = qgraphicsitem_cast<const QGraphicsRectItem*>(item)) {
        return getItemCenter(rectItem);
    } else if (const auto* ellipseItem = qgraphicsitem_cast<const QGraphicsEllipseItem*>(item)) {
        return getItemCenter(ellipseItem);
    } else if (const auto* triangleItem = qgraphicsitem_cast<const QGraphicsPolygonItem*>(item)) {
        return getTriangleCenter(triangleItem);
    } else {
        throw std::runtime_error("Can not define figure's center: unknown figure type");
    }
}

inline QGraphicsItem* ModificationModeView::getItemUnderCursor(QPointF currentCursorPos) {
    return scene()->itemAt(currentCursorPos, QTransform{});
}

QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene);
QGraphicsItem* cloneGraphicsItem(const QGraphicsItem* originalItem);
void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items);
qreal calculateRotationAngle(const QPointF &O, const QPointF &A, const QPointF &B) noexcept;

template<typename ItemT, typename ItemU>
void copyGraphicProperties(const ItemT* originalItem, ItemU* destinationElement);

ModificationModeView::ModificationModeView(QGraphicsScene* graphic_scene)
    : QGraphicsView(graphic_scene),
      selectionArea_(new QGraphicsRectItem()),
      selectionStartPos_(kZeroPoint),
      lastClickPos_(kZeroPoint),
      rotationPointA_(kZeroPoint),
      isMoving_(false),
      isRotating_(false)
{
    setSelectionAreaProperties();
    scene()->addItem(selectionArea_);
}

void ModificationModeView::mousePressEvent(QMouseEvent* event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    QGraphicsItem* itemUnderCursor = getItemUnderCursor(currentCursorPos);

    if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) { // todo: копирование // Qt::MiddleButton
        handleMiddleButtonClick(itemUnderCursor, currentCursorPos);
    } else if (event->button() == Qt::LeftButton) { // todo: выделение
        handleLeftButtonClick(event, itemUnderCursor, currentCursorPos);
    } else if (event->button() == Qt::RightButton) { // todo: вращение
        handleRightButtonClick(event, itemUnderCursor);
    }
}

void ModificationModeView::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsView::mouseMoveEvent(event);

    QPointF mouseCurrentPos = mapToScene(event->pos());
    if (isRotating_) {
        rotateSelectedItems(event);
        emit changeStateOfScene();
    } else if (isMoving_) {
        if (event->buttons() & Qt::LeftButton) moveSelectedItems(mouseCurrentPos);
        emit changeStateOfScene();
    } else if (!isMoving_ && !isRotating_ && event->buttons() & Qt::LeftButton && !(event->modifiers() & Qt::ShiftModifier)) {
        updateSelectionArea(event, mouseCurrentPos);
        emit changeStateOfScene();
    }
}

void ModificationModeView::mouseReleaseEvent(QMouseEvent* event) {
    isMoving_ = false;
    isRotating_ = false;
    selectionArea_->setRect(kZeroSizeRectangle);
    selectionArea_->hide();
}

void ModificationModeView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) {
        detail::deleteSelectedItems(scene());
        emit changeStateOfScene();
    }
}

void ModificationModeView::handleLeftButtonClick(QMouseEvent* event, QGraphicsItem* itemUnderCursor, const QPointF& currentCursorPos) {
    if (itemUnderCursor == nullptr) {
        QGraphicsView::mousePressEvent(event);
        if (!(event->modifiers() & Qt::ControlModifier)) scene()->clearSelection();
        selectionStartPos_ = currentCursorPos;
    } else {
        if (event->modifiers() & Qt::ControlModifier) {
            itemUnderCursor->setSelected(!itemUnderCursor->isSelected());
        } else if (!itemUnderCursor->isSelected()) {
            scene()->clearSelection();
            itemUnderCursor->setSelected(true);
        }
        isMoving_ = true;
        lastClickPos_ = currentCursorPos;
    }
}

void ModificationModeView::handleMiddleButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentCursorPos) {
    if (itemUnderCursor != nullptr) {
        QList<QGraphicsItem*> clonedItems = cloneSelectedItems(scene());
        updateSceneSelection(scene(), clonedItems);
        isMoving_ = true;
        lastClickPos_ = currentCursorPos;
    }
}

void ModificationModeView::handleRightButtonClick(QMouseEvent* event, QGraphicsItem* itemUnderCursor) {
    if (itemUnderCursor != nullptr) {
        isRotating_ = true;
        rotationPointA_ = mapToScene(event->pos());
    } else {
        scene()->clearSelection();
    }
}

void ModificationModeView::updateSelectionArea(QMouseEvent* event, const QPointF& mouseCurrentPos) {
    selectionArea_->show();
    QRectF updatedRectangle = detail::updateRectangleSize(selectionStartPos_, mouseCurrentPos);
    selectionArea_->setRect(updatedRectangle.normalized());
    updateItemsSelection(event, selectionArea_->rect());
}

void ModificationModeView::updateItemsSelection(QMouseEvent* event, const QRectF &rect) {
    QList<QGraphicsItem*> itemsInRect = scene()->items(rect);
    foreach (QGraphicsItem *item, scene()->items()) {
        if (!(event->modifiers() & Qt::ControlModifier)) {
            item->setSelected(itemsInRect.contains(item));
        } else {
            item->setSelected(item->isSelected() || itemsInRect.contains(item));
        }
    }
}

void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items) {
    scene->clearSelection();
    foreach (QGraphicsItem* item, items) {
        item->setSelected(true);
    }
}

qreal calculateRotationAngle(const QPointF &O, const QPointF &A, const QPointF &B) noexcept {
    qreal angleAO = qAtan2(A.y() - O.y(), A.x() - O.x());
    qreal angleBO = qAtan2(B.y() - O.y(), B.x() - O.x());
    qreal angle = angleBO - angleAO;

    if (angle < 0) angle += 2 * M_PI;

    return qRadiansToDegrees(angle);
}

template<typename ItemT, typename ItemU>
void copyGraphicProperties(const ItemT* originalItem, ItemU* destinationElement) {
    destinationElement->setPen(originalItem->pen());
    destinationElement->setBrush(originalItem->brush());
}

QGraphicsItem* cloneGraphicsItem(QGraphicsItem* originalItem) {
    QGraphicsItem* copiedItem = nullptr;

    if (auto* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(originalItem)) {
        auto rotation = rectItem->rotation();
        rectItem->setRotation(0);
        auto* temporaryRectItem = new QGraphicsRectItem(rectItem->sceneBoundingRect());
        temporaryRectItem->setTransformOriginPoint(getGraphicsItemCenterPos(temporaryRectItem));
        temporaryRectItem->setRotation(rotation);
        rectItem->setRotation(rotation);
        copyGraphicProperties(rectItem, temporaryRectItem);
        copiedItem = temporaryRectItem;
    } else if (const auto* ellipseItem = qgraphicsitem_cast<const QGraphicsEllipseItem*>(originalItem)) {
        auto* temporaryEllipseItem = new QGraphicsEllipseItem(ellipseItem->rect());
        copyGraphicProperties(ellipseItem, temporaryEllipseItem);
        copiedItem = temporaryEllipseItem;
    } else if (const auto* polygonItem = qgraphicsitem_cast<const QGraphicsPolygonItem*>(originalItem)) {
//        auto* temporaryPolygonItem = new QGraphicsPolygonItem(polygonItem->polygon());
//        temporaryPolygonItem->setPen(polygonItem->pen());
//        temporaryPolygonItem->setBrush(polygonItem->brush());
//        copiedItem = temporaryPolygonItem;
    }

    if (copiedItem != nullptr) detail::makeItemSelectableAndMovable(copiedItem);

    return copiedItem;
}

QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene) {
    QList<QGraphicsItem*> clonedItems;
    foreach (QGraphicsItem* item, scene->selectedItems()) {
        QGraphicsItem* clonedItem = cloneGraphicsItem(item);
        if (clonedItem) {
            clonedItems.append(clonedItem);
            scene->addItem(clonedItem);
        }
    }
    return clonedItems;
}

void ModificationModeView::moveSelectedItems(const QPointF& mousePos) {
    QPointF delta = mousePos - lastClickPos_;
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        item->moveBy(delta.x(), delta.y());
    }
    lastClickPos_ = mousePos;
}

void ModificationModeView::rotateSelectedItems(QMouseEvent* event) {
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        rotateItem(event, item);
    }
}

void ModificationModeView::rotateItem(QMouseEvent *event, QGraphicsItem* item) {
    QPointF pointO = getGraphicsItemCenterPos(item);
    QPointF pointB = mapToScene(event->pos());

    item->setTransformOriginPoint(pointO);
    qreal angle = calculateRotationAngle(pointO, rotationPointA_, pointB);

    item->setRotation(angle);
}

void ModificationModeView::setSelectionAreaProperties() {
    selectionArea_->setPen(QPen{Qt::blue});
    selectionArea_->setBrush(QBrush{kSelectionAreaBrush});
    selectionArea_->hide();
    selectionArea_->setFlag(QGraphicsItem::ItemIsSelectable, false);
    selectionArea_->setFlag(QGraphicsItem::ItemIsMovable, false);
    selectionArea_->setZValue(1.0);
}
