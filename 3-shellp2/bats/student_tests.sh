#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Changing directory to a valid path" {
    run ./dsh <<EOF
cd /tmp
pwd
exit
EOF
    echo "$output" | grep -q "/tmp"
    [ "$status" -eq 0 ]
}

@test "Changing directory with no parameters stays in same path" {
    start_path=$(pwd)
    run ./dsh <<EOF
cd
pwd
exit
EOF
    echo "$output" | grep -q "$start_path"
    [ "$status" -eq 0 ]
}

@test "Quoted text preserves spacing" {
    run ./dsh <<EOF
echo "hello world"
exit
EOF
    echo "$output" | grep -q "hello world"
    [ "$status" -eq 0 ]
}

@test "Unknown command prints error" {
    run ./dsh <<EOF
foobar
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Run an external command with arguments" {
    run ./dsh <<EOF
echo External Test
exit
EOF
    echo "$output" | grep -q "External Test"
    [ "$status" -eq 0 ]
}

@test "Invalid directory triggers error" {
    run ./dsh <<EOF
cd /nonexistent_directory
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Blank input warns the user" {
    run ./dsh <<EOF

exit
EOF
    echo "$output" | grep -q "warning: no commands provided"
    [ "$status" -eq 0 ]
}

@test "Sequential commands execute in order" {
    run ./dsh <<EOF
echo First
echo Second
exit
EOF
    echo "$output" | grep -q "First"
    echo "$output" | grep -q "Second"
    [ "$status" -eq 0 ]
}

@test "Mixed quotes with spaces" {
    run ./dsh <<EOF
echo "Mixed quotes test with spaces"
exit
EOF
    echo "$output" | grep -q "Mixed quotes test with spaces"
    [ "$status" -eq 0 ]

}

@test "Single argument echo command" {
    run ./dsh <<EOF
echo Hello
exit
EOF
    echo "$output" | grep -q "Hello"
    [ "$status" -eq 0 ]
}

@test "Multiple arguments echo command" {
    run ./dsh <<EOF
echo Hello World Testing
exit
EOF
    echo "$output" | grep -q "Hello World Testing"
    [ "$status" -eq 0 ]
}

@test "Exit with extra arguments still terminates shell" {
    run ./dsh <<EOF
exit now
EOF
    [ "$status" -eq 0 ]
}

@test "CD with multiple arguments uses only the first path" {
    run ./dsh <<EOF
cd /tmp /etc
pwd
exit
EOF
    echo "$output" | grep -q "/tmp"
    [ "$status" -eq 0 ]
}

@test "LS non-existent file shows error" {
    run ./dsh <<EOF
ls does_not_exist_xyz
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "PWD shows current directory" {
    mydir=$(pwd)
    run ./dsh <<EOF
pwd
exit
EOF
    echo "$output" | grep -q "$mydir"
    [ "$status" -eq 0 ]
}

@test "CD to current directory (.) does not change path" {
    original=$(pwd)
    run ./dsh <<EOF
cd .
pwd
exit
EOF
    echo "$output" | grep -q "$original"
    [ "$status" -eq 0 ]
}

@test "Extra spaces before command still works" {
    run ./dsh <<EOF
       echo spaced
exit
EOF
    echo "$output" | grep -q "spaced"
    [ "$status" -eq 0 ]
}

@test "Empty quotes become empty argument" {
    run ./dsh <<EOF
echo "Testing" ""
exit
EOF
    echo "$output" | grep -q "Testing"
    # Should still include the empty argument in the command
    [ "$status" -eq 0 ]
}

@test "Which on 'ls' prints a path to 'ls' (if installed)" {
    run ./dsh <<EOF
which ls
exit
EOF
    echo "$output" | grep -q "/"
    [ "$status" -eq 0 ]
}

@test "Run 'ls -l' to verify multi-argument external command" {
    run ./dsh <<EOF
ls -l
exit
EOF
    echo "$output" | grep -Eq "^-|^d"  # Looks for file/directory listings
    [ "$status" -eq 0 ]
}

@test "Run echo with multiple quoted arguments" {
    run ./dsh <<EOF
echo "foo" "bar" "baz"
exit
EOF
    echo "$output" | grep -q "foo bar baz"
    [ "$status" -eq 0 ]
}

@test "Run echo with leading/trailing spaces around quotes" {
    run ./dsh <<EOF
echo   "  spaced  "
exit
EOF
    echo "$output" | grep -q "  spaced  "
    [ "$status" -eq 0 ]
}


@test "cd fails on empty string argument" {
    run ./dsh <<EOF
cd ""
pwd
exit
EOF
    # We expect it to remain in original directory, so let's just check no chdir error
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Use echo with multiple spaces between arguments" {
    run ./dsh <<EOF
echo    arg1       arg2
exit
EOF
    echo "$output" | grep -q "arg1 arg2"
    [ "$status" -eq 0 ]
}

@test "Try an uppercase command that likely doesn't exist" {
    run ./dsh <<EOF
FAKECMD
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Check if a single-word command with trailing spaces runs" {
    run ./dsh <<EOF
ls
exit
EOF
    echo "$output" | grep -Eq "^-|^d"
    [ "$status" -eq 0 ]
}

@test "Quoted argument containing multiple spaces in the middle" {
    run ./dsh <<EOF
echo "multiple    spaces"
exit
EOF
    echo "$output" | grep -q "multiple    spaces"
    [ "$status" -eq 0 ]
}

@test "Exit with extra arguments still exits successfully" {
    run ./dsh <<EOF
exit now
EOF
    [ "$status" -eq 0 ]
}

@test "Repeated blank lines are ignored" {
    run ./dsh <<EOF


echo BlankTest
   

echo Done
exit
EOF
    echo "$output" | grep -q "BlankTest"
    echo "$output" | grep -q "Done"
    [ "$status" -eq 0 ]
}

@test "CD to /var directory and confirm path" {
    run ./dsh <<EOF
cd /var
pwd
exit
EOF
    echo "$output" | grep -q "/var"
    [ "$status" -eq 0 ]
}

@test "Quoted argument with multiple quote pairs" {
    run ./dsh <<EOF
echo "This" "is" "a test" "with quotes"
exit
EOF
    echo "$output" | grep -q "This is a test with quotes"
    [ "$status" -eq 0 ]
}
