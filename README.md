# P.A.C.T

## Project Overview

P.A.C.T. (Program & Accountability Console Tracker) is a command-line based task and accountability management system developed in the C programming language. The goal of this project is to simulate a real-world productivity platform where users can organize their tasks, participate in structured programs, and maintain accountability through a controlled environment.

The system is designed with two distinct roles: **users** and **administrators**. General users can create personal programs, manage their own tasks, and subscribe to programs created by administrators. On the other hand, administrators have more control over the system, including the ability to create structured programs, assign tasks to users, and enforce accountability through penalty mechanisms.

This project reflects the practical application of fundamental programming concepts such as structures, file handling, modular design, and input validation. It also demonstrates how a relatively low-level language like C can be used to build a feature-rich and logically complex system.

---

## Features

###  User Features

Users can register and log into the system securely using a username and password. Once logged in, they gain access to a personalized dashboard that allows them to manage their activities efficiently.

Users can create **personal programs**, which act as containers for organizing their own tasks. These programs are private and only accessible by the creator. Additionally, users can browse available public programs created by administrators and subscribe to them using a join code.

Within both personal and subscribed programs, users can:

* Create tasks with deadlines
* View all assigned tasks
* Mark tasks as completed
* Track pending and completed work

Users also have the option to unsubscribe from programs if they no longer wish to participate.

---

### Admin Features

Administrators play a key role in maintaining structured accountability. They can create up to three public programs, each with a unique join code that users must provide to subscribe.

Admins can assign tasks to specific users within their programs. A key feature of the system is the **penalty task mechanism**, where admins can assign additional tasks to users who fail to meet expectations. This introduces a layer of accountability that goes beyond simple task tracking.

Admins can also:

* View all subscribers of a program
* Monitor task completion status across users
* See summary statistics such as completion rates
* Delete programs they have created

This dual-role system makes the application more dynamic and closer to real-world platforms.

---

## File Structure and Data Storage

The system uses plain text files to store data persistently. Each file serves a specific purpose:

* `users.txt` stores user credentials and profile information
* `admins.txt` stores administrator credentials
* `programs.txt` contains all program data, including both personal and public programs
* `tasks.txt` stores all tasks along with their status and ownership
* `subscriptions.txt` keeps track of which users are subscribed to which programs
* `admin_programs.txt` links administrators to the programs they have created

All data is stored using a structured format with fields separated by the pipe (`|`) character. This approach allows efficient parsing using functions like `fgets` and `sscanf`.

---

## Program Flow

The application begins with a main menu that allows users to either register, log in, or access the admin system. Based on the selected option, the user is redirected to either the User Dashboard or the Admin Dashboard.

Each dashboard presents a menu-driven interface, enabling users to perform various operations by entering numeric choices. The system continuously runs in a loop until the user chooses to exit.

All operations such as creating programs, assigning tasks, or updating task status are reflected in the respective files, ensuring that data persists between runs.

---

## Design Choices

One of the primary design decisions was to use **file-based storage instead of a database**. This choice keeps the project lightweight and aligns with the scope of CS50, while still demonstrating persistence.

Another important design aspect is the **separation of roles** between users and administrators. This introduces complexity in logic and access control, making the system more realistic.

The project is also designed using a **modular approach**, where each functionality is implemented in separate functions. This improves readability, maintainability, and scalability.

---

## Limitations

Despite its functionality, the system has several limitations:

* Passwords are stored in **plain text**, which is not secure for real-world applications
* The interface is entirely command-line based, with no graphical elements
* Error handling for corrupted or malformed files is limited
* The system uses fixed-size arrays, which restrict scalability
* There is no support for concurrent users or real-time updates

---

## Future Improvements

There are several ways this project can be improved in the future:

* Implement password hashing for secure authentication
* Replace text file storage with a proper database system
* Develop a graphical user interface (GUI)
* Add automated deadline tracking and penalty enforcement
* Improve input validation and error handling
* Introduce search, sorting, and filtering features

---

## Compilation and Execution

To compile and run the program, use a C compiler such as `gcc`:

```bash
gcc pact.c -o pact
./pact
```

Ensure that all required text files are present in the same directory as the program.

---

## Learning Outcomes

This project helped me strengthen my understanding of:

* File handling in C
* Structs and data organization
* Building multi-feature command-line applications
* Designing user-role based systems
* Writing modular and maintainable code

---

## Conclusion

P.A.C.T. is a comprehensive command-line application that demonstrates how multiple programming concepts can be integrated into a single cohesive system. It successfully models a real-world accountability platform using the C programming language.

While there are areas for improvement, the project fulfills its goal of creating a functional, structured, and interactive system. It serves as a strong example of applying foundational programming knowledge to solve practical problems.
