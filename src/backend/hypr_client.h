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

  bool writeRulesFile(const QString &path, const QVector<ExistingRule> &rules,
                      const QString &header = QString());

  bool focusWindow(const QString &address);

  // header receives everything before the first rule block (requires,
  // local vars, comments) so writes can preserve it.
  QVector<ExistingRule> parseRulesFile(const QString &path,
                                       QString *header = nullptr);
};
