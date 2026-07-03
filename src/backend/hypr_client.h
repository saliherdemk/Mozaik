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
  QString opacity; 
};

namespace HyprClient {

QVector<HyprWindow> fetchActiveWindows();

bool writeRulesFile(const QString &path, const QVector<ExistingRule> &rules,
                    const QString &header = QString());

void focusWindow(const QString &address);

QVector<ExistingRule> parseRulesFile(const QString &path,
                                     QString *header = nullptr);

} 
