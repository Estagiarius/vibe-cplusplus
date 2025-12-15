#ifndef ACADEMICMANAGER_H
#define ACADEMICMANAGER_H

#include <QObject>
#include <memory>
#include "DatabaseManager.h"

struct StudentStatus {
    QString studentName;
    QString className;
    double average;
    bool approved;
    QString statusMessage;
};

class AcademicManager : public QObject {
    Q_OBJECT
public:
    explicit AcademicManager(std::shared_ptr<DatabaseManager> db, QObject *parent = nullptr);

    // Business Logic
    StudentStatus calculateStatus(int studentId, int classId);

    // Facade methods (to be used by GUI and AI)
    QString addStudent(const QString& name, const QString& registration);
    QString createCourse(const QString& name, const QString& description);
    QString openClass(const QString& courseName, const QString& semester); // AI might provide course name, not ID
    QString registerGrades(const QString& studentReg, const QString& courseName, double b1, double b2, double b3, double b4);
    QString getStudentReport(const QString& studentReg);
    QString getClassReport(const QString& courseName, const QString& semester);

    /**
     * @brief Analyzes the performance of a class, providing pedagogical insights.
     *
     * This function calculates the class average, standard deviation of grades,
     * and identifies students who are in academic recovery. It returns a formatted
     * string with a summary of the analysis, suitable for display or use by the AI.
     *
     * @param classId The ID of the class to analyze.
     * @return A QString containing the performance analysis report.
     */
    QString analyzeClassPerformance(int classId);

    // Direct Data Access (For GUI Models)
    QList<Student> getAllStudents();
    QList<Course> getAllCourses();
    QList<Class> getAllClasses();
    QList<ClassEnrollment> getClassEnrollments(int classId);

    // Grade update by IDs (GUI direct update)
    bool updateGradesById(int studentId, int classId, double b1, double b2, double b3, double b4, double finalGrade);
    bool enrollStudentById(int studentId, int classId);

private:
    std::shared_ptr<DatabaseManager> m_db;

    // Helper to resolve names to IDs
    int findCourseIdByName(const QString& name);
    int findStudentIdByReg(const QString& reg);
    int findClassIdByCourseAndSemester(int courseId, const QString& semester);
};

#endif // ACADEMICMANAGER_H
