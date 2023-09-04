// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QApplication>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <qmath.h>
#include <stdexcept>
#include "../include/modificationmodeview.h"
#include "../include/detail.h"

namespace {
    const QColor kSelectionAreaBrush{0, 0, 200, 15};
    const QColor kSelectionAreaPen{0 , 0, 255};
    constexpr QRectF kZeroSizeFRectangle{0, 0, 0, 0};
    constexpr qreal kSelectionAreaZValue{1.0};
    constexpr qreal kZeroAngle{0};

    enum class GraphicItemType {
        kRect,
        kCircle,
        kTriangle,
        kPath
    };
}  // namespace

template<typename ItemT, typename ItemU>
void copyGraphicProperties(const ItemT* originalItem, ItemU* destinationElement);

template<GraphicItemType type, typename ItemType>
auto copyGraphicsItem(ItemType* originalItem);

QPolygonF copyPolygon(const QGraphicsPolygonItem* originalItem);
QPointF getPolygonSceneCenter(const QGraphicsPolygonItem* triangleItem);
QPointF getPolygonOwnCenter(const QGraphicsPolygonItem* triangleItem);
QPointF getGraphicsItemSceneCenterPos(const QGraphicsItem* item);
QPointF getGraphicsItemOwnCenterPos(const QGraphicsItem* item);
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
    QGraphicsItem* itemUnderCursor =
            getItemUnderCursor(scene(), currentCursorPos);

    if (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier) {
        handleMiddleButtonClick(itemUnderCursor, currentCursorPos);
    } else if (event->button() == Qt::LeftButton) {
        handleLeftButtonClick(event, itemUnderCursor, currentCursorPos);
    } else if (event->button() == Qt::RightButton) {
        handleRightButtonClick(event, itemUnderCursor);
    }
}

inline bool isSelectionEvent(const QMouseEvent* event, bool isMoving, bool isRotating) noexcept {
    if (event == nullptr) return false;
    return !isMoving &&
           !isRotating &&
           event->buttons() & Qt::LeftButton &&
           !(event->modifiers() & Qt::ShiftModifier);
}

void ModificationModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF mouseCurrentPos = mapToScene(event->pos());
    if (isRotating_) {
        if (rotationInfo_.isEmpty()) rotationInfo_.fillInfo(scene()->selectedItems());
        rotateSelectedItems(event);
        emit changeStateOfScene();
    } else if (isMoving_) {
        if (event->buttons() & Qt::LeftButton) moveSelectedItems(mouseCurrentPos);
        emit changeStateOfScene();
    } else if (isSelectionEvent(event, isMoving_, isRotating_)) {
        updateSelectionArea(event, mouseCurrentPos);
    }
}

void ModificationModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (isRotating_) rotationInfo_.clear();
    isMoving_ = false;
    isRotating_ = false;
    selectionArea_->setRect(kZeroSizeFRectangle);
    selectionArea_->hide();
}

void ModificationModeView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) {
        detail::deleteSelectedItems(scene());
        emit changeStateOfScene();
    }
}

void ModificationModeView::handleLeftButtonClick(QMouseEvent* event,
                                                 QGraphicsItem* itemUnderCursor,
                                                 const QPointF& currentCursorPos) {
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

void ModificationModeView::handleMiddleButtonClick(QGraphicsItem* itemUnderCursor,
                                                   const QPointF& currentCursorPos) {
    if (itemUnderCursor != nullptr) {
        QList<QGraphicsItem*> clonedItems = cloneSelectedItems(scene());
        updateSceneSelection(scene(), clonedItems);
        isMoving_ = true;
        lastClickPos_ = currentCursorPos;
    }
}

void ModificationModeView::handleRightButtonClick(QMouseEvent* event,
                                                  QGraphicsItem* itemUnderCursor) {
    if (itemUnderCursor != nullptr) {
        isRotating_ = true;
        rotationPointA_ = mapToScene(event->pos());
    } else {
        scene()->clearSelection();
    }
}

void ModificationModeView::updateSelectionArea(QMouseEvent* event,
                                               const QPointF& mouseCurrentPos) {
    selectionArea_->show();
    QRectF updatedRectangle =
            detail::updateRectangleSize(selectionStartPos_, mouseCurrentPos);

    selectionArea_->setRect(updatedRectangle.normalized());
    updateItemsSelection(event, selectionArea_->rect());
}

void ModificationModeView::updateItemsSelection(QMouseEvent* event,
                                                const QRectF &selectionRectangle) {
    QList<QGraphicsItem*> itemsInRect = scene()->items(selectionRectangle);
    foreach(QGraphicsItem *item, scene()->items()) {
        if (!(event->modifiers() & Qt::ControlModifier)) {
            item->setSelected(itemsInRect.contains(item));
        } else {
            item->setSelected(item->isSelected() || itemsInRect.contains(item));
        }
    }
}

void ModificationModeView::moveSelectedItems(const QPointF& mouseCurrentPos) {
    QPointF delta = mouseCurrentPos - lastClickPos_;
    foreach(QGraphicsItem* item, scene()->selectedItems()) {
        item->moveBy(delta.x(), delta.y());
    }
    lastClickPos_ = mouseCurrentPos;
}

void ModificationModeView::rotateSelectedItems(QMouseEvent* event) {
    const auto& items = rotationInfo_.getItems();
    const auto& angles = rotationInfo_.getAngles();
    for (int i = 0; i < rotationInfo_.size(); i++) {
        rotateItem(event, items[i], angles[i]);
    }
}

void ModificationModeView::rotateItem(QMouseEvent *event, QGraphicsItem* item, qreal startAngle) {
    QPointF pointO = getGraphicsItemSceneCenterPos(item);
    QPointF pointB = mapToScene(event->pos());
    qreal rotationAngle = calculateRotationAngle(pointO,
                                                 rotationPointA_,
                                                 pointB);

    item->setTransformOriginPoint(getGraphicsItemOwnCenterPos(item));
    item->setRotation(startAngle + rotationAngle);
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

QPointF getPolygonSceneCenter(const QGraphicsPolygonItem* triangleItem) { // todo: make a template for both center calculation functions
    auto points = triangleItem->polygon().toVector();
    QPointF sum{};
    foreach(const auto& point, points) {
        sum += triangleItem->mapToScene(point);
    }
    return sum / static_cast<qreal>(points.size());
}

QPointF getPolygonOwnCenter(const QGraphicsPolygonItem* triangleItem) {
    auto points = triangleItem->polygon().toVector();
    QPointF sum{};
    foreach(const auto& point, points) {
        sum += point;
    }
    return sum / static_cast<qreal>(points.size());
}

QPointF getGraphicsItemSceneCenterPos(const QGraphicsItem* item) {
    if (const auto* rectItem = qgraphicsitem_cast<const QGraphicsRectItem*>(item)) {
        return rectItem->sceneBoundingRect().center();
    } else if (const auto* ellipseItem = qgraphicsitem_cast<const QGraphicsEllipseItem*>(item)) {
        return ellipseItem->sceneBoundingRect().center();
    } else if (const auto* triangleItem = qgraphicsitem_cast<const QGraphicsPolygonItem*>(item)) {
        return getPolygonSceneCenter(triangleItem);
    } else if (const auto* pathItem = qgraphicsitem_cast<const QGraphicsPathItem*>(item)) {
        return pathItem->sceneBoundingRect().center();
    } else {
        throw std::runtime_error("Can not define figure's center: unknown figure type");
    }
}

QPointF getGraphicsItemOwnCenterPos(const QGraphicsItem* item) {
    if (const auto* rectItem = qgraphicsitem_cast<const QGraphicsRectItem*>(item)) {
        return rectItem->boundingRect().center();
    } else if (const auto* ellipseItem = qgraphicsitem_cast<const QGraphicsEllipseItem*>(item)) {
        return ellipseItem->boundingRect().center();
    } else if (const auto* triangleItem = qgraphicsitem_cast<const QGraphicsPolygonItem*>(item)) {
        return getPolygonOwnCenter(triangleItem);
    } else if (const auto* pathItem = qgraphicsitem_cast<const QGraphicsPathItem*>(item)) {
        return pathItem->boundingRect().center();
    } else {
        throw std::runtime_error("Can not define figure's center: unknown figure type");
    }
}

template<typename ItemT, typename ItemU>
void copyGraphicProperties(const ItemT* originalItem, ItemU* destinationElement) {
    destinationElement->setPen(originalItem->pen());
    destinationElement->setBrush(originalItem->brush());
}

template<GraphicItemType type, typename ItemType>
auto copyGraphicsItem(ItemType* originalItem) {
    auto rotationAngle = originalItem->rotation();
    originalItem->setRotation(kZeroAngle);
    ItemType* temporaryItem{nullptr};

    if constexpr (type == GraphicItemType::kRect) {
        temporaryItem = new QGraphicsRectItem(originalItem->sceneBoundingRect());
    } else if constexpr (type == GraphicItemType::kCircle) {
        temporaryItem = new QGraphicsEllipseItem(originalItem->sceneBoundingRect());
    } else if constexpr (type == GraphicItemType::kTriangle) {
        temporaryItem = new QGraphicsPolygonItem(originalItem->mapToScene(originalItem->polygon()));
    } else if constexpr (type == GraphicItemType::kPath) {
        temporaryItem = new QGraphicsPathItem(originalItem->mapToScene(originalItem->path()));
    }

    if (temporaryItem != nullptr) {
        temporaryItem->setTransformOriginPoint(getGraphicsItemSceneCenterPos(temporaryItem));
        temporaryItem->setRotation(rotationAngle);
        originalItem->setRotation(rotationAngle);
        copyGraphicProperties(originalItem, temporaryItem);
    }
    return temporaryItem;
}

QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene) {
    QList<QGraphicsItem*> clonedItems;
    foreach(QGraphicsItem* item, scene->selectedItems()) {
        QGraphicsItem* clonedItem = cloneGraphicsItem(item);
        if (clonedItem) {
            clonedItems.append(clonedItem);
            scene->addItem(clonedItem);
        }
    }
    return clonedItems;
}

QGraphicsItem* cloneGraphicsItem(QGraphicsItem* originalItem) {
    QGraphicsItem* copiedItem = nullptr;

    if (auto* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(originalItem)) {
        copiedItem = copyGraphicsItem<GraphicItemType::kRect>(rectItem);
    } else if (auto* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(originalItem)) {
        copiedItem = copyGraphicsItem<GraphicItemType::kCircle>(ellipseItem);
    } else if (auto* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(originalItem)) {
        copiedItem = copyGraphicsItem<GraphicItemType::kTriangle>(polygonItem);
    } else if (auto* pathItem = qgraphicsitem_cast<QGraphicsPathItem*>(originalItem)) {
        copiedItem = copyGraphicsItem<GraphicItemType::kPath>(pathItem);
    }

    if (copiedItem != nullptr)
        detail::makeItemSelectableAndMovable(copiedItem);

    return copiedItem;
}

qreal calculateRotationAngle(const QPointF& O,
                             const QPointF& A,
                             const QPointF& B) noexcept {
    qreal angleAO = qAtan2(A.y() - O.y(), A.x() - O.x());
    qreal angleBO = qAtan2(B.y() - O.y(), B.x() - O.x());
    qreal angle = angleBO - angleAO;

    if (angle < 0) angle += 2 * M_PI;

    return qRadiansToDegrees(angle);
}

inline QGraphicsItem* getItemUnderCursor(const QGraphicsScene* scene, const QPointF& currentCursorPos) {
    return scene->itemAt(currentCursorPos, QTransform{});
}

void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items) {
    scene->clearSelection();
    foreach(QGraphicsItem* item, items) {
        item->setSelected(true);
    }
}

void ModificationModeView::RotationInfo::clear() {
    items_.clear();
    angles_.clear();
}

qsizetype ModificationModeView::RotationInfo::size() const noexcept {
    return items_.size();
}

bool ModificationModeView::RotationInfo::isEmpty() const noexcept {
    return items_.isEmpty();
}

bool ModificationModeView::RotationInfo::fillInfo(const QList<QGraphicsItem *> &items) {
    items_ = items;
    angles_.reserve(items_.size());
    foreach(const auto& item, items_) {
        angles_.push_back(item->rotation());
    }
    return items_.size() == angles_.size();
}

const QList<QGraphicsItem*>& ModificationModeView::RotationInfo::getItems() const noexcept {
    return items_;
}

const QList<qreal>& ModificationModeView::RotationInfo::getAngles() const noexcept {
    return angles_;
}
