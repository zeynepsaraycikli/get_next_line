*This project has been created as part of the 42 curriculum by zeynepsaraycikli.*

# Get Next Line

## Description
The `get_next_line` project is a C function designed to read a single line from a given file descriptor (fd). The goal is to provide a highly convenient tool for parsing and reading files or standard input, returning one complete line at a time, up to the newline character `\n` or the End of File (EOF). It also introduces the vital concept of **static variables** in C, which are necessary to preserve the remaining unread buffer between successive function calls.

## Instructions
Since this project produces a utility function rather than a standalone executable, it is meant to be compiled alongside your main project files.

To compile your code with the `get_next_line` function, use the following syntax (you can define the `BUFFER_SIZE` at compilation):
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 your_main.c get_next_line.c get_next_line_utils.c -o your_program
```

To compile the bonus part (which manages multiple file descriptors simultaneously):
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 your_main.c get_next_line_bonus.c get_next_line_utils_bonus.c -o your_program
```

If you prefer to compile it as a static library (`libgnl.a`) for easy linking in the future, you can use the provided Makefile:
- `make` : Compiles the mandatory part into `libgnl.a`
- `make bonus` : Compiles the bonus part into `libgnl.a`
- `make clean` : Removes object files
- `make fclean` : Removes object files and the static library
- `make re` : Recompiles the library from scratch

## Algorithm Explanation
The algorithm relies on maintaining a persistent state across function calls using a `static char *` variable (or an array of them in the bonus). The process follows a three-step logic:

1. **Read and Stash (`read_and_stash`)**: We read from the file descriptor in chunks defined by `BUFFER_SIZE`. After each read, the newly acquired buffer is joined (via `ft_strjoin`) with the persistent static variable (`stash`). This looping process halts immediately once a newline character `\n` is detected within the stash, or if `read` returns 0 (indicating EOF).
2. **Extract Line (`extract_line`)**: We iterate through our accumulated `stash` until we hit the first `\n` or the null terminator. We allocate the exact amount of memory needed to hold this segment (including the `\n` if it exists) and return it to the user.
3. **Update Stash (`update_stash`)**: We must remove the line we just extracted from the `stash` so that the next function call begins reading immediately after the newline. We isolate the trailing remainder of the string, move it into a newly allocated string, and completely `free()` the old stash to prevent memory leaks. If no characters remain after the newline, the stash is simply freed and set to `NULL`.

This logic was chosen because it correctly handles edge cases (like a file not ending in a newline), avoids utilizing excessively large memory blocks at once, and guarantees leak-free execution by rigorously cleaning up unused memory allocations.

## Resources
- [C File I/O and File Descriptors (Wikipedia)](https://en.wikipedia.org/wiki/File_descriptor)
- [Understanding Static Variables in C](https://www.geeksforgeeks.org/static-variables-in-c/)
- **AI Usage**: AI was used purely as a guided peer-learning tool to help construct this README file format and to cross-check the memory leak safety of the `ft_strjoin` and `update_stash` operations against the constraints of the 42 subject.

