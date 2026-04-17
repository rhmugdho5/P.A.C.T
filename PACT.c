#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_DESC 200
#define MAX_DATE 20
#define MAX_TASKS 100
#define MAX_CODE 20
#define MAX_PROGRAMS 100

//HERE'RE THE CONSTANTS WE'LL BE USING

#define FILE_PROGRAMS "programs.txt"
#define FILE_TASKS "tasks.txt"
#define FILE_USERS "users.txt"
#define FILE_ADMINS "admins.txt"
#define FILE_SUBSCRIPTIONS "subscriptions.txt"
#define FILE_ADMIN_PROGRAMS "admin_programs.txt"


// STRUCTURES

typedef struct {
    char username[MAX_NAME];
    char password[MAX_NAME];
    char name[MAX_NAME];
    char email[MAX_NAME];
} User;

typedef struct {
    int id;
    char name[MAX_NAME];
    char adminName[MAX_NAME];
    char description[MAX_DESC];
    char joinCode[MAX_CODE];
    int isAdminProgram;
    char creatorUsername[MAX_NAME];
} Program;

typedef struct {
    int taskId;
    int programId;
    char username[MAX_NAME];
    char title[MAX_NAME];
    char deadline[MAX_DATE];
    int isCompleted;
    int isPenalty;
} Task;

typedef struct {
    char username[MAX_NAME];
    int programId;
} Subscription;

typedef struct {
    char username[MAX_NAME];
    char password[MAX_NAME];
    int programId;
} Admin;

// GLOBAL VARIABLES :D

User currentUser;
Admin currentAdmin;
int isUserLoggedIn = 0;
int isAdminLoggedIn = 0;

// Function declarations
void viewMySubscriptions();
void viewMyPersonalPrograms();
void viewAdminPrograms();

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int getIntInput(char *prompt) {
    char buffer[20];
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &value) == 1) {
                return value;
            }
        }
        printf("[!] Invalid input. Please enter a number.\n");
    }
}

int isUserSubscribed(char *username, int programId) {
    FILE *f = fopen(FILE_SUBSCRIPTIONS, "r");
    if (!f) return 0;

    Subscription s;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|%d", s.username, &s.programId);
        if (strcmp(s.username, username) == 0 && s.programId == programId) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int userExists(char *username) {
    FILE *f = fopen(FILE_USERS, "r");
    if (!f) return 0;
    User u;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|", u.username);
        if (strcmp(username, u.username) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void getInput(char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

void printLine() {
    printf("--------------------------------------------------------------------------------\n");
}

void clearScreen() {
    printf("\n\n");
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
}

int getNextProgramId() {
    FILE *fp = fopen(FILE_PROGRAMS, "r");
    if (!fp) return 1;
    int maxId = 0;
    Program p;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {       // Reads until new line occurs or 511 character limit reached and stores in line
        sscanf(line, "%d|", &p.id);             // gets integer value that is before the pipe character '|' and later stores into p.id structure's element
        if (p.id > maxId) maxId = p.id;
    }
    fclose(fp);
    return maxId + 1;   //successfully returns the ID of the next program, which should be one value more than the existing id
}

int getNextTaskId() {    //same logic throughout
    FILE *fp = fopen(FILE_TASKS, "r");
    if (!fp) return 1;
    int maxId = 0;
    Task t;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d|", &t.taskId);
        if (t.taskId > maxId) maxId = t.taskId;
    }
    fclose(fp);
    return maxId + 1;
}

int programExists(int programId) {
    FILE *fp = fopen(FILE_PROGRAMS, "r");
    if (!fp) return 0;
    Program p;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d|", &p.id);
        if (p.id == programId) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void saveAdminProgramLink(char *adminUsername, int programId) {
    FILE *f = fopen(FILE_ADMIN_PROGRAMS, "a");
    if (f) {
        fprintf(f, "%s|%d\n", adminUsername, programId);
        fclose(f);
    }
}

int getAdminProgramCount(char *adminUsername) {
    FILE *f = fopen(FILE_ADMIN_PROGRAMS, "r");
    if (!f) return 0;
    int count = 0;
    char username[MAX_NAME];
    int programId;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|%d", username, &programId);
        if (strcmp(username, adminUsername) == 0) {
            count++;
        }
    }
    fclose(f);
    return count;
}
//The codes above were written by Rejowan
// ========================================
// 1: USER AUTHENTICATION (Apurba Krishna)
// ========================================

void saveUser(User u) {
    FILE *f = fopen(FILE_USERS, "a");
    if (!f) {
        printf("Error: Cannot save user data.\n");
        return;
    }
    fprintf(f, "%s|%s|%s|%s\n", u.username, u.password, u.name, u.email);
    fclose(f);
}

int userLogin() {
    char username[MAX_NAME], password[MAX_NAME];
    getInput("Username: ", username, MAX_NAME);
    getInput("Password: ", password, MAX_NAME);
    FILE *f = fopen(FILE_USERS, "r");
    if (!f) {
        printf("[!] No user database found. Please register first.\n");
        return 0;
    }
    User u;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^\n]", u.username, u.password, u.name, u.email);
        if (strcmp(username, u.username) == 0 && strcmp(password, u.password) == 0) {
            currentUser = u;
            isUserLoggedIn = 1;
            fclose(f);
            printf(">>> Login successful! Welcome %s.\n", u.name);
            return 1;
        }
    }
    fclose(f);
    printf("[!] Login failed. Invalid credentials.\n");
    return 0;
}

void registerUser() {
    User u;
    printf("\n=== USER REGISTRATION ===\n");
    getInput("Choose username: ", u.username, MAX_NAME);
    FILE *f = fopen(FILE_USERS, "r");
    if (f) {
        User existing;
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            sscanf(line, "%[^|]|", existing.username);
            if (strcmp(u.username, existing.username) == 0) {
                printf("[!] Username already exists. Please choose another.\n");
                fclose(f);
                return;
            }
        }
        fclose(f);
    }
    getInput("Choose password: ", u.password, MAX_NAME);
    getInput("Enter full name: ", u.name, MAX_NAME);
    getInput("Enter email:     ", u.email, MAX_NAME);
    saveUser(u);
    printf(">>> Registration successful! You can now login.\n");
}

void showProfile() {
    printf("\n=== USER PROFILE ===\n");
    printf("Username: %s\n", currentUser.username);
    printf("Name:     %s\n", currentUser.name);
    printf("Email:    %s\n", currentUser.email);
    printLine();
}

// ===================================================
// 2: PROGRAM & TASK MANAGEMENT (Rejoan Hasan Mugdho)
// ===================================================

void createAdminProgram() {
    if (getAdminProgramCount(currentAdmin.username) >= 3) {
        printf("\n[!] You have reached the maximum limit of 3 programs.\n");
        return;
    }

    Program p;
    FILE *fp = fopen(FILE_PROGRAMS, "a");
    if (fp == NULL) {
        perror("Error opening programs file");
        return;
    }
    printf("\n=== CREATE NEW ADMIN PROGRAM ===\n");
    p.id = getNextProgramId();
    printf("Auto-assigned Program ID: %d\n", p.id);
    getInput("Enter Program Name: ", p.name, MAX_NAME);
    getInput("Enter Description:  ", p.description, MAX_DESC);
    getInput("Create Join Code:   ", p.joinCode, MAX_CODE);
    strcpy(p.adminName, currentAdmin.username);
    p.isAdminProgram = 1;
    strcpy(p.creatorUsername, "ADMIN");


    fprintf(fp, "%d|%s|%s|%s|%s|%d|%s\n",
            p.id, p.name, p.adminName, p.description, p.joinCode, p.isAdminProgram, p.creatorUsername);
    fclose(fp);
    saveAdminProgramLink(currentAdmin.username, p.id);
    currentAdmin.programId = p.id;
    printf(">>> Success: Admin program '%s' created with ID %d\n", p.name, p.id);
    printf(">>> Users will need join code '%s' to subscribe\n", p.joinCode);
}

void createUserProgram() {
    Program p;
    FILE *fp = fopen(FILE_PROGRAMS, "a");
    if (fp == NULL) {
        perror("Error opening programs file");
        return;
    }
    printf("\n=== CREATE PERSONAL PROGRAM ===\n");
    p.id = getNextProgramId();
    printf("Auto-assigned Program ID: %d\n", p.id);
    getInput("Enter Program Name: ", p.name, MAX_NAME);
    getInput("Enter Description:  ", p.description, MAX_DESC);
    strcpy(p.adminName, "Self");
    strcpy(p.joinCode, "NONE");
    p.isAdminProgram = 0;
    strcpy(p.creatorUsername, currentUser.username);

    // Write with format
    fprintf(fp, "%d|%s|%s|%s|%s|%d|%s\n",
            p.id, p.name, p.adminName, p.description, p.joinCode, p.isAdminProgram, p.creatorUsername);
    fclose(fp);

    // Auto-subscribe user to their own program
    FILE *sf = fopen(FILE_SUBSCRIPTIONS, "a");
    if (sf) {
        fprintf(sf, "%s|%d\n", currentUser.username, p.id);
        fclose(sf);
    }
    printf(">>> Success: Personal program '%s' created with ID %d\n", p.name, p.id);
    printf(">>> Creator: %s (this is YOUR program)\n", p.creatorUsername);
}

void viewPublicPrograms() {
    Program p;
    FILE *fp = fopen(FILE_PROGRAMS, "r");
    if (fp == NULL) {
        printf("\n[!] No programs available.\n");
        return;
    }
    printf("\n=== AVAILABLE PUBLIC PROGRAMS ===\n");
    printf("%-5s | %-25s | %-15s | %s\n", "ID", "PROGRAM NAME", "ADMIN", "DESCRIPTION");
    printLine();
    int found = 0;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|%[^\n]",
               &p.id, p.name, p.adminName, p.description, p.joinCode, &p.isAdminProgram, p.creatorUsername);
        if (p.isAdminProgram == 1) {
            printf("%-5d | %-25s | %-15s | %s\n", p.id, p.name, p.adminName, p.description);
            found = 1;
        }
    }
    fclose(fp);
    if (!found) {
        printf("[!] No public programs available yet.\n");
    }
    printLine();
}

void viewMyPersonalPrograms() {
    Program p;
    FILE *fp = fopen(FILE_PROGRAMS, "r");
    if (fp == NULL) {
        printf("\n[!] No programs found.\n");
        return;
    }
    printf("\n=== MY PERSONAL PROGRAMS ===\n");
    printf("%-5s | %-25s | %s\n", "ID", "PROGRAM NAME", "DESCRIPTION");
    printLine();
    int found = 0;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        // Initialize all fields
        memset(&p, 0, sizeof(Program));

        int scanResult = sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|%[^\n]",
               &p.id, p.name, p.adminName, p.description, p.joinCode, &p.isAdminProgram, p.creatorUsername);



        if (scanResult >= 6 && p.isAdminProgram == 0 &&
            strlen(p.creatorUsername) > 0 &&
            strcmp(p.creatorUsername, currentUser.username) == 0) {
            printf("%-5d | %-25s | %s\n", p.id, p.name, p.description);
            found = 1;
        }
    }
    fclose(fp);
    if (!found) {
        printf("[!] You haven't created any personal programs yet.\n");
    }
    printLine();
}

void viewAdminPrograms() {
    FILE *f = fopen(FILE_ADMIN_PROGRAMS, "r");
    if (!f) {
        printf("\n[!] You haven't created any programs yet.\n");
        return;
    }
    printf("\n=== YOUR ADMIN PROGRAMS ===\n");
    printf("%-5s | %-25s | %s\n", "ID", "PROGRAM NAME", "DESCRIPTION");
    printLine();
    int found = 0;
    char username[MAX_NAME];
    int programId;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|%d", username, &programId);
        if (strcmp(username, currentAdmin.username) == 0) {
            FILE *pf = fopen(FILE_PROGRAMS, "r");
            if (pf) {
                Program p;
                char pline[512];
                while (fgets(pline, sizeof(pline), pf)) {
                    sscanf(pline, "%d|%[^|]|%[^|]|%[^|]|", &p.id, p.name, p.adminName, p.description);
                    if (p.id == programId) {
                        printf("%-5d | %-25s | %s\n", p.id, p.name, p.description);
                        found = 1;
                        break;
                    }
                }
                fclose(pf);
            }
        }
    }
    fclose(f);
    if (!found) {
        printf("[!] You haven't created any programs yet.\n");
    }
    printLine();
}

int selectAdminProgram() {
    viewAdminPrograms();
    printf("\nEnter Program ID to manage (0 to cancel): ");
    int pid = getIntInput("");
    if (pid == 0) return 0;

    FILE *f = fopen(FILE_ADMIN_PROGRAMS, "r");
    if (!f) {
        printf("[!] No programs found.\n");
        return 0;
    }
    char username[MAX_NAME];
    int programId;
    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|%d", username, &programId);
        if (strcmp(username, currentAdmin.username) == 0 && programId == pid) {
            found = 1;
            break;
        }
    }
    fclose(f);
    if (!found) {
        printf("[!] Invalid program ID or not your program.\n");
        return 0;
    }
    currentAdmin.programId = pid;
    printf(">>> Now managing Program ID: %d\n", pid);
    return 1;
}

void createTaskForUser() {
    Task t;
    char targetUser[MAX_NAME];
    printf("\n=== ASSIGN TASK TO USER ===\n");

    if (getAdminProgramCount(currentAdmin.username) == 0) {
        printf("[!] You must create a program first.\n");
        return;
    }

    if (!selectAdminProgram()) return;

    getInput("Target User Username (0 to cancel): ", targetUser, MAX_NAME);
    if (strcmp(targetUser, "0") == 0) return;

    if (!userExists(targetUser)) {
        printf("[!] User '%s' does not exist.\n", targetUser);
        return;
    }

    // CHECK IF USER IS SUBSCRIBED TO THIS PROGRAM
    if (!isUserSubscribed(targetUser, currentAdmin.programId)) {
        printf("[!] User '%s' is not subscribed to this program (ID: %d).\n", targetUser, currentAdmin.programId);
        printf("[!] Users must subscribe to your program before you can assign tasks.\n");
        return;
    }

    t.programId = currentAdmin.programId;
    t.taskId = getNextTaskId();
    printf("Auto-assigned Task ID: %d\n", t.taskId);
    getInput("Task Title:       ", t.title, MAX_NAME);
    getInput("Deadline (DD/MM): ", t.deadline, MAX_DATE);
    strcpy(t.username, targetUser);
    t.isCompleted = 0;
    t.isPenalty = 0;
    FILE *fp = fopen(FILE_TASKS, "a");
    if (!fp) {
        printf("Error creating task.\n");
        return;
    }
    fprintf(fp, "%d|%d|%s|%s|%s|%d|%d\n", t.taskId, t.programId, t.username, t.title, t.deadline, t.isCompleted, t.isPenalty);
    fclose(fp);
    printf(">>> Task assigned to user '%s' successfully!\n", targetUser);
}

void createPersonalTask() {
    Task t;
    int programId;
    printf("\n=== CREATE PERSONAL TASK ===\n");
    viewMyPersonalPrograms();

    int hasPersonalPrograms = 0;
    FILE *checkFp = fopen(FILE_PROGRAMS, "r");
    if (checkFp) {
        Program p;
        char line[512];
        while (fgets(line, sizeof(line), checkFp)) {
            memset(&p, 0, sizeof(Program));
            int scanResult = sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|%[^\n]",
                   &p.id, p.name, p.adminName, p.description, p.joinCode, &p.isAdminProgram, p.creatorUsername);
            if (scanResult >= 6 && p.isAdminProgram == 0 &&
                strlen(p.creatorUsername) > 0 &&
                strcmp(p.creatorUsername, currentUser.username) == 0) {
                hasPersonalPrograms = 1;
                break;
            }
        }
        fclose(checkFp);
    }
    if (!hasPersonalPrograms) {
        printf("\n[!] You have no personal programs. Create one first!\n");
        return;
    }
    printf("\nEnter Program ID for this task (0 to cancel): ");
    programId = getIntInput("");
    if (programId == 0) return;

    FILE *fp = fopen(FILE_PROGRAMS, "r");
    if (!fp) {
        printf("[!] Cannot access programs.\n");
        return;
    }
    Program p;
    int validProgram = 0;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        memset(&p, 0, sizeof(Program));
        int scanResult = sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|%[^\n]",
               &p.id, p.name, p.adminName, p.description, p.joinCode, &p.isAdminProgram, p.creatorUsername);
        if (scanResult >= 6 && p.id == programId && p.isAdminProgram == 0 &&
            strlen(p.creatorUsername) > 0 &&
            strcmp(p.creatorUsername, currentUser.username) == 0) {
            validProgram = 1;
            break;
        }
    }
    fclose(fp);
    if (!validProgram) {
        printf("[!] Invalid program ID or not your personal program.\n");
        return;
    }
    t.programId = programId;
    t.taskId = getNextTaskId();
    printf("Auto-assigned Task ID: %d\n", t.taskId);
    getInput("Task Title:       ", t.title, MAX_NAME);
    getInput("Deadline (DD/MM): ", t.deadline, MAX_DATE);
    strcpy(t.username, currentUser.username);
    t.isCompleted = 0;
    t.isPenalty = 0;
    fp = fopen(FILE_TASKS, "a");
    if (!fp) {
        printf("Error creating task.\n");
        return;
    }
    fprintf(fp, "%d|%d|%s|%s|%s|%d|%d\n", t.taskId, t.programId, t.username, t.title, t.deadline, t.isCompleted, t.isPenalty);
    fclose(fp);
    printf(">>> Personal task created successfully!\n");
}

void viewMyTasks() {
    FILE *fp = fopen(FILE_TASKS, "r");
    if (!fp) {
        printf("\n[!] No tasks found.\n");
        return;
    }
    printf("\n=== MY TASKS ===\n");
    printf("%-5s | %-8s | %-25s | %-10s | %-10s | %s\n", "ID", "PROG ID", "TITLE", "DEADLINE", "STATUS", "TYPE");
    printLine();
    int found = 0;
    Task t;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d|%d|%[^|]|%[^|]|%[^|]|%d|%d",
               &t.taskId, &t.programId, t.username, t.title, t.deadline, &t.isCompleted, &t.isPenalty);
        if (strcmp(t.username, currentUser.username) == 0) {
            printf("%-5d | %-8d | %-25s | %-10s | %-10s | %s\n",
                   t.taskId, t.programId, t.title, t.deadline,
                   (t.isCompleted ? "DONE" : "PENDING"),
                   (t.isPenalty ? "!PENALTY!" : "Normal"));
            found = 1;
        }
    }
    fclose(fp);
    if (!found) {
        printf("[!] You have no tasks assigned.\n");
    }
    printLine();
}

void markTaskComplete() {
    Task tasks[MAX_TASKS];
    int count = 0, searchId, found = 0;
    char line[512];
    FILE *fp = fopen(FILE_TASKS, "r");
    if (!fp) {
        printf("\n[!] No tasks available.\n");
        return;
    }
    while (fgets(line, sizeof(line), fp) && count < MAX_TASKS) {
        sscanf(line, "%d|%d|%[^|]|%[^|]|%[^|]|%d|%d",
               &tasks[count].taskId, &tasks[count].programId, tasks[count].username,
               tasks[count].title, tasks[count].deadline, &tasks[count].isCompleted, &tasks[count].isPenalty);
        count++;
    }
    fclose(fp);
    viewMyTasks();
    printf("\nEnter Task ID to mark complete (0 to cancel): ");
    searchId = getIntInput("");
    if (searchId == 0) return;

    for (int i = 0; i < count; i++) {
        if (tasks[i].taskId == searchId && strcmp(tasks[i].username, currentUser.username) == 0) {
            if (tasks[i].isCompleted == 1) {
                printf("[!] This task is already marked as completed.\n");
                return;
            }
            tasks[i].isCompleted = 1;
            found = 1;
            printf(">>> Task '%s' marked as COMPLETED!\n", tasks[i].title);
            break;
        }
    }
    if (!found) {
        printf("[!] Task ID %d not found or not assigned to you.\n", searchId);
        return;
    }
    fp = fopen(FILE_TASKS, "w");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d|%d|%s|%s|%s|%d|%d\n",
                tasks[i].taskId, tasks[i].programId, tasks[i].username, tasks[i].title,
                tasks[i].deadline, tasks[i].isCompleted, tasks[i].isPenalty);
    }
    fclose(fp);
}

void unsubscribeFromProgram() {
    printf("\n=== UNSUBSCRIBE FROM PROGRAM ===\n");
    viewMySubscriptions();
    printf("\nEnter Program ID to unsubscribe (0 to cancel): ");
    int pid = getIntInput("");
    if (pid == 0) return;

    Subscription subs[MAX_PROGRAMS];
    int count = 0;
    FILE *f = fopen(FILE_SUBSCRIPTIONS, "r");
    if (!f) {
        printf("[!] No subscriptions found.\n");
        return;
    }
    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), f) && count < MAX_PROGRAMS) {
        sscanf(line, "%[^|]|%d", subs[count].username, &subs[count].programId);
        if (strcmp(subs[count].username, currentUser.username) == 0 && subs[count].programId == pid) {
            found = 1;
        } else {
            count++;
        }
    }
    fclose(f);
    if (!found) {
        printf("[!] You are not subscribed to program ID %d.\n", pid);
        return;
    }
    f = fopen(FILE_SUBSCRIPTIONS, "w");
    for (int i = 0; i < count; i++) {
        fprintf(f, "%s|%d\n", subs[i].username, subs[i].programId);
    }
    fclose(f);
    printf(">>> Successfully unsubscribed from Program ID %d!\n", pid);
}

void deleteAdminProgram() {
    printf("\n=== DELETE ADMIN PROGRAM ===\n");
    viewAdminPrograms();
    printf("\nEnter Program ID to delete (0 to cancel): ");
    int pid = getIntInput("");
    if (pid == 0) return;

    Program programs[MAX_PROGRAMS];
    int pcount = 0;
    FILE *fp = fopen(FILE_PROGRAMS, "r");
    if (!fp) {
        printf("[!] No programs found.\n");
        return;
    }
    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), fp) && pcount < MAX_PROGRAMS) {
        sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|%[^\n]",
               &programs[pcount].id, programs[pcount].name, programs[pcount].adminName,
               programs[pcount].description, programs[pcount].joinCode,
               &programs[pcount].isAdminProgram, programs[pcount].creatorUsername);
        if (programs[pcount].id == pid && strcmp(programs[pcount].adminName, currentAdmin.username) == 0) {
            found = 1;
        } else {
            pcount++;
        }
    }
    fclose(fp);
    if (!found) {
        printf("[!] Program not found or not yours to delete.\n");
        return;
    }
    fp = fopen(FILE_PROGRAMS, "w");
    for (int i = 0; i < pcount; i++) {
        fprintf(fp, "%d|%s|%s|%s|%s|%d|%s\n",
                programs[i].id, programs[i].name, programs[i].adminName,
                programs[i].description, programs[i].joinCode,
                programs[i].isAdminProgram, programs[i].creatorUsername);
    }
    fclose(fp);

    char aplinks[MAX_PROGRAMS][256];
    int apcount = 0;
    FILE *af = fopen(FILE_ADMIN_PROGRAMS, "r");
    if (af) {
        char username[MAX_NAME];
        int programId;
        while (fgets(line, sizeof(line), af) && apcount < MAX_PROGRAMS) {
            sscanf(line, "%[^|]|%d", username, &programId);
            if (!(strcmp(username, currentAdmin.username) == 0 && programId == pid)) {
                strcpy(aplinks[apcount++], line);
            }
        }
        fclose(af);
        af = fopen(FILE_ADMIN_PROGRAMS, "w");
        for (int i = 0; i < apcount; i++) {
            fprintf(af, "%s", aplinks[i]);
        }
        fclose(af);
    }
    printf(">>> Program ID %d deleted successfully!\n", pid);
}

// ================================================
// 3: ADMIN SYSTEM & PENALTY LOGIC (Ashfaqul Alam)
// ================================================


int adminLogin() {
    char username[MAX_NAME], password[MAX_NAME];
    getInput("Admin Username: ", username, MAX_NAME);
    getInput("Admin Password: ", password, MAX_NAME);
    FILE *f = fopen(FILE_ADMINS, "r");
    if (!f) {
        printf("[!] No admin database found.\n");
        return 0;
    }
    Admin a;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|%[^|]|%d", a.username, a.password, &a.programId);
        if (strcmp(username, a.username) == 0 && strcmp(password, a.password) == 0) {
            currentAdmin = a;
            isAdminLoggedIn = 1;
            fclose(f);
            printf(">>> Admin login successful!\n");
            return 1;
        }
    }
    fclose(f);
    printf("[!] Admin login failed.\n");
    return 0;
}

void createAdminAccount() {
    Admin a;
    printf("\n=== CREATE ADMIN ACCOUNT ===\n");
    getInput("Admin Username: ", a.username, MAX_NAME);
    FILE *f = fopen(FILE_ADMINS, "r");
    if (f) {
        Admin existing;
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            sscanf(line, "%[^|]|", existing.username);
            if (strcmp(a.username, existing.username) == 0) {
                printf("[!] Admin username already exists.\n");
                fclose(f);
                return;
            }
        }
        fclose(f);
    }
    getInput("Admin Password: ", a.password, MAX_NAME);
    a.programId = 0;
    f = fopen(FILE_ADMINS, "a");
    if (!f) {
        printf("Error creating admin account.\n");
        return;
    }
    fprintf(f, "%s|%s|%d\n", a.username, a.password, a.programId);
    fclose(f);
    printf(">>> Admin account created successfully!\n");
    printf(">>> Login and create your programs to start managing.\n");
}

void assignPenaltyTask() {
    Task t;
    char targetUser[MAX_NAME];
    printf("\n=== ASSIGN PENALTY TASK ===\n");

    if (getAdminProgramCount(currentAdmin.username) == 0) {
        printf("[!] You must create a program first.\n");
        return;
    }

    if (!selectAdminProgram()) return;

    getInput("Target User Username (0 to cancel): ", targetUser, MAX_NAME);
    if (strcmp(targetUser, "0") == 0) return;

    if (!userExists(targetUser)) {
        printf("[!] User '%s' does not exist.\n", targetUser);
        return;
    }

    // CHeck if user subbed to program
    if (!isUserSubscribed(targetUser, currentAdmin.programId)) {
        printf("[!] User '%s' is not subscribed to this program (ID: %d).\n", targetUser, currentAdmin.programId);
        printf("[!] Users must subscribe to your program before you can assign penalty tasks.\n");
        return;
    }

    t.programId = currentAdmin.programId;
    t.taskId = getNextTaskId();
    printf("Auto-assigned Penalty Task ID: %d\n", t.taskId);
    getInput("Penalty Task Title: ", t.title, MAX_NAME);
    getInput("Deadline (DD/MM):   ", t.deadline, MAX_DATE);
    strcpy(t.username, targetUser);
    t.isCompleted = 0;
    t.isPenalty = 1;
    FILE *fp = fopen(FILE_TASKS, "a");
    if (!fp) {
        printf("Error assigning penalty task.\n");
        return;
    }
    fprintf(fp, "%d|%d|%s|%s|%s|%d|%d\n", t.taskId, t.programId, t.username, t.title, t.deadline, t.isCompleted, t.isPenalty);
    fclose(fp);
    printf(">>> Penalty task assigned to '%s' for missed deadline!\n", targetUser);
}

void viewProgramSubscribers() {
    printf("\n=== VIEW PROGRAM SUBSCRIBERS ===\n");

    if (getAdminProgramCount(currentAdmin.username) == 0) {
        printf("[!] You must create a program first.\n");
        return;
    }

    if (!selectAdminProgram()) return;

    FILE *f = fopen(FILE_SUBSCRIPTIONS, "r");
    if (!f) {
        printf("\n[!] No subscribers found.\n");
        return;
    }
    printf("\n=== SUBSCRIBERS TO PROGRAM ID: %d ===\n", currentAdmin.programId);
    printLine();
    int found = 0;
    Subscription s;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|%d", s.username, &s.programId);
        if (s.programId == currentAdmin.programId) {
            printf("Username: %s\n", s.username);
            found = 1;
        }
    }
    fclose(f);
    if (!found) {
        printf("[!] No subscribers yet.\n");
    }
    printLine();
}

void viewProgramTaskStatus() {
    printf("\n=== VIEW PROGRAM TASK STATUS ===\n");

    if (getAdminProgramCount(currentAdmin.username) == 0) {
        printf("[!] You must create a program first.\n");
        return;
    }

    if (!selectAdminProgram()) return;

    FILE *fp = fopen(FILE_TASKS, "r");
    if (!fp) {
        printf("\n[!] No tasks found for this program.\n");
        return;
    }

    printf("\n=== TASK STATUS FOR PROGRAM ID: %d ===\n", currentAdmin.programId);
    printf("%-5s | %-12s | %-25s | %-10s | %-10s | %s\n",
           "ID", "USER", "TITLE", "DEADLINE", "STATUS", "TYPE");
    printLine();

    int found = 0;
    Task t;
    char line[512];

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d|%d|%[^|]|%[^|]|%[^|]|%d|%d",
               &t.taskId, &t.programId, t.username, t.title, t.deadline, &t.isCompleted, &t.isPenalty);

        if (t.programId == currentAdmin.programId) {
            printf("%-5d | %-12s | %-25s | %-10s | %-10s | %s\n",
                   t.taskId, t.username, t.title, t.deadline,
                   (t.isCompleted ? "COMPLETED" : "PENDING"),
                   (t.isPenalty ? "PENALTY" : "Normal"));
            found = 1;
        }
    }
    fclose(fp);

    if (!found) {
        printf("[!] No tasks assigned for this program yet.\n");
    }
    printLine();


    fp = fopen(FILE_TASKS, "r");
    if (fp) {
        int totalTasks = 0, completedTasks = 0, pendingTasks = 0;
        while (fgets(line, sizeof(line), fp)) {
            sscanf(line, "%d|%d|%[^|]|%[^|]|%[^|]|%d|%d",
                   &t.taskId, &t.programId, t.username, t.title, t.deadline, &t.isCompleted, &t.isPenalty);
            if (t.programId == currentAdmin.programId) {
                totalTasks++;
                if (t.isCompleted) completedTasks++;
                else pendingTasks++;
            }
        }
        fclose(fp);

        if (totalTasks > 0) {
            printf("\n=== SUMMARY ===\n");
            printf("Total Tasks:     %d\n", totalTasks);
            printf("Completed:       %d\n", completedTasks);
            printf("Pending:         %d\n", pendingTasks);
            printf("Completion Rate: %.1f%%\n", (completedTasks * 100.0) / totalTasks);
            printLine();
        }
    }
}

void subscribeToProgram() {
    int programId;
    char joinCode[MAX_CODE];
    viewPublicPrograms();
    FILE *checkFile = fopen(FILE_PROGRAMS, "r");
    if (!checkFile) {
        printf("\n[!] No programs available to subscribe to.\n");
        return;
    }
    int hasPublicPrograms = 0;
    Program p;
    char line[512];
    while (fgets(line, sizeof(line), checkFile)) {
        sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|",
               &p.id, p.name, p.adminName, p.description, p.joinCode, &p.isAdminProgram);
        if (p.isAdminProgram == 1) {
            hasPublicPrograms = 1;
            break;
        }
    }
    fclose(checkFile);
    if (!hasPublicPrograms) {
        printf("\n[!] No public programs available to subscribe to.\n");
        return;
    }
    printf("\nEnter Program ID to subscribe (0 to cancel): ");
    programId = getIntInput("");
    if (programId == 0) return;

    if (!programExists(programId)) {
        printf("[!] Program ID %d does not exist.\n", programId);
        return;
    }
    FILE *fp = fopen(FILE_PROGRAMS, "r");
    int isPublic = 0;
    char correctCode[MAX_CODE];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|",
               &p.id, p.name, p.adminName, p.description, p.joinCode, &p.isAdminProgram);
        if (p.id == programId) {
            if (p.isAdminProgram == 1) {
                isPublic = 1;
                strcpy(correctCode, p.joinCode);
            }
            break;
        }
    }
    fclose(fp);
    if (!isPublic) {
        printf("[!] This program is not available for public subscription.\n");
        return;
    }
    getInput("Enter Join Code: ", joinCode, MAX_CODE);
    if (strcmp(joinCode, correctCode) != 0) {
        printf("[!] Incorrect join code. Subscription failed.\n");
        return;
    }
    FILE *f = fopen(FILE_SUBSCRIPTIONS, "r");
    if (f) {
        Subscription s;
        while (fgets(line, sizeof(line), f)) {
            sscanf(line, "%[^|]|%d", s.username, &s.programId);
            if (strcmp(s.username, currentUser.username) == 0 && s.programId == programId) {
                printf("[!] You are already subscribed to this program.\n");
                fclose(f);
                return;
            }
        }
        fclose(f);
    }
    f = fopen(FILE_SUBSCRIPTIONS, "a");
    if (!f) {
        printf("Error subscribing to program.\n");
        return;
    }
    fprintf(f, "%s|%d\n", currentUser.username, programId);
    fclose(f);
    printf(">>> Successfully subscribed to Program ID %d!\n", programId);
}

void viewMySubscriptions() {
    FILE *f = fopen(FILE_SUBSCRIPTIONS, "r");
    if (!f) {
        printf("\n[!] You have no subscriptions yet.\n");
        return;
    }
    printf("\n=== MY SUBSCRIPTIONS ===\n");
    printf("%-5s | %-25s | %-15s | %s\n", "ID", "PROGRAM NAME", "TYPE", "ADMIN");
    printLine();
    int found = 0;
    Subscription s;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        sscanf(line, "%[^|]|%d", s.username, &s.programId);
        if (strcmp(s.username, currentUser.username) == 0) {
            FILE *pf = fopen(FILE_PROGRAMS, "r");
            if (pf) {
                Program p;
                char pline[512];
                while (fgets(pline, sizeof(pline), pf)) {
                    sscanf(pline, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d|",
                           &p.id, p.name, p.adminName, p.description, p.joinCode, &p.isAdminProgram);
                    if (p.id == s.programId) {
                        printf("%-5d | %-25s | %-15s | %s\n", p.id, p.name,
                               (p.isAdminProgram ? "Public" : "Personal"), p.adminName);
                        found = 1;
                        break;
                    }
                }
                fclose(pf);
            }
        }
    }
    fclose(f);
    if (!found) {
        printf("[!] You have no subscriptions yet.\n");
    }
    printLine();
}

void userMenu() {
    int choice;
    while (1) {
        clearScreen();
        printf("   P.A.C.T. USER DASHBOARD\n");
        printf("   Logged in as: %s\n", currentUser.username);
        printLine();
        printf("   1. View My Profile\n");
        printf("   2. Browse Public Programs\n");
        printf("   3. Subscribe to Public Program\n");
        printf("   4. Unsubscribe from Program\n");
        printf("   5. Create Personal Program\n");
        printf("   6. View My Personal Programs\n");
        printf("   7. Create Personal Task\n");
        printf("   8. View My Subscriptions\n");
        printf("   9. View All My Tasks\n");
        printf("  10. Mark Task Complete\n");
        printf("   0. Logout\n");
        printLine();
        printf("   Selection > ");
        choice = getIntInput("");
        switch (choice) {
        case 1:
            showProfile();
            pauseScreen();
            break;
        case 2:
            viewPublicPrograms();
            pauseScreen();
            break;
        case 3:
            subscribeToProgram();
            pauseScreen();
            break;
        case 4:
            unsubscribeFromProgram();
            pauseScreen();
            break;
        case 5:
            createUserProgram();
            pauseScreen();
            break;
        case 6:
            viewMyPersonalPrograms();
            pauseScreen();
            break;
        case 7:
            createPersonalTask();
            pauseScreen();
            break;
        case 8:
            viewMySubscriptions();
            pauseScreen();
            break;
        case 9:
            viewMyTasks();
            pauseScreen();
            break;
        case 10:
            markTaskComplete();
            pauseScreen();
            break;
        case 0:
            isUserLoggedIn = 0;
            printf("Logged out successfully.\n");
            return;
        default:
            printf("[!] Invalid selection.\n");
            pauseScreen();
        }
    }
}

void adminMenu() {
    int choice;
    while (1) {
        clearScreen();
        printf("   P.A.C.T. ADMIN DASHBOARD\n");
        printf("   Logged in as: %s\n", currentAdmin.username);
        printLine();
        printf("   1. Create Admin Program (Max 3)\n");
        printf("   2. View My Programs\n");
        printf("   3. Assign Task to User\n");
        printf("   4. Assign Penalty Task\n");
        printf("   5. View Program Subscribers\n");
        printf("   6. View Task Completion Status\n");
        printf("   7. Delete Admin Program\n");
        printf("   8. View All Public Programs\n");
        printf("   0. Logout\n");
        printLine();
        printf("   Selection > ");
        choice = getIntInput("");
        switch (choice) {
        case 1:
            createAdminProgram();
            pauseScreen();
            break;
        case 2:
            viewAdminPrograms();
            pauseScreen();
            break;
        case 3:
            createTaskForUser();
            pauseScreen();
            break;
        case 4:
            assignPenaltyTask();
            pauseScreen();
            break;
        case 5:
            viewProgramSubscribers();
            pauseScreen();
            break;
        case 6:
            viewProgramTaskStatus();
            pauseScreen();
            break;
        case 7:
            deleteAdminProgram();
            pauseScreen();
            break;
        case 8:
            viewPublicPrograms();
            pauseScreen();
            break;
        case 0:
            isAdminLoggedIn = 0;
            printf("Admin logged out successfully.\n");
            return;
        default:
            printf("[!] Invalid selection.\n");
            pauseScreen();
        }
    }
}

int main() {
    int choice;
    while (1) {
        clearScreen();
        printf("\n   ========================================\n");
        printf("   P.A.C.T. SYSTEM - Main Menu\n");
        printf("   Program & Accountability Console Tracker\n");
        printf("   ========================================\n");
        printLine();
        printf("   1. User Registration\n");
        printf("   2. User Login\n");
        printf("   3. Admin Login\n");
        printf("   4. Create Admin Account\n");
        printf("   0. Exit\n");
        printLine();
        printf("   Selection > ");
        choice = getIntInput("");
        switch (choice) {
        case 1:
            registerUser();
            pauseScreen();
            break;
        case 2:
            if (userLogin()) {
                userMenu();
            }
            pauseScreen();
            break;
        case 3:
            if (adminLogin()) {
                adminMenu();
            }
            pauseScreen();
            break;
        case 4:
            createAdminAccount();
            pauseScreen();
            break;
        case 0:
            printf("Thank you for using P.A.C.T. System!\n");
            printf("Shutting down...\n");
            exit(0);
        default:
            printf("[!] Invalid selection. Try again.\n");
            pauseScreen();
        }
    }
    return 0;
}
