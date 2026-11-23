#include "models/shapes/dot.hpp"
#include "models/shapes/line.hpp"
#include "models/shapes/oval.hpp"
#include "models/shapes/rectangle.hpp"
#include "models/shapes/lineDots.hpp"
#include "models/shapes/cube.hpp"
#include "utils/logger.hpp"
#include "ui/windows/myEditor.hpp"
#include <QApplication>
#include <memory>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  std::unique_ptr<MyEditor> myEditorInstance { std::make_unique<MyEditor>() };

  myEditorInstance->registerShape<DotShape>();
  myEditorInstance->registerShape<LineShape>();
  myEditorInstance->registerShape<OvalShape>();
  myEditorInstance->registerShape<RectangleShape>();
  myEditorInstance->registerShape<LineDotsShape>();
  myEditorInstance->registerShape<CubeShape>();

  myEditorInstance->show();

  Logger::info("window initiation");
  return app.exec();
}
