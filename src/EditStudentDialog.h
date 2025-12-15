#ifndef EDITSTUDENTDIALOG_H
#define EDITSTUDENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class EditStudentDialog : public QDialog {
    Q_OBJECT
public:
    EditStudentDialog(const QString &name, const QString &registration, QWidget *parent = nullptr)
        : QDialog(parent) {
        setWindowTitle("Editar Aluno");

        QVBoxLayout *layout = new QVBoxLayout(this);
        m_nameEdit = new QLineEdit(name, this);
        m_regEdit = new QLineEdit(registration, this);
        QPushButton *okButton = new QPushButton("OK", this);

        layout->addWidget(new QLabel("Nome:", this));
        layout->addWidget(m_nameEdit);
        layout->addWidget(new QLabel("Matrícula:", this));
        layout->addWidget(m_regEdit);
        layout->addWidget(okButton);

        connect(okButton, &QPushButton::clicked, this, &EditStudentDialog::accept);
    }

    QString getName() const { return m_nameEdit->text(); }
    QString getRegistration() const { return m_regEdit->text(); }

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_regEdit;
};

#endif // EDITSTUDENTDIALOG_H
