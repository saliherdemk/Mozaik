#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  qputenv("QT_QPA_PLATFORMTHEME", "xdgdesktopportal");

  QApplication app(argc, argv);

  MainWindow window;
  window.show();

  return app.exec();
}
