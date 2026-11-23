#ifndef INCLUDE_MODEL_SHAPES_LINE_DOTS_HPP_
#define INCLUDE_MODEL_SHAPES_LINE_DOTS_HPP_

#include "./shape.hpp"
#include "./dot.hpp"
#include "./line.hpp"
#include <QIcon>
#include <initializer_list>

class DotsContaining {
public:
  DotsContaining() = default;
  DotsContaining(std::initializer_list<DotShape> dots) noexcept : dots(dots) {}
  ~DotsContaining() = default;
  DotsContaining(const DotsContaining& other) noexcept : dots(other.dots) {}
  DotsContaining& operator=(DotsContaining other) {
    this->dots = other.dots;
    return *this;
  }

  DotShape& addDot(DotShape&& dot) {
    dots.push_back(dot);
    return dots.back();
  }

  DotShape& getDot(size_t dotIndex) {
    return dots.at(dotIndex);
  }

  const DotShape& getDot(size_t dotIndex) const {
    return dots.at(dotIndex);
  }

private:
    std::vector<DotShape> dots;
};

class LineDotsShape : public LineShape, private DotsContaining {
public:
  LineDotsShape() : DotsContaining({ DotShape(), DotShape() }) {}
  LineDotsShape(Position aPos, Position bPos) : DotsContaining({ DotShape(aPos), DotShape(bPos) }), LineShape(aPos, bPos) {}
  ~LineDotsShape() = default;
  LineDotsShape(const LineDotsShape& other) noexcept 
    : DotsContaining({ DotShape(other.getStartPosition()), DotShape(other.getEndPosition()) })
    , LineShape(other.getStartPosition(), other.getEndPosition()) {}
  LineDotsShape& operator=(LineDotsShape other) {
    setStartPosition(other.getStartPosition());
    setEndPosition(other.getEndPosition());
    return *this;
  }

  [[nodiscard]] Position getStartPosition() const {
    return LineShape::getStartPosition();
  }

  void setStartPosition(Position position) {
    DotsContaining::getDot(0).setPosition(position);
    LineShape::setStartPosition(position);
  }

  [[nodiscard]] Position getEndPosition() const {
    return LineShape::getEndPosition();
  }

  void setEndPosition(Position position) {
    DotsContaining::getDot(1).setPosition(position);
    LineShape::setEndPosition(position);
  }

  char* getShapeName() const noexcept override {
    static char shapeName[] { "Лінія з точками" };
    return shapeName;
  }

  uint8_t getShapeID() const noexcept override {
    return 5;
  }

  std::unique_ptr<Shape> createInstance() const noexcept override {
    return std::make_unique<LineDotsShape>();
  }

  void renderShape(QPainter& painter) const override {
    LineShape::renderShape(painter);
    DotsContaining::getDot(0).renderShape(painter);
    DotsContaining::getDot(1).renderShape(painter);
  }

  void renderPhantomShape(QPainter& painter) const override {
    LineShape::renderPhantomShape(painter);
    DotsContaining::getDot(0).renderPhantomShape(painter);
    DotsContaining::getDot(1).renderPhantomShape(painter);
  }

  void updateShapeConfig(const Position& dragStart, const Position& dragEnd) noexcept override {
    LineShape::updateShapeConfig(dragStart, dragEnd);
    DotsContaining::getDot(0).updateShapeConfig(dragStart, dragStart);
    DotsContaining::getDot(1).updateShapeConfig(dragEnd, dragEnd);
  }

  QIcon getToolbarIcon() noexcept override {
    return QIcon(":/icons/line-dot.png");
  }
};

#endif

