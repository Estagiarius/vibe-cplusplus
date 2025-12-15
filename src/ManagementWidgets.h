#ifndef MANAGEMENTWIDGETS_H
#define MANAGEMENTWIDGETS_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <memory>
#include "AcademicManager.h"

// Base class for shared styling or logic if needed, but for now standalone is fine.

// ======================= STUDENT VIEW =======================
class StudentView : public QWidget {
    Q_OBJECT
public:
    explicit StudentView(std::shared_ptr<AcademicManager> manager, QWidget *parent = nullptr);
    void refresh();

private slots:
    void onAdd();

private:
    std::shared_ptr<AcademicManager> m_manager;
    QTableView *m_table;
    QStandardItemModel *m_model;

    QLineEdit *m_nameEdit;
    QLineEdit *m_regEdit;
};

// ======================= COURSE VIEW =======================
class CourseView : public QWidget {
    Q_OBJECT
public:
    explicit CourseView(std::shared_ptr<AcademicManager> manager, QWidget *parent = nullptr);
    void refresh();

private slots:
    void onAdd();

private:
    std::shared_ptr<AcademicManager> m_manager;
    QTableView *m_table;
    QStandardItemModel *m_model;

    QLineEdit *m_nameEdit;
    QLineEdit *m_descEdit;
};

// ======================= CLASS VIEW =======================
class ClassView : public QWidget {
    Q_OBJECT
public:
    explicit ClassView(std::shared_ptr<AcademicManager> manager, QWidget *parent = nullptr);
    void refresh();

private slots:
    void onAdd();

private:
    std::shared_ptr<AcademicManager> m_manager;
    QTableView *m_table;
    QStandardItemModel *m_model;

    QComboBox *m_courseCombo;
    QLineEdit *m_semesterEdit;
};

#endif // MANAGEMENTWIDGETS_H
