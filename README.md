# Sudoku using multithreading

**Introduction:**
A Sudoku puzzle uses a 9×9 grid in which each row, column, and 3×3 subgrid must contain all the digits 1...9.
This project consists of designing a multithreaded application that determines whether the solution to a Sudoku puzzle is valid.

### Part 1 of 2: Sudoku Solution Validator
Threads are created to check the following criteria:
* One thread - check if each column contains the digits 1 through 9
* One thread - check if each row contains the digits 1 through 9
* One thread - check if each of the 3×3 subgrids contains the digits 1 through 9

**Passing Parameters to Each Thread**

The parent thread creates the worker threads, passing each worker the location that it must check in the grid. This step requires passing several parameters to each thread, which is done using a struct.
> For example, a structure to pass the row and column where a thread must begin validating:
> typedef struct {
> int row;
> int column;
> };

**Returning Results to the Parent Thread**

Each worker thread determines the validity of a particular region of the Sudoku puzzle. Once a worker has performed this check, it passes its results back to the parent. The ith index in this array corresponds to the ith worker thread. If a worker sets its corresponding value to 1, it is indicating that its region of the Sudoku puzzle is valid. A value of 0 would indicate otherwise.
When all worker threads have completed, the parent thread checks each entry in the result array to determine if the Sudoku puzzle is valid.
* When a thread returns values to the main thread and it is writing on the array, no other thread can read array value.
* This requires synchronization and mutex lock.
* Invalid entries and their indexes are stored.

### Part 2 of 2: Sudoku Puzzle Solution
A Sudoku matrix with at least 2 invalid entries and its solution are initialized. Each thread is assigned to each row and column and corresponding values are changed if found erroneous (same as followed in part 1). 
Limited threads are run using semaphores to balance overhead and security issues.
After resolving invalid entries, the solution is validated and the result is displayed.
