#ifndef GRADEBOOKVIEW_H
#define GRADEBOOKVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <memory>
#include "AcademicManager.h"

class GradebookView : public QWidget {
    Q_OBJECT
public:
    explicit GradebookView(std::shared_ptr<AcademicManager> manager, QWidget *parent = nullptr);
    void refreshClasses();
    void refreshGrades();

private slots:
    void onClassChanged(int index);
    void onCellChanged(int row, int column);
    void onEnrollStudent();

private:
    std::shared_ptr<AcademicManager> m_manager;
    QComboBox *m_classCombo;
    QTableWidget *m_table;

    QComboBox *m_studentSelector; // For adding student to class

    bool m_loading; // To prevent cell changed signals during load
};

#endif // GRADEBOOKVIEW_H
