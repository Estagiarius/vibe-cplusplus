#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <QList>
#include <optional>

struct Student {
    int id;
    QString name;
    QString registration;
};

struct Course {
    int id;
    QString name;
    QString description;
};

struct Class {
    int id;
    int courseId;
    QString semester; // e.g., "2023.1"
};

struct Grade {
    int id;
    int studentId;
    int classId;
    double b1, b2, b3, b4;
    double finalGrade; // optional 5th grade if needed, or just part of calculation
    bool hasFinal;
};

struct ClassEnrollment {
    int studentId;
    QString studentName;
    QString studentReg;
    Grade grade;
};

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    explicit DatabaseManager(const QString& dbName = "academic.db", QObject *parent = nullptr);
    ~DatabaseManager();

    bool init();

    // Students
    bool addStudent(const QString& name, const QString& registration);
    QList<Student> getAllStudents();
    std::optional<Student> getStudentByRegistration(const QString& reg);

    // Courses
    bool addCourse(const QString& name, const QString& description);
    QList<Course> getAllCourses();

    // Classes
    bool addClass(int courseId, const QString& semester);
    QList<Class> getAllClasses();

    // Enrollments/Grades
    bool enrollStudent(int studentId, int classId);
    bool updateGrades(int studentId, int classId, double b1, double b2, double b3, double b4, double finalGrade = -1);
    std::optional<Grade> getStudentGrade(int studentId, int classId);
    QList<ClassEnrollment> getClassEnrollments(int classId);

    QSqlDatabase getDb() const { return m_db; }

private:
    QSqlDatabase m_db;
    bool createTables();
};

#endif // DATABASEMANAGER_H
