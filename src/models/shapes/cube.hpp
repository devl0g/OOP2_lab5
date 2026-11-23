#ifndef INCLUDE_MODEL_SHAPES_CUBE_HPP_
#define INCLUDE_MODEL_SHAPES_CUBE_HPP_

#include "./shape.hpp"
#include "./rectangle.hpp"
#include "./line.hpp"
#include <QIcon>
#include <algorithm>
#include <initializer_list>

class LinesContaining {
public:
  LinesContaining() = default;
  LinesContaining(std::initializer_list<LineShape> lines) noexcept : lines(lines) {}
  ~LinesContaining() = default;
  LinesContaining(const LinesContaining& other) noexcept : lines(other.lines) {}
  LinesContaining& operator=(LinesContaining other) {
    this->lines = other.lines;
    return *this;
  }

  LineShape& addLine(LineShape&& line) {
    lines.push_back(line);
    return lines.back();
  }

  LineShape& getLine(size_t lineIndex) {
    return lines.at(lineIndex);
  }

  const LineShape& getLine(size_t lineIndex) const {
    return lines.at(lineIndex);
  }

private:
    std::vector<LineShape> lines;
};

class RectanglesContaining {
public:
  RectanglesContaining() = default;
  RectanglesContaining(std::initializer_list<RectangleShape> rectangles) noexcept : rectangles(rectangles) {}
  ~RectanglesContaining() = default;
  RectanglesContaining(const RectanglesContaining& other) noexcept : rectangles(other.rectangles) {}
  RectanglesContaining& operator=(RectanglesContaining other) {
    this->rectangles = other.rectangles;
    return *this;
  }

  RectangleShape& addRectangle(RectangleShape&& rectangle) {
    rectangles.push_back(rectangle);
    return rectangles.back();
  }

  RectangleShape& getRectangle(size_t rectIndex) {
    return rectangles.at(rectIndex);
  }

  const RectangleShape& getRectangle(size_t rectIndex) const {
    return rectangles.at(rectIndex);
  }

private:
    std::vector<RectangleShape> rectangles;
};

class CubeShape : public Shape, private LinesContaining, private RectanglesContaining {
public:
  CubeShape(): LinesContaining({ LineShape(), LineShape(), LineShape(), LineShape() }), RectanglesContaining({ RectangleShape(), RectangleShape() }) {}
  CubeShape(Position aPos, Position bPos) : LinesContaining({ LineShape(), LineShape(), LineShape(), LineShape() }), RectanglesContaining({ RectangleShape(), RectangleShape() }), startPosition(aPos), endPosition(bPos) {};
  ~CubeShape() = default;
  CubeShape(const CubeShape& other) noexcept : startPosition(other.startPosition), endPosition(other.endPosition) {}
  CubeShape& operator=(CubeShape other) {
    this->startPosition = other.startPosition;
    this->endPosition = other.endPosition;
    return *this;
  }

  [[nodiscard]] Position getStartPosition() const {
    return startPosition;
  }

  void setStartPosition(Position position) {
    this->startPosition = position;
  }

  [[nodiscard]] Position getEndPosition() const {
    return endPosition;
  }

  void setEndPosition(Position position) {
    this->endPosition = position;
  }

  char* getShapeName() const noexcept override {
    static char shapeName[] { "Куб" };
    return shapeName;
  }

  uint8_t getShapeID() const noexcept override {
    return 6;
  }

  std::unique_ptr<Shape> createInstance() const noexcept override {
    return std::make_unique<CubeShape>();
  }

  void renderShape(QPainter& painter) const override {
    RectanglesContaining::getRectangle(0).renderShape(painter);
    RectanglesContaining::getRectangle(1).renderShape(painter);
    LinesContaining::getLine(0).renderShape(painter);
    LinesContaining::getLine(1).renderShape(painter);
    LinesContaining::getLine(2).renderShape(painter);
    LinesContaining::getLine(3).renderShape(painter);
  }

  void renderPhantomShape(QPainter& painter) const override {
    RectanglesContaining::getRectangle(0).renderPhantomShape(painter);
    RectanglesContaining::getRectangle(1).renderPhantomShape(painter);
    LinesContaining::getLine(0).renderPhantomShape(painter);
    LinesContaining::getLine(1).renderPhantomShape(painter);
    LinesContaining::getLine(2).renderPhantomShape(painter);
    LinesContaining::getLine(3).renderPhantomShape(painter);
  }

  void updateShapeConfig(const Position& dragStart, const Position& dragEnd) noexcept override {
    this->startPosition = dragStart;
    this->endPosition = dragEnd;
    Position start = calcStart();
    Position end = calcEnd();
    Position sstart = calcSecondCubeStart();
    Position fend = calcFirstCubeEnd();

    Position aCenter = { (start.x + fend.x) / 2, (start.y + fend.y) / 2 };
    RectanglesContaining::getRectangle(0).updateShapeConfig(
        aCenter,
        { (fend.x - start.x) / 2 + aCenter.x, (fend.y - start.y) / 2 + aCenter.y }
    );

    Position bCenter = { (sstart.x + end.x) / 2, (sstart.y + end.y) / 2 };
    RectanglesContaining::getRectangle(1).updateShapeConfig(
        bCenter,
        { (end.x - sstart.x) / 2 + bCenter.x, (end.y - sstart.y) / 2 + bCenter.y }
    );

    LinesContaining::getLine(0).updateShapeConfig({ fend.x, start.y }, { end.x, sstart.y });
    LinesContaining::getLine(1).updateShapeConfig({ start.x, fend.y }, { sstart.x, end.y });
    LinesContaining::getLine(2).updateShapeConfig(start, sstart);
    LinesContaining::getLine(3).updateShapeConfig(end, fend);
  }

  QIcon getToolbarIcon() noexcept override {
    return QIcon(":/icons/cube.png");
  }

private:
  Position startPosition { 0, 0 };
  Position endPosition { 0, 0 };

  [[nodiscard]] Position calcStart() const noexcept {
    return { std::min(startPosition.x, endPosition.x), std::min(startPosition.y, endPosition.y) };
  }

  [[nodiscard]] Position calcEnd() const noexcept {
    return { std::max(startPosition.x, endPosition.x), std::max(startPosition.y, endPosition.y) };
  }

  [[nodiscard]] Position calcFirstCubeEnd() const noexcept {
    Position start = calcStart();
    Position end = calcEnd();

    Position size {
      .x = end.x - start.x,
      .y = end.y - start.y,
    };

    size.x *= 2.0f / 3.0f;
    size.y *= 2.0f / 3.0f;

    return { start.x + size.x, start.y + size.y };
  }

  [[nodiscard]] Position calcSecondCubeStart() const noexcept {
    Position start = calcStart();
    Position end = calcEnd();

    Position size {
      .x = end.x - start.x,
      .y = end.y - start.y,
    };

    size.x *= 2.0f / 3.0f;
    size.y *= 2.0f / 3.0f;

    return { end.x - size.x, end.y - size.y };
  }
};

#endif

