# Academic Management System with AI Integration

This project is a C++17 application developed with the **Qt 6 Framework** that simplifies the management of students, courses, classes, and grades. Its standout feature is the integration of a **Generative AI Assistant** (powered by Ollama or OpenAI), allowing users to interact with the system using natural language.

## Features

*   **Academic Management:**
    *   **Students:** Register and list students.
    *   **Courses & Classes:** Create courses and open classes for specific semesters.
    *   **Gradebook:** Spreadsheet-like view to launch grades (B1, B2, B3, B4, Final Exam).
    *   **Automatic Calculation:** Computes averages and determines approval status based on Brazilian academic rules (Recovery if Avg < 7.0).
*   **AI Assistant:**
    *   Chat interface integrated into the application.
    *   Execute commands naturally: *"Add student Maria", "Update grades for John", "How is the Math class doing?"*.
    *   Generates statistical reports via chat.
*   **Modern GUI:**
    *   Built with Qt Widgets.
    *   Tabs for management and chat.
    *   Real-time validation and feedback.

## Prerequisites

To build and run this project, you need:

*   **Linux (Ubuntu/Pop!_OS/Debian recommended)**
*   **C++ Compiler** (GCC 9+ or Clang)
*   **CMake** (3.16 or newer)
*   **Qt 6** (Core, Gui, Widgets, Sql, Network, Test)
*   **SQLite**

## Installation

### Automatic Setup (Ubuntu/Pop!_OS)
We provide a script to install all dependencies:
```bash
chmod +x setup.sh
./setup.sh
```

### Manual Installation
If you prefer installing manually:
```bash
sudo apt update
sudo apt install build-essential cmake \
    qt6-base-dev qt6-base-dev-tools \
    libqt6sql6-sqlite libqt6network6t64 libqt6test6t64 \
    libgl1-mesa-dev
```

## Building the Project

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-repo/vibe-cplusplus.git
    cd vibe-cplusplus
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build && cd build
    ```

3.  **Compile with CMake:**
    ```bash
    cmake ..
    make
    ```

4.  **Run:**
    ```bash
    ./AcademicManager
    ```

## Running Tests

The project includes unit tests for business logic and validation.
```bash
cd build
./TestLogic
./TestValidation
```

## AI Configuration (Ollama)

By default, the system tries to connect to a local **Ollama** instance.

1.  [Install Ollama](https://ollama.com/)
2.  Pull the default model (Llama 3):
    ```bash
    ollama run llama3
    ```
3.  Keep Ollama running in the background. The application connects to `http://localhost:11434`.

*Note: To change the model or API URL (e.g., to use OpenAI), modify `src/AIClient.cpp`.*

## Project Structure

*   `src/`: Source code.
    *   `DatabaseManager`: SQLite handling.
    *   `AcademicManager`: Business logic.
    *   `AIClient`: LLM integration.
    *   `MainWindow` & `*View`: GUI components.
*   `tests/`: QTest unit tests.
*   `AGENTS.md`: Detailed documentation for AI Agents and architecture.

## License

This project is licensed under the MIT License.
