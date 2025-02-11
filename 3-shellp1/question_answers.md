1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is ideal because it safely handles buffer sizes to prevent overflow and can detect EOF (end-of-file) conditions, which is crucial for both interactive use and automated testing through shell scripts. It also preserves whitespace and newlines, giving us full control over input parsing.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() instead of a fixed array allows us to allocate exactly the amount of memory needed based on the defined constants (SH_CMD_MAX) and ensures memory is allocated on the heap rather than the stack, where large arrays could cause stack overflow in resource-constrained environments.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces is essential because trailing/leading spaces can cause command execution failures since the shell would try to execute commands with spaces in their names or pass arguments with unintended spaces. For example, "  ls  -l  " would be treated differently from "ls -l" by the system, potentially causing the command to fail or behave unexpectedly.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Three key redirection examples we should implement are: > for output redirection to a file, < for input redirection from a file, and >> for appending output to a file. The main challenge would be managing file descriptors and handling file permissions/errors correctly.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection connects a command with files for input/output, while piping connects the output of one command directly to the input of another command in memory. Pipes create a communication channel between processes, while redirection works with files.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: Keeping STDERR separate from STDOUT allows users to handle errors differently from normal output, enabling proper error logging and debugging while maintaining clean program output.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our shell should provide both separate and merged output options through standard redirection operators (2> for STDERR, > for STDOUT, and 2>&1 for merging) to maintain compatibility with typical shell behavior and enable flexible error handling.
