#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  qputenv("QT_QPA_PLATFORMTHEME", "xdgdesktopportal");

  QApplication app(argc, argv);
  // Wayland app_id must match the .desktop basename for bars to find the icon
  app.setDesktopFileName("mozaik");

  MainWindow window;
  window.show();

  return app.exec();
}
