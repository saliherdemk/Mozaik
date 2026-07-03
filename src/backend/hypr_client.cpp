#include "hypr_client.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <QTextStream>

QVector<HyprWindow> HyprClient::fetchActiveWindows() {
  QVector<HyprWindow> windows;

  QProcess process;
  process.start("hyprctl", QStringList() << "-j" << "clients");
  // ponytail: blocks the UI thread; 3s cap instead of the 30s default
  if (!process.waitForFinished(3000)) {
    qWarning() << "Failed to execute hyprctl command.";
    return windows;
  }

  QByteArray output = process.readAllStandardOutput();
  QJsonDocument doc = QJsonDocument::fromJson(output);

  if (!doc.isArray()) {
    qWarning() << "Hyprland output is not a valid JSON array.";
    return windows;
  }

  QJsonArray array = doc.array();
  for (const QJsonValue &value : array) {
    QJsonObject obj = value.toObject();

    HyprWindow win;
    win.address = obj["address"].toString();
    win.wmClass = obj["class"].toString();
    win.title = obj["title"].toString();
    win.isFloating = obj["floating"].toBool();

    QJsonObject workspaceObj = obj["workspace"].toObject();
    win.workspaceId = workspaceObj["id"].toInt();

    windows.append(win);
  }

  return windows;
}

static const QRegularExpression kRuleBlockRe(
    R"re(hl\.window_rule\(\{(.*?)\}\))re",
    QRegularExpression::DotMatchesEverythingOption);

static QString serializeLuaRule(const ExistingRule &rule) {
  QString s = "hl.window_rule({\n";
  s += QString("  name = \"%1\",\n").arg(rule.name);
  if (rule.floatEnabled)
    s += "  float = true,\n";
  if (!rule.size.isEmpty())
    s += QString("  size = \"%1\",\n").arg(rule.size);
  if (!rule.move.isEmpty())
    s += QString("  move = \"%1\",\n").arg(rule.move);
  // class/title hold raw Lua expressions (may be vars.* or ".." concats),
  // so they are written back verbatim, never re-quoted.
  if (rule.matchTitle.isEmpty())
    s += QString("  match = { class = %1 },\n").arg(rule.matchClass);
  else
    s += QString("  match = { class = %1, title = %2 },\n")
             .arg(rule.matchClass, rule.matchTitle);
  s += "})";
  return s;
}

bool HyprClient::writeRulesFile(const QString &path,
                                const QVector<ExistingRule> &rules,
                                const QString &header) {
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "Could not open rules file for writing:" << path;
    return false;
  }

  QTextStream out(&file);
  if (!header.trimmed().isEmpty())
    out << header.trimmed() << "\n\n";
  for (const ExistingRule &rule : rules)
    out << serializeLuaRule(rule) << "\n\n";
  file.close();

  QProcess::startDetached("hyprctl", QStringList() << "reload");
  return true;
}

QVector<ExistingRule> HyprClient::parseRulesFile(const QString &path,
                                                 QString *header) {
  QVector<ExistingRule> rules;

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Could not open rules file for reading:" << path;
    return rules;
  }

  QString content = QTextStream(&file).readAll();
  file.close();

  if (header) {
    QRegularExpressionMatch first = kRuleBlockRe.match(content);
    *header = first.hasMatch() ? content.left(first.capturedStart(0)) : content;
  }

  static const QRegularExpression matchRe(
      R"re(match\s*=\s*\{(.*?)\})re",
      QRegularExpression::DotMatchesEverythingOption);
  static const QRegularExpression nameRe(R"re(name\s*=\s*"([^"]*)")re");
  static const QRegularExpression classFieldRe(
      R"re(class\s*=\s*(.+?)(?:,\s*title\s*=\s*(.+?))?\s*,?\s*$)re");
  static const QRegularExpression floatRe(R"re(\bfloat\s*=\s*true\b)re");
  static const QRegularExpression sizeRe(R"re(size\s*=\s*"([^"]*)")re");
  static const QRegularExpression moveRe(R"re(move\s*=\s*"([^"]*)")re");

  QRegularExpressionMatchIterator blockIt = kRuleBlockRe.globalMatch(content);
  while (blockIt.hasNext()) {
    QString block = blockIt.next().captured(1);

    ExistingRule rule;

    QRegularExpressionMatch nameMatch = nameRe.match(block);
    if (nameMatch.hasMatch())
      rule.name = nameMatch.captured(1);

    QRegularExpressionMatch matchMatch = matchRe.match(block);
    if (matchMatch.hasMatch()) {
      QString matchBlock = matchMatch.captured(1).trimmed();

      QRegularExpressionMatch fieldsMatch = classFieldRe.match(matchBlock);
      if (fieldsMatch.hasMatch()) {
        rule.matchClass = fieldsMatch.captured(1).trimmed();
        if (!fieldsMatch.captured(2).isEmpty())
          rule.matchTitle = fieldsMatch.captured(2).trimmed();
      }
    }

    rule.floatEnabled = floatRe.match(block).hasMatch();

    QRegularExpressionMatch sizeMatch = sizeRe.match(block);
    if (sizeMatch.hasMatch())
      rule.size = sizeMatch.captured(1);

    QRegularExpressionMatch moveMatch = moveRe.match(block);
    if (moveMatch.hasMatch())
      rule.move = moveMatch.captured(1);

    rules.append(rule);
  }

  return rules;
}

void HyprClient::focusWindow(const QString &address) {
  if (address.isEmpty())
    return;

  QProcess::startDetached(
      "hyprctl", QStringList() << "dispatch" << "focuswindow"
                                << QString("address:%1").arg(address));
}
