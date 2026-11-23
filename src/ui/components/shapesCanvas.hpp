#ifndef INCLUDE_UI_COMPONENTS_SHAPES_CANVAS_HPP_
#define INCLUDE_UI_COMPONENTS_SHAPES_CANVAS_HPP_

#include "../../config.hpp"
#include "../../models/shapes/shape.hpp"
#include <QWidget>
#include <cstdint>
#include <memory>

struct MouseState {
  bool isDragging { false };
  int64_t dragStartX { -1 };
  int64_t dragStartY { -1 };
  int64_t dragEndX { -1 };
  int64_t dragEndY { -1 };
};

struct ShapeRecord {
  std::unique_ptr<Shape> shape { nullptr };
  Position start {};
  Position end {};
};

class ShapesCanvas : public QWidget {
    Q_OBJECT

public:
    explicit ShapesCanvas(QWidget* parent = nullptr) noexcept : QWidget(parent) {};
    ~ShapesCanvas() noexcept = default;
    ShapesCanvas(const ShapesCanvas& other) = delete;
    ShapesCanvas& operator=(ShapesCanvas other) = delete;

    void setDrawingShape(std::unique_ptr<Shape> shape) {
      currentShapeType = std::move(shape);
    }

    [[nodiscard]] uint64_t getShapesCount() const noexcept { return shapesCount; }
    [[nodiscard]] const ShapeRecord& getRecord(uint64_t index) const { return canvasShapes[index]; }

    bool addShape(std::unique_ptr<Shape> shape, Position start, Position end);
    void removeShape(uint64_t index);
    void clearShapes();
    void setSelectedIndex(int64_t index);

signals:
    void shapesChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    std::unique_ptr<Shape> currentShapeType { nullptr };
    MouseState mouseState {};
    std::unique_ptr<Shape> drawingShape { nullptr };
    ShapeRecord canvasShapes[MAX_CANVAS_SHAPES];
    uint64_t shapesCount { 0 };
    int64_t selectedIndex { -1 };
};

#endif
