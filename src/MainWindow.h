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
#include "SettingsDialog.h"

// Forward declarations of our new widgets
class StudentView;
class CourseView;
class ClassView;
class GradebookView;

/**
 * @brief The MainWindow class is the main window of the application.
 *
 * It contains the main tab widget with the management and AI chat interfaces,
 * as well as the menu bar for accessing settings.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructs a new MainWindow.
     * @param manager A shared pointer to the AcademicManager for business logic.
     * @param aiClient A shared pointer to the AIClient for AI interactions.
     * @param parent The parent widget.
     */
    MainWindow(std::shared_ptr<AcademicManager> manager, std::shared_ptr<AIClient> aiClient, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // AI Chat
    /**
     * @brief Called when the user sends a message in the chat.
     */
    void onSendChat();

    /**
     * @brief Appends a message to the chat display.
     * @param sender The sender of the message (e.g., "User" or "AI").
     * @param message The message content.
     */
    void appendChatMessage(const QString& sender, const QString& message);

    /**
     * @brief Opens the AI settings dialog.
     */
    void onSettings();

    /**
     * @brief Copies the last AI response to the clipboard.
     */
    void onCopyResponse();

    // Sync slots (refresh views when tabs change)
    void onTabChanged(int index);

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
    QLabel *m_typingIndicator;
    QPushButton *m_copyButton;

    QString m_lastResponse;

    /**
     * @brief Sets up the main UI of the window.
     */
    void setupUi();

    /**
     * @brief Sets up the management tab with all the management views.
     */
    void setupManagementTab();

    /**
     * @brief Sets up the AI assistant chat tab.
     */
    void setupChatTab();
};

#endif // MAINWINDOW_H
