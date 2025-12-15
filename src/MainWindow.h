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

// Forward declarations of our new widgets
class StudentView;
class CourseView;
class ClassView;
class GradebookView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<AcademicManager> manager, std::shared_ptr<AIClient> aiClient, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // AI Chat
    void onSendChat();
    void appendChatMessage(const QString& sender, const QString& message);

    // Sync slots (refresh views when tabs change)
    void onTabChanged(int index);

    // Settings
    void onSettings();

private:
    std::shared_ptr<AcademicManager> m_manager;
    std::shared_ptr<AIClient> m_aiClient;

    QTabWidget *m_mainTabs;

    // Management Views
    StudentView *m_studentView;
    CourseView *m_courseView;
    ClassView *m_classView;
    GradebookView *m_gradebookView;

    // Chat Widgets
    QTextEdit *m_chatDisplay;
    QLineEdit *m_chatInput;
    QPushButton *m_sendBtn;

    void setupUi();
    void setupManagementTab();
    void setupChatTab();
};

#endif // MAINWINDOW_H
