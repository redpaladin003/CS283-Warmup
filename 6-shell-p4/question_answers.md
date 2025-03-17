1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_My implementation ensures all child processes complete by calling waitpid() on each child's PID in a loop after creating the pipeline. Without waitpid(), child processes would become "zombies" that consume system resources and prevent proper error reporting. The shell would continue accepting new commands while previous commands might still be running, potentially causing output confusion and race conditions. Proper process cleanup with waitpid() is essential for collecting exit statuses and maintaining shell stability_

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_Closing unused pipe ends after dup2() is necessary to prevent resource leaks, as file descriptors are limited system resources. If pipe ends remain open, processes reading from pipes never receive EOF since the write end is still open somewhere. This can cause deadlocks where programs wait indefinitely for input that will never arrive. Additionally, data flow through the pipeline could be disrupted if pipes are improperly closed_

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_The cd command must be a built-in because each process has its own working directory, and an external cd would only change the directory in the child process, not the shell. If cd were external, directory changes would not persist between commands because the shell's own directory would remain unchanged. Users would experience confusing behavior where cd appears to work but has no lasting effect. Built-ins like cd and exit need direct access to the shell's own environment and process state_

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_To support arbitrary pipe chains, I would replace fixed arrays with dynamically allocated arrays that grow as needed during command parsing. This approach would require careful memory management with proper allocation error handling and resource cleanup even on error paths. The trade-offs include increased code complexity, potential for memory leaks, and slightly reduced performance compared to fixed arrays. There would still be practical limits due to system constraints on processes and file descriptors_
