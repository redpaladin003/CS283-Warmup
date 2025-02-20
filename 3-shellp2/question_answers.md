1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  we use `fork/execvp` because it provides a safe way to spawn a new process to execute the command while keeping the original shell active. This separation allows the parent to manage multiple commands sequentially and to capture the child's exit status afterward

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, the implementation will detect the error, report the failure to the user, and handle the situation without crashing the shell

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: execvp() finds the command by iterating over the directories specified in the PATH environment variable, looking for an executable file that matches the given command name

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: Calling wait() makes the parent process waits for the child to finish, collects its exit status, and prevents zombie processes. Without it, the parent might continue executing and leave terminated child processes uncollected

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() retrieves the exit code from the child’s termination status, and offers feedback on the success or failure of the executed command for proper error handling and control flow

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: The function groups characters between quotes into a single token, it make sure that arguments containing spaces are treated as one unit for correctly interpreting commands where whitespace is part of an argument

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: The updated parsing logic improves upon previous implementations by managing quoted arguments and tokenizing input. Unexpected challenges included handling edge cases ensuring efficient memory management during parsing

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: Signals allow processes to receive asynchronous notifications of events, differing from other IPC methods by being lightweight and limited in data transmission

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL, SIGTERM, and SIGINT are used signals. SIGKILL forcefully terminates a process, SIGTERM requests a graceful shutdown, and SIGINT (typically triggered by Ctrl+C) interrupts a process, allowing it to perform cleanup if programmed to handle the signal

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is suspended immediately. Unlike signals such as SIGINT, SIGSTOP cannot be caught or ignored, ensuring that the process can always be halted regardless of its state
