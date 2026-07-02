#include "hypr_client.h"
#include <QDebug>
#include <QDir>
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
  if (!process.waitForFinished()) {
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

bool HyprClient::appendWindowRule(const WindowRule &rule) {
  if (rule.name.isEmpty() || rule.matchClass.isEmpty())
    return false;

  QString path = QDir::homePath() + "/.config/hypr/gui-rules.conf";
  QFile file(path);

  if (!file.open(QIODevice::Append | QIODevice::Text)) {
    qWarning() << "Could not open rules file for writing:" << path;
    return false;
  }

  QTextStream out(&file);
  out << "windowrule {\n";
  out << "  name = " << rule.name << "\n";

  if (rule.floatEnabled)
    out << "  float = on\n";

  if (rule.sizeEnabled)
    out << "  size = " << rule.sizeWidth << " " << rule.sizeHeight << "\n";

  if (rule.moveEnabled)
    out << "  move = " << rule.moveX << " " << rule.moveY << "\n";

  out << "  match:class = " << rule.matchClass << "\n";

  if (!rule.matchTitle.isEmpty())
    out << "  match:title = " << rule.matchTitle << "\n";

  out << "}\n\n";
  file.close();

  QProcess::startDetached("hyprctl", QStringList() << "reload");
  return true;
}

QVector<ExistingRule> HyprClient::parseRulesFile(const QString &path) {
  QVector<ExistingRule> rules;

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Could not open rules file for reading:" << path;
    return rules;
  }

  QString content = QTextStream(&file).readAll();
  file.close();

  static const QRegularExpression blockRe(
      R"re(hl\.window_rule\(\{(.*?)\}\))re",
      QRegularExpression::DotMatchesEverythingOption);
  static const QRegularExpression matchRe(
      R"re(match\s*=\s*\{(.*?)\})re",
      QRegularExpression::DotMatchesEverythingOption);
  static const QRegularExpression nameRe(R"re(name\s*=\s*"([^"]*)")re");
  static const QRegularExpression classFieldRe(
      R"re(class\s*=\s*(.+?)(?:,\s*title\s*=\s*(.+?))?\s*,?\s*$)re");
  static const QRegularExpression floatRe(R"re(\bfloat\s*=\s*true\b)re");
  static const QRegularExpression sizeRe(R"re(size\s*=\s*"([^"]*)")re");
  static const QRegularExpression moveRe(R"re(move\s*=\s*"([^"]*)")re");

  auto stripIfQuoted = [](QString s) {
    s = s.trimmed();
    if (s.length() >= 2 && s.startsWith('"') && s.endsWith('"') &&
        s.count('"') == 2)
      return s.mid(1, s.length() - 2);
    return s;
  };

  QRegularExpressionMatchIterator blockIt = blockRe.globalMatch(content);
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
        rule.matchClass = stripIfQuoted(fieldsMatch.captured(1));
        if (!fieldsMatch.captured(2).isEmpty())
          rule.matchTitle = stripIfQuoted(fieldsMatch.captured(2));
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

bool HyprClient::focusWindow(const QString &address) {
  if (address.isEmpty())
    return false;

  return QProcess::startDetached(
      "hyprctl", QStringList() << "dispatch" << "focuswindow"
                                << QString("address:%1").arg(address));
}
