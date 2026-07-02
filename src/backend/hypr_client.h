#pragma once
#include <QString>
#include <QVector>

struct HyprWindow {
  QString address;
  QString wmClass;
  QString title;
  int workspaceId;
  bool isFloating;
};

struct WindowRule {
  QString name;
  QString matchClass;
  QString matchTitle;

  bool floatEnabled = false;

  bool sizeEnabled = false;
  int sizeWidth = 0;
  int sizeHeight = 0;

  bool moveEnabled = false;
  int moveX = 0;
  int moveY = 0;
};

struct ExistingRule {
  QString name;
  QString matchClass;
  QString matchTitle;
  bool floatEnabled = false;
  QString size;
  QString move;
};

class HyprClient {
public:
  HyprClient() = default;

  QVector<HyprWindow> fetchActiveWindows();

  bool appendWindowRule(const WindowRule &rule);

  bool focusWindow(const QString &address);

  QVector<ExistingRule> parseRulesFile(const QString &path);
};
