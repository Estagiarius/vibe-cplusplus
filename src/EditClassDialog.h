#ifndef EDITCLASSDIALOG_H
#define EDITCLASSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "DatabaseManager.h"

class EditClassDialog : public QDialog {
    Q_OBJECT
public:
    EditClassDialog(int courseId, const QString &semester, const QList<Course> &courses, QWidget *parent = nullptr)
        : QDialog(parent) {
        setWindowTitle("Editar Turma");

        QVBoxLayout *layout = new QVBoxLayout(this);
        m_courseCombo = new QComboBox(this);
        for (const auto &course : courses) {
            m_courseCombo->addItem(course.name, course.id);
        }
        m_courseCombo->setCurrentIndex(m_courseCombo->findData(courseId));

        m_semesterEdit = new QLineEdit(semester, this);
        QPushButton *okButton = new QPushButton("OK", this);

        layout->addWidget(new QLabel("Curso:", this));
        layout->addWidget(m_courseCombo);
        layout->addWidget(new QLabel("Semestre:", this));
        layout->addWidget(m_semesterEdit);
        layout->addWidget(okButton);

        connect(okButton, &QPushButton::clicked, this, &EditClassDialog::accept);
    }

    int getCourseId() const { return m_courseCombo->currentData().toInt(); }
    QString getSemester() const { return m_semesterEdit->text(); }

private:
    QComboBox *m_courseCombo;
    QLineEdit *m_semesterEdit;
};

#endif // EDITCLASSDIALOG_H
