#ifndef GRADEBOOKVIEW_H
#define GRADEBOOKVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QDateEdit>
#include <memory>
#include "AcademicManager.h"

class GradebookView : public QWidget {
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override;
public:
    explicit GradebookView(std::shared_ptr<AcademicManager> manager, QWidget *parent = nullptr);
    void refreshClasses();
    void refreshGrades();
    void refreshAttendance();

private slots:
    void onClassChanged(int index);
    void onCellChanged(int row, int column);
    void onEnrollStudent();
    void onDateChanged(const QDate &date);
    void onSaveAttendance();
    void onExportGrades();

private:
    void setupUi();
    void setupGradesTab();
    void setupAttendanceTab();

    std::shared_ptr<AcademicManager> m_manager;

    // Main UI
    QTabWidget *m_tabWidget;
    QComboBox *m_classCombo;

    // Grades Tab
    QWidget *m_gradesTab;
    QTableWidget *m_gradesTable;
    QComboBox *m_studentSelector;

    // Attendance Tab
    QWidget *m_attendanceTab;
    QTableWidget *m_attendanceTable;
    QDateEdit *m_dateEdit;

    bool m_loading; // To prevent cell changed signals during load
};

#endif // GRADEBOOKVIEW_H
