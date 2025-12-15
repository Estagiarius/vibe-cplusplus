#ifndef EDITCOURSEDIALOG_H
#define EDITCOURSEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class EditCourseDialog : public QDialog {
    Q_OBJECT
public:
    EditCourseDialog(const QString &name, const QString &description, QWidget *parent = nullptr)
        : QDialog(parent) {
        setWindowTitle("Editar Curso");

        QVBoxLayout *layout = new QVBoxLayout(this);
        m_nameEdit = new QLineEdit(name, this);
        m_descEdit = new QLineEdit(description, this);
        QPushButton *okButton = new QPushButton("OK", this);

        layout->addWidget(new QLabel("Nome:", this));
        layout->addWidget(m_nameEdit);
        layout->addWidget(new QLabel("Descrição:", this));
        layout->addWidget(m_descEdit);
        layout->addWidget(okButton);

        connect(okButton, &QPushButton::clicked, this, &EditCourseDialog::accept);
    }

    QString getName() const { return m_nameEdit->text(); }
    QString getDescription() const { return m_descEdit->text(); }

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_descEdit;
};

#endif // EDITCOURSEDIALOG_H
