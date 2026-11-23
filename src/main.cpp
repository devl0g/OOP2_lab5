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
