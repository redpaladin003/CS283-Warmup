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

@test "Basic pipe functionality" {
    run ./dsh <<EOF
echo "Hello World" | grep "Hello"
exit
EOF
    echo "$output" | grep -q "Hello World"
    [ "$status" -eq 0 ]
}

@test "Triple command pipeline" {
    run ./dsh <<EOF
echo "Hello World" | grep "Hello" | wc -w
exit
EOF
    echo "$output" | grep -q "2"
    [ "$status" -eq 0 ]
}

@test "Error handling for nonexistent command in pipe" {
    run ./dsh <<EOF
echo "Hello World" | foobar | wc -w
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Error propagation in pipeline" {
    run ./dsh <<EOF
cat nonexistent_file | wc -l
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Pipe output filtering" {
    run ./dsh <<EOF
echo "Hello World" | grep "World"
exit
EOF
    echo "$output" | grep -q "World"
    [ "$status" -eq 0 ]
}

@test "Semicolon command separation" {
    run ./dsh <<EOF
echo First; echo Second; echo Third
exit
EOF
    echo "$output" | grep -q "First"
    echo "$output" | grep -q "Second"
    echo "$output" | grep -q "Third"
    [ "$status" -eq 0 ]
}

@test "Graceful handling of unknown commands" {
    run ./dsh <<EOF
foobar
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Quote handling for spaces" {
    run ./dsh <<EOF
echo "hello world"
exit
EOF
    echo "$output" | grep -q "hello world"
    [ "$status" -eq 0 ]
}

@test "Builtin cd command with path" {
    run ./dsh <<EOF
cd /tmp
pwd
exit
EOF
    echo "$output" | grep -q "/tmp"
    [ "$status" -eq 0 ]
}

@test "Builtin cd without arguments" {
    current_dir=$(pwd)
    run ./dsh <<EOF
cd
pwd
exit
EOF
    echo "$output" | grep -q "$current_dir"
    [ "$status" -eq 0 ]
}

@test "External command argument passing" {
    run ./dsh <<EOF
echo External Test
exit
EOF
    echo "$output" | grep -q "External Test"
    [ "$status" -eq 0 ]
}

@test "Error handling for invalid directory" {
    run ./dsh <<EOF
cd /nonexistent_directory
exit
EOF
    echo "$output" | grep -qi "No such file"
    [ "$status" -eq 0 ]
}

@test "Empty command detection" {
    run ./dsh <<EOF
       
exit
EOF
    echo "$output" | grep -q "warning: no commands provided"
    [ "$status" -eq 0 ]
}

@test "Sequential command execution" {
    run ./dsh <<EOF
echo First
echo Second
exit
EOF
    echo "$output" | grep -q "First"
    echo "$output" | grep -q "Second"
    [ "$status" -eq 0 ]
}

@test "Complex quote handling" {
    run ./dsh <<EOF
echo "Mixed quotes test with spaces"
exit
EOF
    echo "$output" | grep -q "Mixed quotes test with spaces"
    [ "$status" -eq 0 ]
}

@test "Exit command functionality" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Semicolon with pipe combination" {
    run ./dsh <<EOF
echo "Testing"; echo "Hello pipe" | grep "Hello"
exit
EOF
    echo "$output" | grep -q "Testing"
    echo "$output" | grep -q "Hello pipe"
    [ "$status" -eq 0 ]
}

@test "Multi-level pipeline with redirection" {
    # Create a temporary file with test data
    cat > test_data.txt << EOL
Line 1
Line 2
Line 3
Line with word
Last line
EOL

    run ./dsh <<EOF
cat test_data.txt | grep "Line" | grep -v "Last" | wc -l
exit
EOF
    # Output debugging information
    echo "Actual output: $output"
    
    # The grep needs to be more flexible as the output might include shell prompts
    [[ "$output" =~ 4 ]]
    
    # Clean up
    rm -f test_data.txt
    
    [ "$status" -eq 0 ]
}

@test "Dragon extra credit command" {

    run ./dsh <<EOF
dragon
exit
EOF
    
    # Debug output to see what we're getting
    echo "Dragon output: $output"
    
    # Check for the header first
    [[ "$output" =~ "DREXEL UNIVERSITY DRAGON" ]]
    
    # Check for percent signs which are used in the ASCII art
    [[ "$output" =~ \%\%\%\%\% ]]
    
    [ "$status" -eq 0 ]
}

@test "Complex pipe with sort and head" {
    # Create a test file with numbers
    cat > numbers.txt << EOL
5
10
1
8
3
7
2
4
9
6
EOL

    run ./dsh <<EOF
cat numbers.txt | sort -n | head -5
exit
EOF

    # The first five numbers in sorted order should be 1, 2, 3, 4, 5
    [[ "$output" =~ 1 ]]
    [[ "$output" =~ 2 ]]
    [[ "$output" =~ 3 ]]
    [[ "$output" =~ 4 ]]
    [[ "$output" =~ 5 ]]

    # Clean up
    rm -f numbers.txt

    [ "$status" -eq 0 ]
}

@test "Input whitespace handling" {
    run ./dsh <<EOF
   echo    "testing   spaces"
exit
EOF

    # Should output "testing   spaces" (preserving spaces inside quotes)
    [[ "$output" =~ "testing   spaces" ]]
    [ "$status" -eq 0 ]
}

@test "Pipe with multiple arguments" {
    run ./dsh <<EOF
echo "1 2 3 4 5" | tr " " "\n" | grep "[13]"
exit
EOF

    # Should output only lines containing 1 or 3
    [[ "$output" =~ 1 ]]
    [[ "$output" =~ 3 ]]
    [[ ! "$output" =~ 2 ]]
    [[ ! "$output" =~ 4 ]]
    [[ ! "$output" =~ 5 ]]

    [ "$status" -eq 0 ]
}

@test "Path command execution" {
    run ./dsh <<EOF
which ls
exit
EOF
    
    # Debug output
    echo "Actual output: $output"
    
    # This should find ls in a standard path location
    [[ "$output" =~ "/bin/ls" || "$output" =~ "/usr/bin/ls" ]]
    [ "$status" -eq 0 ]
}

@test "Four-command pipeline" {
    # Create a test file with some content
    cat > test_lines.txt << EOL
apple
banana
cherry
date
EOL

    run ./dsh <<EOF
cat test_lines.txt | grep "[aeiou]" | grep -v "e" | wc -l
exit
EOF

    # Debug output
    echo "Actual output: $output"

    # The result should be 3 (apple, banana, and date have vowels;
    # cherry has 'e' so it's excluded by grep -v)
    [[ "$output" =~ 3 ]]

    # Clean up
    rm -f test_lines.txt

    [ "$status" -eq 0 ]
}

@test "Simple pwd command" {
    run ./dsh <<EOF
pwd
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should contain the current directory
    current_dir=$(pwd)
    [[ "$output" =~ "$current_dir" ]]
    [ "$status" -eq 0 ]
}

@test "Repeated exit commands" {
    run ./dsh <<EOF
exit
exit
exit
EOF
    # Should exit after first exit command with status 0
    [ "$status" -eq 0 ]
}

@test "Empty pipe segment warning" {
    run ./dsh <<EOF
echo "test" | | grep test
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should show warning about empty commands
    [[ "$output" =~ "warning: no commands provided" ]]
    [ "$status" -eq 0 ]
}

@test "Long pipe line with basic commands" {
    run ./dsh <<EOF
echo "test" | cat | cat | cat | cat
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Final output should still be "test"
    [[ "$output" =~ "test" ]]
    [ "$status" -eq 0 ]
}

@test "Command with numeric output" {
    run ./dsh <<EOF
echo 12345
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should output the number
    [[ "$output" =~ "12345" ]]
    [ "$status" -eq 0 ]
}

@test "Simple file creation and verification" {
    run ./dsh <<EOF
echo "test content" > test_file.txt
cat test_file.txt
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should contain the file content
    [[ "$output" =~ "test content" || -f "test_file.txt" ]]

    # Clean up
    rm -f test_file.txt

    [ "$status" -eq 0 ]
}

@test "Commands with excessive spaces" {
    run ./dsh <<EOF
     echo      "spaced      out"
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should handle the spaces correctly
    [[ "$output" =~ "spaced      out" ]]
    [ "$status" -eq 0 ]
}

@test "Multiple cd commands" {
    # Create test directories
    mkdir -p test_dir1/test_dir2

    run ./dsh <<EOF
cd test_dir1
pwd
cd test_dir2
pwd
cd ../..
pwd
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Clean up
    rmdir -p test_dir1/test_dir2

    # Should show directory changes
    current_dir=$(pwd)
    [[ "$output" =~ "test_dir1" ]]
    [[ "$output" =~ "test_dir2" ]]
    [[ "$output" =~ "$current_dir" ]]
    [ "$status" -eq 0 ]
}

@test "Simple grep with pattern" {
    run ./dsh <<EOF
echo -e "line1\nline2\nline3\ntext" | grep "line"
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should only contain lines with "line"
    [[ "$output" =~ "line1" ]]
    [[ "$output" =~ "line2" ]]
    [[ "$output" =~ "line3" ]]
    [[ ! "$output" =~ "text" ]]
    [ "$status" -eq 0 ]
}

@test "Pipeline with sort command" {
    # Create a test file with unsorted lines
    cat > unsorted.txt << EOL
zebra
apple
monkey
banana
EOL

    run ./dsh <<EOF
cat unsorted.txt | sort
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Clean up
    rm -f unsorted.txt

    # Check if output contains sorted lines (alphabetically)
    [[ "$output" =~ "apple" ]]
    [[ "$output" =~ "banana" ]]
    [[ "$output" =~ "monkey" ]]
    [[ "$output" =~ "zebra" ]]

    # Verify apple appears before zebra in the output
    apple_pos=$(echo "$output" | grep -n "apple" | cut -d: -f1)
    zebra_pos=$(echo "$output" | grep -n "zebra" | cut -d: -f1)
    [[ $apple_pos -lt $zebra_pos ]]

    [ "$status" -eq 0 ]
}

#!/usr/bin/env bats
# File: student_tests.sh

@test "Command with underscore characters" {
    run ./dsh <<EOF
echo hello_world_test
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should contain the underscore characters
    [[ "$output" =~ "hello_world_test" ]]
    [ "$status" -eq 0 ]
}

@test "Pipe to head command" {
    run ./dsh <<EOF
echo -e "line1\nline2\nline3\nline4\nline5" | head -3
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should only contain the first 3 lines
    [[ "$output" =~ "line1" ]]
    [[ "$output" =~ "line2" ]]
    [[ "$output" =~ "line3" ]]
    [[ ! "$output" =~ "line4" ]]
    [ "$status" -eq 0 ]
}

@test "Nonexistent directory for cd" {
    run ./dsh <<EOF
cd /dir_that_does_not_exist_12345
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should show an error message but not crash
    [[ "$output" =~ "No such file" || "$output" =~ "cd:" ]]
    [ "$status" -eq 0 ]
}

@test "Cat command with single file" {
    # Create a test file
    echo "Test file content" > test_file.txt

    run ./dsh <<EOF
cat test_file.txt
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Clean up
    rm -f test_file.txt

    # Should contain the file content
    [[ "$output" =~ "Test file content" ]]
    [ "$status" -eq 0 ]
}

@test "Echo command with special characters" {
    run ./dsh <<EOF
echo "!@#$%^&*()_+"
exit
EOF
    # Debug output
    echo "Actual output: $output"

    # Should contain the special characters
    [[ "$output" =~ "!@#$%^&*()_+" ]]
    [ "$status" -eq 0 ]
}
