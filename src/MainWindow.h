#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <memory>
#include "AcademicManager.h"
#include "AIClient.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<AcademicManager> manager, std::shared_ptr<AIClient> aiClient, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Manual Operations
    void onAddStudent();
    void onAddCourse();

    // AI Chat
    void onSendChat();
    void appendChatMessage(const QString& sender, const QString& message);

private:
    std::shared_ptr<AcademicManager> m_manager;
    std::shared_ptr<AIClient> m_aiClient;

    QTabWidget *m_tabs;

    // Management Widgets
    QLineEdit *m_studentNameEdit;
    QLineEdit *m_studentRegEdit;

    // Chat Widgets
    QTextEdit *m_chatDisplay;
    QLineEdit *m_chatInput;
    QPushButton *m_sendBtn;

    void setupUi();
    void setupManagementTab();
    void setupChatTab();
};

#endif // MAINWINDOW_H
