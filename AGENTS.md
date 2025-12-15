# Academic Management System with AI - Developer Guide

## Project Overview

This project is a C++ Academic Management System that integrates a local Generative AI (via Ollama or OpenAI API) to assist in managing students, courses, classes, and grades.

**Key Features:**
*   **Core Management:** CRUD operations for Students, Courses, and Classes.
*   **Grade Management:** Support for 4 bimesters + Final Exam.
    *   Approval Rule: Average >= 5.0 (Considering Final Exam if needed).
*   **AI Integration:** A Chat Interface that interprets natural language to execute system commands (e.g., "Add student John", "What is the class average?").
*   **GUI:** Built with Qt 6 Widgets, featuring specialized views for management and a spreadsheet-like Gradebook.

## Architecture

*   **`DatabaseManager`**: Low-level wrapper around `QSqlDatabase` (SQLite). Handles schema creation and raw CRUD queries.
*   **`AcademicManager`**: Business logic layer. Calculates averages, determines approval status, and provides a facade of "high-level string-based methods" for the AI to call.
*   **`AIClient`**: Manages HTTP communication with the LLM provider. Defines a JSON schema of tools (functions) that the AI can call and maps the AI's response back to `AcademicManager` methods.
*   **`MainWindow` & Widgets**:
    *   `MainWindow`: Holds the main tab structure (Management vs AI Chat).
    *   `ManagementWidgets`: `StudentView`, `CourseView`, `ClassView` for listing and adding entities.
    *   `GradebookView`: A complex widget for editing grades of a whole class.

## Build Instructions

**Requirements:**
*   C++17 compliant compiler
*   CMake >= 3.16
*   Qt 6 (Core, Gui, Widgets, Sql, Network, Test)
*   SQLite driver for Qt

**Steps:**
```bash
mkdir build
cd build
cmake ..
make
```

**Running the Application:**
```bash
./AcademicManager
```

**Running Tests:**
```bash
./TestLogic
```

## AI Configuration

The `AIClient` is configured by default to connect to a local Ollama instance:
*   **URL**: `http://localhost:11434/v1/chat/completions`
*   **Model**: `llama3`

To use OpenAI or another provider, modify the defaults in `src/AIClient.cpp` or (in future versions) use the settings UI.

### Supported AI Tools
The system exposes the following tools to the AI:
1.  `add_student(name, registration)`
2.  `add_course(name, description)`
3.  `update_grades(student_registration, course_name, b1, b2, b3, b4)`
4.  `get_student_report(registration)`
5.  `get_class_report(course_name, semester)`

## Code Style & Conventions

*   **Qt Framework**: Use Qt containers (`QList`, `QString`) and signals/slots where appropriate.
*   **Smart Pointers**: Use `std::shared_ptr` for managing the lifespan of core services (`AcademicManager`, `DatabaseManager`).
*   **Error Handling**: `DatabaseManager` methods return `bool` or `std::optional`. `AcademicManager` facade methods return user-friendly `QString` messages.

## Future Improvements
*   Implement "Edit/Delete" context menus in the management tables.
*   Add a Settings Dialog to configure AI Endpoint/Key at runtime.
*   Improve the complexity of the AI Prompt to handle more vague queries.
