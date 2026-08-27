#include "models.h"

int loadProject(Project *project, char *boardFileName, char *taskFileName){
    if(readBoard(project, boardFileName) == 1){
        return 1;
    }
    if(readAllTasks(project, taskFileName) == 1){
        return 1;
    }
    return 0;
}
int readBoard(Project *project, char *fileName){
    FILE *file_ptr = fopen(fileName, "r");

    if (file_ptr == NULL) {
        // File doesn't exist, let's create it!
        printf("File not found. Creating a new one...\n");
        file_ptr = fopen(fileName, "w+"); // w+ creates it and allows reading
    }

    if (file_ptr == NULL)
        return 1;

    char buffer[255];
    int i = 0;
    int fd = fileno(file_ptr);
    flock(fd, LOCK_EX);
    while (fgets(buffer, sizeof(buffer), file_ptr) != NULL && i < MAX_BOARD)
    {
        // 1. Remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        // 2. Correct Tokenizing
        char *boardId = strtok(buffer, ",");
        char *boardTitle = strtok(NULL, ",");

        // 3. Check if tokens exist before using them
        if (boardId != NULL && boardTitle != NULL)
        {
            strcpy(project->boards[i].boardId, boardId);
            strcpy(project->boards[i].boardTitle, boardTitle);
            i++;
        }
    }
    project->totalBoards = i;
    flock(fd, LOCK_UN);
    fclose(file_ptr);
    return 0;
}
int saveBoard(Project *project,  char *fileName){
    FILE *file_ptr;

    file_ptr = fopen(fileName, "w");

    if (file_ptr == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }
    int fd = fileno(file_ptr);
    flock(fd, LOCK_EX);
    Board *boards = project->boards;
    for (int i = 0; i < MAX_BOARD; i++)
    {
        fprintf(file_ptr, "%s,%s\n", boards[i].boardId, boards[i].boardTitle);
    }
    flock(fd, LOCK_UN);
    fclose(file_ptr);
    return 0;
}
int readAllTasks(Project *project, char *fileName){
    FILE *file_ptr = fopen(fileName, "r");

    if (file_ptr == NULL) {
        printf("File not found. Creating a new one...\n");
        file_ptr = fopen(fileName, "w+");
    }

    if (file_ptr == NULL)
        return 1;

    char buffer[255];
    int fd = fileno(file_ptr);
    flock(fd, LOCK_EX);

    while (fgets(buffer, sizeof(buffer), file_ptr) != NULL)
    {
        // 1. Remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        // 2. Tokenize
        char *boardId = strtok(buffer, ",");
        char *taskId = strtok(NULL, ",");
        char *taskTitle = strtok(NULL, ",");
        char *isFinishStr = strtok(NULL, ",");

        if (boardId == NULL || taskId == NULL || taskTitle == NULL || isFinishStr == NULL)
            continue; // skip malformed line

        // 3. Find the board with this boardId
        Board *targetBoard = NULL;
        for (int b = 0; b < project->totalBoards; b++)
        {
            if (strcmp(project->boards[b].boardId, boardId) == 0)
            {
                targetBoard = &project->boards[b];
                break;
            }
        }

        if (targetBoard == NULL)
            continue; // task belongs to a board we don't have (shouldn't normally happen)

        // 4. Add task to that board (bounds-checked)
        if (targetBoard->totalTasks < MAX_TASK)
        {
            int i = targetBoard->totalTasks;
            strcpy(targetBoard->tasks[i].taskId, taskId);
            strcpy(targetBoard->tasks[i].taskTitle, taskTitle);
            targetBoard->tasks[i].isFinish = atoi(isFinishStr);
            targetBoard->totalTasks++;
        }
    }

    flock(fd, LOCK_UN);
    fclose(file_ptr);
    return 0;
}
int saveAllTasks(Project *project, char *fileName){
    FILE *file_ptr = fopen(fileName, "w");
    if (file_ptr == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    int fd = fileno(file_ptr);
    flock(fd, LOCK_EX);

    for (int b = 0; b < project->totalBoards; b++) {
        Board *board = &project->boards[b];
        for (int i = 0; i < board->totalTasks; i++) {
            fprintf(file_ptr, "%s,%s,%s,%d\n",
                    board->boardId,
                    board->tasks[i].taskId,
                    board->tasks[i].taskTitle,
                    board->tasks[i].isFinish);
        }
    }

    flock(fd, LOCK_UN);
    fclose(file_ptr);
    return 0;
}
int addBoard(Project *project, char *boardTitle){
    
    snprintf(project->boards[project->totalBoards].boardId, 
            sizeof(project->boards[project->totalBoards].boardId), 
            "%d", project->totalBoards+1);
    strcpy(project->boards[project->totalBoards].boardTitle, boardTitle);
    project->totalBoards++;
    return 0;
}
int addTask(Board *board, char *taskTitle){
    snprintf(board->tasks[board->totalTasks].taskId, sizeof(board->tasks[board->totalTasks].taskId), "%d", board->totalTasks + 1);
    strcpy(board->tasks[board->totalTasks].taskTitle, taskTitle);
    board->totalTasks++;
    return 0;
}