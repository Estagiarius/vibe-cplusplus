#include <QApplication>
#include "MainWindow.h"
#include "DatabaseManager.h"
#include "AcademicManager.h"
#include "AIClient.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Initialize Database
    auto db = std::make_shared<DatabaseManager>("academic_system.db");
    if (!db->init()) {
        return -1;
    }

    // Initialize Logic
    auto manager = std::make_shared<AcademicManager>(db);

    // Initialize AI Client
    auto aiClient = std::make_shared<AIClient>(manager);

    // Create UI
    MainWindow w(manager, aiClient);
    w.show();

    return app.exec();
}
