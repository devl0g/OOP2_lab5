#include "models/shapes/dot.hpp"
#include "models/shapes/line.hpp"
#include "models/shapes/oval.hpp"
#include "models/shapes/rectangle.hpp"
#include "models/shapes/lineDots.hpp"
#include "models/shapes/cube.hpp"
#include "ui/windows/myEditor.hpp"
#include <QApplication>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  MyEditor* myEditorInstance { MyEditor::getInstance() };
  auto shapesTable = new ShapesTable();

  shapesTable->setOnRowSelected([myEditorInstance](int row) {
    myEditorInstance->getCanvas()->setSelectedIndex(row);
  });
  shapesTable->setOnRowDeleteRequested([myEditorInstance](int row) {
    myEditorInstance->getCanvas()->removeShape(static_cast<uint64_t>(row));
  });

  myEditorInstance->setTable(shapesTable);

  myEditorInstance->registerShape<DotShape>();
  myEditorInstance->registerShape<LineShape>();
  myEditorInstance->registerShape<OvalShape>();
  myEditorInstance->registerShape<RectangleShape>();
  myEditorInstance->registerShape<LineDotsShape>();
  myEditorInstance->registerShape<CubeShape>();

  myEditorInstance->show();

  const int result = app.exec();

  MyEditor::destroyInstance();
  return result;
}
