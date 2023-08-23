#include "modificationmodeview.h"
#include "detail.h"

#include <QApplication>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>

namespace {
    constexpr QColor kSelectionAreaBrush{0, 0, 200, 15};
    constexpr QColor kSelectionAreaPen{0 , 0, 255};
    constexpr QRectF kZeroSizeFRectangle{0, 0, 0, 0};
    constexpr qreal kSelectionAreaZValue{1.0};
    constexpr qreal kZeroAngle{0};
}

template<typename ItemT, typename ItemU>
void copyGraphicProperties(const ItemT* originalItem, ItemU* destinationElement);

template<typename RectLikeItem>
QPointF getItemCenter(const RectLikeItem* rectItem) noexcept;

QPointF getTriangleCenter(const QGraphicsPolygonItem* triangleItem);
QPointF getGraphicsItemCenterPos(const QGraphicsItem* item);
QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene);
QGraphicsItem* cloneGraphicsItem(QGraphicsItem* originalItem);
qreal calculateRotationAngle(const QPointF& O, const QPointF& A, const QPointF& B) noexcept;
inline QGraphicsItem* getItemUnderCursor(const QGraphicsScene* scene, const QPointF& currentCursorPos);
void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items);

// ------------------------------------------------------------------------------------------------------------

ModificationModeView::ModificationModeView(QGraphicsScene* graphic_scene)
    : QGraphicsView(graphic_scene),
      selectionArea_(new QGraphicsRectItem()),
      selectionStartPos_(detail::kZeroPointF),
      lastClickPos_(detail::kZeroPointF),
      rotationPointA_(detail::kZeroPointF),
      isMoving_(false),
      isRotating_(false)
{
    setSelectionAreaProperties();
    scene()->addItem(selectionArea_);
}

void ModificationModeView::mousePressEvent(QMouseEvent* event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    QGraphicsItem* itemUnderCursor = getItemUnderCursor(scene(), currentCursorPos);

    if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) { // todo: Qt::MiddleButton
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
    selectionArea_->setRect(kZeroSizeFRectangle);
    selectionArea_->hide();
}

void ModificationModeView::wheelEvent(QWheelEvent* event) {
    // todo: обработка скроллинга
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

void ModificationModeView::updateItemsSelection(QMouseEvent* event, const QRectF &selectionRectangle) {
    QList<QGraphicsItem*> itemsInRect = scene()->items(selectionRectangle);
    foreach (QGraphicsItem *item, scene()->items()) {
        if (!(event->modifiers() & Qt::ControlModifier)) {
            item->setSelected(itemsInRect.contains(item));
        } else {
            item->setSelected(item->isSelected() || itemsInRect.contains(item));
        }
    }
}

void ModificationModeView::moveSelectedItems(const QPointF& mouseCurrentPos) {
    QPointF delta = mouseCurrentPos - lastClickPos_;
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        item->moveBy(delta.x(), delta.y());
    }
    lastClickPos_ = mouseCurrentPos;
}

void ModificationModeView::rotateSelectedItems(QMouseEvent* event) {
            foreach (QGraphicsItem* item, scene()->selectedItems()) {
            rotateItem(event, item);
        }
}

void ModificationModeView::rotateItem(QMouseEvent *event, QGraphicsItem* item) {
    QPointF pointO = getGraphicsItemCenterPos(item);
    QPointF pointB = mapToScene(event->pos());
    qreal rotationAngle = calculateRotationAngle(pointO, rotationPointA_, pointB);
    item->setTransformOriginPoint(pointO);
    item->setRotation(rotationAngle);
}

void ModificationModeView::setSelectionAreaProperties() {
    selectionArea_->setPen(QPen{kSelectionAreaPen});
    selectionArea_->setBrush(QBrush{kSelectionAreaBrush});
    selectionArea_->hide();
    selectionArea_->setFlag(QGraphicsItem::ItemIsSelectable, false);
    selectionArea_->setFlag(QGraphicsItem::ItemIsMovable, false);
    selectionArea_->setZValue(kSelectionAreaZValue);
}

// ------------------------------------------------------------------------------------------------------------

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

enum class GraphicItemType {
    kRect,
    kCircle,
    kTriangle
};

template<GraphicItemType type, typename ItemType>
auto copyGraphicsItem(ItemType originalItem) {
    auto rotationAngle = originalItem->rotation();
    originalItem->setRotation(kZeroAngle);
    ItemType temporaryRectItem;
    if constexpr (type == GraphicItemType::kRect) {
        temporaryRectItem = new QGraphicsRectItem(originalItem->sceneBoundingRect());
    } else if constexpr (type == GraphicItemType::kCircle) {
        temporaryRectItem = new QGraphicsEllipseItem(originalItem->sceneBoundingRect());
    }
    temporaryRectItem->setTransformOriginPoint(getGraphicsItemCenterPos(temporaryRectItem));
    temporaryRectItem->setRotation(rotationAngle);
    originalItem->setRotation(rotationAngle);
    copyGraphicProperties(originalItem, temporaryRectItem);
    return temporaryRectItem;
}

QGraphicsItem* cloneGraphicsItem(QGraphicsItem* originalItem) {
    QGraphicsItem* copiedItem = nullptr;

    if (auto* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(originalItem)) {
        copiedItem = copyGraphicsItem<GraphicItemType::kRect>(rectItem);
    } else if (auto* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(originalItem)) {
        copiedItem = copyGraphicsItem<GraphicItemType::kCircle>(ellipseItem);
    } else if (auto* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(originalItem)) {
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

QPointF getGraphicsItemCenterPos(const QGraphicsItem* item) {
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

inline QGraphicsItem* getItemUnderCursor(const QGraphicsScene* scene, const QPointF& currentCursorPos) {
    return scene->itemAt(currentCursorPos, QTransform{});
}
