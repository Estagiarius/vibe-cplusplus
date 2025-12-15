#include "AIClient.h"
#include <QDebug>
#include <QJsonValue>

AIClient::AIClient(std::shared_ptr<AcademicManager> manager, QObject *parent)
    : QObject(parent), m_manager(manager) {

    // Defaults for Ollama local
    m_baseUrl = "http://localhost:11434/v1/chat/completions";
    m_model = "llama3"; // Default, can be changed
    m_apiKey = "ollama"; // Not usually needed for local ollama, but good for structure

    setupTools();
}

void AIClient::setBaseUrl(const QString& url) {
    m_baseUrl = url;
}

void AIClient::setApiKey(const QString& key) {
    m_apiKey = key;
}

void AIClient::setModel(const QString& model) {
    m_model = model;
}

void AIClient::setupTools() {
    // Define tools in OpenAI compatible JSON format

    QJsonObject addStudentTool;
    addStudentTool["type"] = "function";
    QJsonObject asFunc;
    asFunc["name"] = "add_student";
    asFunc["description"] = "Register a new student in the system.";
    QJsonObject asParams;
    asParams["type"] = "object";
    QJsonObject asProps;
    asProps["name"] = QJsonObject{{"type", "string"}, {"description", "Full name of the student"}};
    asProps["registration"] = QJsonObject{{"type", "string"}, {"description", "Unique registration ID (matricula)"}};
    asParams["properties"] = asProps;
    asParams["required"] = QJsonArray{"name", "registration"};
    asFunc["parameters"] = asParams;
    addStudentTool["function"] = asFunc;

    QJsonObject addCourseTool;
    addCourseTool["type"] = "function";
    QJsonObject acFunc;
    acFunc["name"] = "add_course";
    acFunc["description"] = "Create a new course.";
    QJsonObject acParams;
    acParams["type"] = "object";
    QJsonObject acProps;
    acProps["name"] = QJsonObject{{"type", "string"}, {"description", "Name of the course (e.g., Mathematics)"}};
    acProps["description"] = QJsonObject{{"type", "string"}, {"description", "Short description"}};
    acParams["properties"] = acProps;
    acParams["required"] = QJsonArray{"name"};
    acFunc["parameters"] = acParams;
    addCourseTool["function"] = acFunc;

    QJsonObject updateGradeTool;
    updateGradeTool["type"] = "function";
    QJsonObject ugFunc;
    ugFunc["name"] = "update_grades";
    ugFunc["description"] = "Register or update grades for a student in a course.";
    QJsonObject ugParams;
    ugParams["type"] = "object";
    QJsonObject ugProps;
    ugProps["student_registration"] = QJsonObject{{"type", "string"}};
    ugProps["course_name"] = QJsonObject{{"type", "string"}};
    ugProps["b1"] = QJsonObject{{"type", "number"}};
    ugProps["b2"] = QJsonObject{{"type", "number"}};
    ugProps["b3"] = QJsonObject{{"type", "number"}};
    ugProps["b4"] = QJsonObject{{"type", "number"}};
    ugParams["properties"] = ugProps;
    ugParams["required"] = QJsonArray{"student_registration", "course_name"};
    ugFunc["parameters"] = ugParams;
    updateGradeTool["function"] = ugFunc;

    m_tools.append(addStudentTool);
    m_tools.append(addCourseTool);
    m_tools.append(updateGradeTool);

    QJsonObject getStudentReportTool;
    getStudentReportTool["type"] = "function";
    QJsonObject gsrFunc;
    gsrFunc["name"] = "get_student_report";
    gsrFunc["description"] = "Get a report of a student's grades and status in all classes.";
    QJsonObject gsrParams;
    gsrParams["type"] = "object";
    QJsonObject gsrProps;
    gsrProps["registration"] = QJsonObject{{"type", "string"}, {"description", "Student Registration ID"}};
    gsrParams["properties"] = gsrProps;
    gsrParams["required"] = QJsonArray{"registration"};
    gsrFunc["parameters"] = gsrParams;
    getStudentReportTool["function"] = gsrFunc;

    QJsonObject getClassReportTool;
    getClassReportTool["type"] = "function";
    QJsonObject gcrFunc;
    gcrFunc["name"] = "get_class_report";
    gcrFunc["description"] = "Get a statistical report of a specific class (course + semester).";
    QJsonObject gcrParams;
    gcrParams["type"] = "object";
    QJsonObject gcrProps;
    gcrProps["course_name"] = QJsonObject{{"type", "string"}};
    gcrProps["semester"] = QJsonObject{{"type", "string"}};
    gcrParams["properties"] = gcrProps;
    gcrParams["required"] = QJsonArray{"course_name", "semester"};
    gcrFunc["parameters"] = gcrParams;
    getClassReportTool["function"] = gcrFunc;

    m_tools.append(getStudentReportTool);
    m_tools.append(getClassReportTool);
}

void AIClient::sendMessage(const QString& userMessage, std::function<void(QString)> callback) {
    QNetworkRequest request;
    request.setUrl(QUrl(m_baseUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + m_apiKey.toUtf8());

    QJsonObject payload;
    payload["model"] = m_model;

    QJsonArray messages;
    messages.append(QJsonObject{
        {"role", "user"},
        {"content", userMessage}
    });
    payload["messages"] = messages;
    payload["tools"] = m_tools;
    payload["tool_choice"] = "auto";

    QNetworkReply *reply = m_network.post(request, QJsonDocument(payload).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply, callback]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QJsonObject root = doc.object();

            // Parse choices
            QJsonArray choices = root["choices"].toArray();
            if (!choices.isEmpty()) {
                QJsonObject message = choices[0].toObject()["message"].toObject();

                // Check for tool calls
                if (message.contains("tool_calls")) {
                    QJsonArray toolCalls = message["tool_calls"].toArray();
                    // Handle the first tool call (simplified for now)
                    if (!toolCalls.isEmpty()) {
                        handleToolCall(toolCalls[0].toObject(), callback);
                    }
                } else {
                    // Plain text response
                    callback(message["content"].toString());
                }
            } else {
                callback("Error: No response choices from AI.");
            }
        } else {
            callback("Network Error: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void AIClient::handleToolCall(const QJsonObject& toolCall, std::function<void(QString)> callback) {
    QJsonObject func = toolCall["function"].toObject();
    QString name = func["name"].toString();
    QString argsStr = func["arguments"].toString();
    QJsonObject args = QJsonDocument::fromJson(argsStr.toUtf8()).object();

    executeTool(name, args, callback);
}

void AIClient::executeTool(const QString& name, const QJsonObject& args, std::function<void(QString)> callback) {
    QString result;

    if (name == "add_student") {
        result = m_manager->addStudent(
            args["name"].toString(),
            args["registration"].toString()
        );
    } else if (name == "add_course") {
        result = m_manager->createCourse(
            args["name"].toString(),
            args["description"].toString()
        );
    } else if (name == "update_grades") {
        result = m_manager->registerGrades(
            args["student_registration"].toString(),
            args["course_name"].toString(),
            args["b1"].toDouble(),
            args["b2"].toDouble(),
            args["b3"].toDouble(),
            args["b4"].toDouble()
        );
    } else if (name == "get_student_report") {
        result = m_manager->getStudentReport(
            args["registration"].toString()
        );
    } else if (name == "get_class_report") {
        result = m_manager->getClassReport(
            args["course_name"].toString(),
            args["semester"].toString()
        );
    } else {
        result = "Unknown tool: " + name;
    }

    callback(result);
}
