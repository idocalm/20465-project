# THIS FILE WAS MADE SOLELY TO TEST THE ASSEMBLY ON THE TESTS DIRECTORY ALL AT ONCE
# IT IS ONLY AN HELPER SCRIPT AND IS NOT NEEDED TO RUN THE ASSEMBLY ! 


# Run build script
./build.sh

# Define the base path where directories are located
BASE_PATH="tests"

# Check if a path is provided
if [ -z "$BASE_PATH" ]; then
    echo "Please provide a path to the directories."
    exit 1
fi

# Iterate over each directory in the given path
for dir in "$BASE_PATH"/*; do
    if [ -d "$dir" ]; then
        dir_name=$(basename "$dir")

        if [ -f "$dir/test.as" ]; then
            ./asm "$dir/test" > "$dir/errors.txt"

            if diff "$dir/errors.txt" "$dir/expected.txt" > /dev/null; then
                echo "PASS: $dir/errors.txt matches expected.txt"
            else
                echo "FAIL: $dir/errors.txt does not match expected.txt"
            fi

            if [ -f "$dir/test.ob" ]; then
                if diff "$dir/test.ob" "$dir/ob_expected.ob" > /dev/null; then
                    echo "PASS: $dir/test.ob matches ob_expected.ob"
                else
                    echo "FAIL: $dir/test.ob does not match ob_expected.ob"
                fi
            fi

            if [ -f "$dir/test.ent" ]; then
                if diff "$dir/test.ent" "$dir/ent_expected.ent" > /dev/null; then
                    echo "PASS: $dir/test.ent matches ent_expected.ent"
                else
                    echo "FAIL: $dir/test.ent does not match ent_expected.ent"
                fi
            fi

            if [ -f "$dir/test.ext" ]; then
                if diff "$dir/test.ext" "$dir/ext_expected.ext" > /dev/null; then
                    echo "PASS: $dir/test.ext matches ext_expected.ext"
                else
                    echo "FAIL: $dir/test.ext does not match ext_expected.ext"
                fi
            fi

        else
            echo "No test.as file found in $dir"
        fi
    fi
done

