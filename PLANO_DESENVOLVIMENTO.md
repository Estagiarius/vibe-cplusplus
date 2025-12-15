# Plano de Desenvolvimento: Melhorias no Sistema de Gestão Acadêmica

Este documento descreve o plano de trabalho para uma equipe de 3 desenvolvedores, visando melhorar a experiência do Professor no sistema. O trabalho foi dividido para maximizar a produtividade e minimizar conflitos de merge (git), separando as responsabilidades por módulos funcionais.

## Estratégia de Desenvolvimento

*   **Branches:** Cada desenvolvedor deve trabalhar em uma branch separada (`feature/gradebook`, `feature/management`, `feature/ai`).
*   **Ponto Crítico:** O arquivo `src/DatabaseManager.cpp` é um recurso compartilhado.
    *   **Mitigação:** Recomenda-se que novos métodos SQL sejam adicionados em seções claramente marcadas ou no final do arquivo. Se possível, coordenem as alterações de schema (criação de tabelas) no início do sprint.

---

## Desenvolvedor 1: Avaliação e Acompanhamento (Gradebook)

**Objetivo:** Tornar o processo de lançamento de notas mais eficiente e introduzir o controle de frequência.

**Arquivos Principais:** `src/GradebookView.cpp`, `src/GradebookView.h`.
**Arquivos Secundários:** `src/DatabaseManager.cpp` (Schema de Frequência).

### Tarefas:

1.  **Exportação de Dados (CSV):**
    *   Adicionar um botão "Exportar Notas" na `GradebookView`.
    *   Implementar a lógica para gerar um arquivo `.csv` contendo: Nome do Aluno, Matrícula, Notas (B1-B4), Média e Situação.
    *   *Valor para o Professor:* Facilita o backup e uso de dados em outras ferramentas (Excel).

2.  **Controle de Frequência (Novo Recurso):**
    *   **Banco de Dados:** Criar tabela `attendance` (id, student_id, class_id, date, present).
    *   **UI:** Adicionar uma nova aba ou modo na `GradebookView` para "Chamada".
    *   **Lógica:** Permitir marcar presença/falta para toda a turma em uma data específica.

3.  **Melhorias Visuais no Lançamento:**
    *   Implementar navegação via teclado (setas) na tabela de notas para agilizar a digitação.
    *   Destacar automaticamente notas abaixo da média (ex: < 6.0) em vermelho enquanto digita.

---

## Desenvolvedor 2: Gestão Administrativa (Management)

**Objetivo:** Dar autonomia total ao professor para gerenciar seus dados (CRUD Completo) e encontrar informações rapidamente.

**Arquivos Principais:** `src/ManagementWidgets.cpp`, `src/ManagementWidgets.h`.
**Arquivos Secundários:** `src/AcademicManager.cpp` (Regras de validação).

### Tarefas:

1.  **Menu de Contexto (Editar/Excluir):**
    *   Implementar "Right-Click" nas tabelas de Alunos, Cursos e Turmas.
    *   **Ações:**
        *   *Editar:* Abrir um diálogo pré-preenchido para alterar dados.
        *   *Excluir:* Remover o registro (com confirmação). **Atenção:** Garantir integridade referencial (ex: não excluir curso se houver turmas).

2.  **Busca e Filtragem:**
    *   Adicionar uma barra de pesquisa (`QLineEdit`) no topo de `StudentView` e `CourseView`.
    *   Implementar filtro em tempo real (filtrar `QSortFilterProxyModel` ou recarregar dados) por nome ou matrícula.
    *   *Valor para o Professor:* Encontrar alunos rapidamente em listas grandes.

3.  **Validação de Dados:**
    *   Impedir cadastro de alunos com mesma matrícula.
    *   Impedir cadastro de turmas duplicadas (mesmo curso/semestre).
    *   Exibir mensagens de erro amigáveis (usando `QMessageBox`).

---

## Desenvolvedor 3: Inteligência Artificial e Configuração

**Objetivo:** Transformar a IA em um assistente pessoal configurável e melhorar a qualidade dos insights.

**Arquivos Principais:** `src/AIClient.cpp`, `src/AIClient.h`, `src/MainWindow.cpp`.
**Arquivos Secundários:** `src/AcademicManager.cpp` (Novas funções de relatório).

### Tarefas:

1.  **Menu de Configurações:**
    *   Criar um diálogo de configurações (acessível via Menu Bar).
    *   Permitir configurar:
        *   **Provedor IA:** (Ollama Local ou OpenAI).
        *   **Endpoint URL:** (ex: `http://localhost:11434...` ou `https://api.openai.com...`).
        *   **API Key:** Campo seguro para chave de API (salvar em `QSettings`).

2.  **Relatórios Detalhados (Prompt Engineering):**
    *   Melhorar o System Prompt no `AIClient` para agir como um "Pedagogo Sênior".
    *   Criar nova tool para a IA: `analyze_class_performance(class_id)`.
        *   Esta função deve agregar dados de média da turma, desvio padrão e lista de alunos em recuperação, permitindo que a IA gere um relatório textual (ex: "A turma está com dificuldade no 2º bimestre...").

3.  **Interface de Chat Aprimorada:**
    *   Adicionar indicador de "Digitando..." ou "Processando..." visual.
    *   Permitir copiar a resposta da IA para a área de transferência.

---

## Resumo da Divisão

| Desenvolvedor | Foco Principal | Risco de Conflito | Mitigação |
| :--- | :--- | :--- | :--- |
| **Dev 1** | Notas, Frequência, CSV | Baixo | Trabalha isolado em `GradebookView`. |
| **Dev 2** | CRUD, Busca, UI Geral | Médio (`DatabaseManager`) | Focar em `ManagementWidgets`; adicionar `delete` queries com cuidado. |
| **Dev 3** | IA, Configuração, Settings | Baixo | Trabalha isolado em `AIClient`/`MainWindow`. |
