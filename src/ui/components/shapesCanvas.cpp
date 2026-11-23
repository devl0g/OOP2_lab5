#include "shapesCanvas.hpp"
#include <QMouseEvent>
#include <QPaintEvent>
#include <cstdint>

void ShapesCanvas::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);
  painter.fillRect(rect(), QColor(255, 255, 255));

  for (uint64_t shapeIndex = 0; shapeIndex < shapesCount; ++shapeIndex) {
    canvasShapes[shapeIndex].shape->renderShape(painter);
  }

  if (selectedIndex >= 0 && static_cast<uint64_t>(selectedIndex) < shapesCount) {
    canvasShapes[selectedIndex].shape->renderPhantomShape(painter);
  }

  if (drawingShape) {
    drawingShape->renderPhantomShape(painter);
  }
}

void ShapesCanvas::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    const auto position = event->pos();

    mouseState.isDragging = true;
    mouseState.dragStartX = position.x();
    mouseState.dragStartY = position.y();
    mouseState.dragEndX = position.x();
    mouseState.dragEndY = position.y();

    if (currentShapeType) {
      drawingShape = currentShapeType->createInstance();
      drawingShape->updateShapeConfig({ mouseState.dragStartX, mouseState.dragStartY }, { mouseState.dragEndX, mouseState.dragEndY });
    }
  }
}

void ShapesCanvas::mouseMoveEvent(QMouseEvent* event) {
  const auto position = event->pos();

  mouseState.dragEndX = position.x();
  mouseState.dragEndY = position.y();

  if (drawingShape) {
    drawingShape->updateShapeConfig({ mouseState.dragStartX, mouseState.dragStartY }, { mouseState.dragEndX, mouseState.dragEndY });
    update();
  }
}

void ShapesCanvas::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::MouseButton::LeftButton && mouseState.isDragging) {
    mouseState.isDragging = false;

    const auto position = event->pos();
    mouseState.dragEndX = position.x();
    mouseState.dragEndY = position.y();

    if (drawingShape) {
      const Position start { mouseState.dragStartX, mouseState.dragStartY };
      const Position end { mouseState.dragEndX, mouseState.dragEndY };
      drawingShape->updateShapeConfig(start, end);
      addShape(std::move(drawingShape), start, end);
      drawingShape.reset();
    }

    update();
  }
}

bool ShapesCanvas::addShape(std::unique_ptr<Shape> shape, Position start, Position end) {
  if (!shape || shapesCount >= MAX_CANVAS_SHAPES) {
    return false;
  }

  canvasShapes[shapesCount].shape = std::move(shape);
  canvasShapes[shapesCount].start = start;
  canvasShapes[shapesCount].end = end;
  ++shapesCount;

  emit shapesChanged();
  update();
  return true;
}

void ShapesCanvas::removeShape(uint64_t index) {
  if (index >= shapesCount) {
    return;
  }

  for (uint64_t i = index; i + 1 < shapesCount; ++i) {
    canvasShapes[i] = std::move(canvasShapes[i + 1]);
  }
  canvasShapes[shapesCount - 1].shape.reset();
  --shapesCount;
  selectedIndex = -1;

  emit shapesChanged();
  update();
}

void ShapesCanvas::clearShapes() {
  for (uint64_t i = 0; i < shapesCount; ++i) {
    canvasShapes[i].shape.reset();
  }
  shapesCount = 0;
  selectedIndex = -1;

  emit shapesChanged();
  update();
}

void ShapesCanvas::setSelectedIndex(int64_t index) {
  selectedIndex = index;
  update();
}
