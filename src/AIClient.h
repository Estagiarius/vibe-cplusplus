#ifndef AICLIENT_H
#define AICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <functional>
#include "AcademicManager.h"

class AIClient : public QObject {
    Q_OBJECT
public:
    explicit AIClient(std::shared_ptr<AcademicManager> manager, QObject *parent = nullptr);

    void setBaseUrl(const QString& url); // e.g., http://localhost:11434/v1 for Ollama
    void setApiKey(const QString& key);
    void setModel(const QString& model);

    // Main entry point for user chat
    void sendMessage(const QString& userMessage, std::function<void(QString)> callback);

private:
    std::shared_ptr<AcademicManager> m_manager;
    QNetworkAccessManager m_network;
    QString m_baseUrl;
    QString m_apiKey;
    QString m_model;

    QJsonArray m_tools; // Tool definitions

    void setupTools();
    void handleToolCall(const QJsonObject& toolCall, std::function<void(QString)> callback);
    void executeTool(const QString& name, const QJsonObject& args, std::function<void(QString)> callback);
};

#endif // AICLIENT_H
