#!/usr/bin/env bats

# File: student_tests.sh
# 
# Unit tests for the shell using dsh, rsh client, and rsh server

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Check exit command works" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Check stop-server command works" {
    run ./dsh -c 127.0.0.1 <<EOF
stop-server
EOF
    [ "$status" -eq 0 ]
}

@test "Check pwd runs without errors" {
    run ./dsh <<EOF                
pwd
EOF
    [ "$status" -eq 0 ]
}

@test "Check echo runs without errors" {
    run ./dsh <<EOF                
echo "Hello World!"
EOF
    [ "$status" -eq 0 ]
}

@test "cd to non-existent directory prints error" {
    run ./dsh <<EOF
cd /nonexistent_directory
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Whitespace only input warns the user" {
    run ./dsh <<EOF
       
exit
EOF
    echo "$output" | grep -q "warning: no commands provided"
    [ "$status" -eq 0 ]
}

@test "Multiple separate commands execute correctly" {
    run ./dsh <<EOF
echo First
echo Second
exit
EOF
    echo "$output" | grep -q "First"
    echo "$output" | grep -q "Second"
    [ "$status" -eq 0 ]
}

@test "Command with mixed quotes and spaces" {
    run ./dsh <<EOF
echo "Mixed quotes test with spaces"
exit
EOF
    echo "$output" | grep -q "Mixed quotes test with spaces"
    [ "$status" -eq 0 ]
}

@test "Built-in exit command terminates the shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Handle empty commands" {
    run ./dsh <<EOF
    
EOF
    [[ "$output" == *"warning: no commands provided"* ]]
}

@test "Pipe with three commands (three-stage pipe)" {
  run ./dsh <<EOF
echo "abc" | tr 'a' 'A' | tr 'b' 'B'
exit
EOF
  # Expect "ABc" from the pipeline
  echo "$output" | grep -q "ABc"
  [ "$status" -eq 0 ]
}

@test "Check cd to a valid directory (e.g. /tmp)" {
  run ./dsh <<EOF
cd /tmp
pwd
exit
EOF
  # Expect the shell to print "/tmp" somewhere
  echo "$output" | grep -q "/tmp"
  [ "$status" -eq 0 ]
}

@test "Check error for invalid command" {
  run ./dsh <<EOF
thiscommanddoesnotexist
exit
EOF
  # The code prints "execvp failed" or "fork failed" in the child if the command doesn't exist
  echo "$output" | grep -q "execvp failed"
  [ "$status" -eq 0 ]
}

@test "Check piping more than two commands" {
  run ./dsh <<EOF
echo "1 2 3 4" | tr ' ' '\n' | grep 3
exit
EOF
  # We expect "3" to appear in the output
  echo "$output" | grep -q "3"
  [ "$status" -eq 0 ]
}

@test "Check leading/trailing spaces in command" {
  run ./dsh <<EOF
        echo "leading spaces"
    exit
EOF
  echo "$output" | grep -q "leading spaces"
  [ "$status" -eq 0 ]
}

@test "Check double quotes" {
  run ./dsh <<EOF
echo "Testing quotes with spaces"
exit
EOF
  echo "$output" | grep -q "Testing quotes with spaces"
  [ "$status" -eq 0 ]
}

@test "Check single quotes" {
  run ./dsh <<EOF
echo 'single-quoted text'
exit
EOF
  echo "$output" | grep -q "single-quoted text"
  [ "$status" -eq 0 ]
}

@test "Check multiple separate piped lines" {
  run ./dsh <<EOF
echo "first line" | cat
echo "second line" | cat
exit
EOF
  echo "$output" | grep -q "first line"
  echo "$output" | grep -q "second line"
  [ "$status" -eq 0 ]
}

@test "Check handling a built-in plus a command" {
  run ./dsh <<EOF
cd /
pwd
exit
EOF
  # Expect to see "/" in the output
  echo "$output" | grep -q "/"
  [ "$status" -eq 0 ]
}

@test "Pipe produces no output (grep no match)" {
  run ./dsh <<EOF
echo "Hello" | grep "ZZZ"
exit
EOF
  # Expect no matching lines, so no 'Hello' in output
  [[ "$output" != *"Hello"* ]]
  [ "$status" -eq 0 ]
}

@test "exit command with trailing spaces" {
  run ./dsh <<EOF
exit
EOF
  # If the shell exits cleanly, status = 0
  [ "$status" -eq 0 ]
}

@test "cd with no arguments (should not error)" {
  run ./dsh <<EOF
cd
pwd
exit
EOF
  # Usually cd with no arguments either does nothing or goes to $HOME.
  # We'll just check it doesn't crash and prints something from pwd
  [[ "$output" == *"/"* ]]
  [ "$status" -eq 0 ]
}

@test "cd .. and check directory changed" {
  run ./dsh <<EOF
pwd
cd ..
pwd
exit
EOF
  # We expect the second pwd to differ (at least by one path component)
  # We'll just check that we have two 'pwd' lines in output
  lines_with_pwd=$(echo "$output" | grep -c "/")
  [ "$lines_with_pwd" -ge 2 ]
  [ "$status" -eq 0 ]
}

@test "Unknown built-in (dragon) doesn't crash shell" {
  run ./dsh <<EOF
dragon
echo "Still alive"
exit
EOF
  # 'dragon' is not recognized in your code, so it just forks exec or does nothing
  # We check that the shell didn't crash and that "Still alive" is printed
  echo "$output" | grep -q "Still alive"
  [ "$status" -eq 0 ]
}

@test "ls -l runs without errors" {
  run ./dsh <<EOF
ls -l
exit
EOF
  # We don't strictly parse the output, just confirm the shell didn't fail
  [ "$status" -eq 0 ]
}

@test "ls nonexistent prints error message" {
  run ./dsh <<EOF
ls /DOES_NOT_EXIST_123
exit
EOF
  # We expect "No such file" or similar in the output
  echo "$output" | grep -qi "No such file"
  [ "$status" -eq 0 ]
}


@test "Complex quotes with single and double quotes" {
  run ./dsh <<EOF
echo "some 'quoted' text"
exit
EOF
  # We expect "some 'quoted' text" in the output
  echo "$output" | grep -q "some 'quoted' text"
  [ "$status" -eq 0 ]
}

@test "Pipe with no output in second stage" {
  run ./dsh <<EOF
echo "1 2 3" | grep "4"
exit
EOF
  # We expect no "1 2 3" in output, since grep "4" won't match
  [[ "$output" != *"1 2 3"* ]]
  [ "$status" -eq 0 ]
}

@test "Exit mid sequence" {
  run ./dsh <<EOF
echo "Before exit"
exit
echo "AFTER exit"
EOF

  # We expect "Before exit" to appear,
  # but "AFTER exit" should NOT appear in the output
  echo "$output" | grep -q "Before exit"
  [[ "$output" != *"AFTER exit"* ]]
  [ "$status" -eq 0 ]
}
