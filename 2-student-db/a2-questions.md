## Assignment 2 Questions

#### Directions
Please answer the following questions and submit in your repo for the second assignment.  
Please keep the answers as short and concise as possible.

---

### 1. Externalizing `get_student(...)`
> **Answer:** Yes, externalizing `get_student(...)` improves modularity and code reusability. It centralizes student record retrieval logic, reducing redundant code in functions like `del_student`, `add_student`, and `print_student`, improving maintainability.

---

### 2. Why is returning a local variable bad in C?
> **Answer:** Returning a pointer to a local variable (`return &student;`) is a **bad idea** because the variable is allocated on the stack and gets deallocated when the function exits. This results in **undefined behavior**, as the pointer would reference an invalid memory location.

---

### 3. Using `malloc()` for `get_student(...)`
> **Answer:** This method works but has **two major issues**:
> 1. **Memory Leak Risk** – If the caller forgets to `free()` the allocated memory, it results in memory leaks.
> 2. **Overhead** – Allocating memory dynamically for each query adds unnecessary overhead compared to passing a reference.

---

### 4. Sparse Files and File System Blocks

#### **4a. Why does `ls` show increasing file sizes?**
> **Answer:** The file size increases because each student record is stored at an offset of `id * 64`. When adding **ID=1**, space is allocated for `2 * 64 = 128 bytes`, for **ID=3**, `4 * 64 = 256 bytes`, and for **ID=64**, `65 * 64 = 4160 bytes`. The file grows **to accommodate the highest student ID stored**.

#### **4b. Why did `du` report no change until ID=64?**
> **Answer:** Linux uses **sparse files**, meaning storage is only allocated when data is written. Before ID=64, all records fit within **one 4K disk block**. When ID=64 is added, it exceeds the 4K block limit, triggering allocation of another **4K block**, increasing disk usage from **4K to 8K**.

#### **4c. Why does adding ID=99999 increase file size but not disk usage?**
> **Answer:** Since student ID=99999 is stored at an offset of `99999 * 64 = 6.4MB`, `ls` reports a **logical file size** of 6.4MB. However, Linux does **not allocate physical storage** for empty gaps in sparse files, so `du` only reports **12K of actual storage**.

---

