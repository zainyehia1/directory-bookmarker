# Directory Bookmarker (bm)
A lightweight command-line tool for bookmarking directories, enabling instant navigation. You don't have to repeatedly type long paths anymore!

Bookmark your important directories once, and jump to them instantly from anywhere in your file system.

Built for developers and terminal users who frequently navigate through multiple directories and projects and need fast, reliable directory navigation without maintaining dozens of shell
aliases or relying on auto-learning tools.

## Features

***bm provides essential bookmark management with Unix-style simplicity.***

- **Quick navigation** - Quickly navigate to any bookmarked directory instantly with `bm go <name>` from anywhere in your file system
- **Add bookmarks** - Save directories with memorable names (supports tilde expansion like `~/Desktop`)
- **List bookmarks** - View all saved shortcuts with their full paths
- **Rename bookmarks** - Change bookmark names without losing the path
- **Edit bookmarks** - Edit the path of existing bookmarks
- **Delete bookmarks** - Remove bookmarks you no longer need
- **Path validation** - Automatically verifies if directories exist before saving
- **Persistent storage** - Bookmarks saved in `~/.bm/bookmarks.tsv`

## Installation

`bm` requires a UNIX-based system (Linux, macOS, WSL) and a C compiler.

#### Steps:
1. **Clone and Compile:**
   * Clone the repo: `git clone https://github.com/zainyehia1/directory-bookmarker.git`
   * `cd directory-bookmarker`
   * Run: `make`


2. **Install:** 
   * Run: `make install`
       * This creates a `bin` folder in your `Home` directory which contains the `bm` binary.
     

3. **Shell Function:**
    * This shell function is required for `bm go` to change your current directory.
    * Add this to your `.bashrc` (or `.zshrc` for zsh):
   ```bash 
   bm() {
   # Check first argument
   if [ "$1" = "go" ]; then
   # It's a 'go' command - special handling
   local path=$(command bm go "$2")  # Run C program, capture output
   [ -n "$path" ] && cd "$path"       # If path exists, cd to it
   else
   # Not 'go' - just pass to C program
   command bm "$@"  # Pass all arguments
   fi
   }
   ```
   * Reload your shell: `source ~/.bashrc`


4. **Verify Installation:**
    * Check that bm is accessible: `which bm`
    * View available commands: `bm help`
    * Initialize the system: `bm init`
    * Test it: `bm add test ~/Desktop` then `bm go test`
      * Delete the bookmark if you want: `bm delete test`
      
## Usage

**Initialize the bookmark system:**

```bash
$ bm init
```

```text
Bookmark system initialized!
```

**Add bookmarks:**
```bash
$ bm add desk ~/Desktop      # Using tilde expansion
```

```text
Bookmark added successfully!
```

```bash
$ bm add work /home/user/Documents/Work   # Using absolute path
```

```text
Bookmark added successfully!
```

**List bookmarks:**
```bash
$ bm list
```

```text
+-----------------+---------------------------+
|  Bookmark Name  | Directory Path            |
+-----------------+---------------------------+
| desk            | /home/user/Desktop        |  # Tilde resolved to absolute path
| work            | /home/user/Downloads/Work |
+-----------------+---------------------------+
```

**Rename bookmarks:**
```bash
$ bm rename desk desktop
```

```text
Bookmark 'desk' has been renamed successfully!
'desktop' --> /home/user/Desktop
```

**Edit bookmarks:**
```bash
$ bm edit work ~/Documents/MyCompany/Work
```

```text
Bookmark 'work' has been edited successfully!
'work' --> /home/user/Documents/MyCompany/Work 
```

**Delete bookmarks:**
```bash
$ bm delete desktop
```

```text
Bookmark 'desktop' deleted successfully!
```

**Navigate to a bookmarked directory:**
```bash
$ bm go work
```

```bash
$ pwd
```

```text
/home/user/Documents/MyCompany/Work
```

**Check usage and valid commands:**
```bash
$ bm help
```

```text
Usage: bm <command> [<args>]
Commands:
  init                                  Initialize bookmark system
  add <name> <path>                     Add a bookmark
  delete <name>                         Delete a bookmark
  list                                  List all bookmarks
  rename <old_name> <new_name>          Rename a bookmark
  edit <name> <new_path>                Edit a bookmark's path
  go <name>                             Print path of a bookmark
  help                                  Print this message
```

## How It Works

### From Command Line to Action:
* When a user runs `bm`, the program first validates the command-line input and then dispatches execution to the appropriate operation.
  * The program uses `argc` and `argv` to determine how many arguments were provided and to identify the requested command.
  * The command is determined by comparing the second argument with the supported commands using `strcmp()`
* If the user enters an invalid command:
  * The program prints an error message and displays the help output.
* The program checks whether the user provides the proper amount of command-line arguments before execution. This ensures consistent storage format in the bookmark file with valid entries.


### Data Structures & Internal Design:
* Bookmarks are represented in memory using a singly linked list rather than a fixed-size array.
* This design does not limit the user to a fixed amount of bookmarks. They can add as many bookmarks as they want.
* Each node in the linked list consists of a `Bookmark` struct which contains a name and a path, and a pointer to the next node.
* If a command requires modification of the bookmark file (add, rename, edit, delete), then the entire file is loaded into a linked list in memory.
* Changes are applied to the linked list and then written back to the file.


### Persistent Storage & File Format:
* Bookmarks are stored persistently in a tab-separated values (TSV) file.
* TSVs are simple and easily readable, hence the choice to use them.
* The file is stored in `~/.bm/bookmarks.tsv` which is hidden by default and allows for the bookmarks to be accessed from any directory.
* The program ensures that only validated input is written to the file.
* **Note:** If the file is manually edited, there is currently no way to properly validate input.

### Path Handling & Validation:
* Paths are validated before being stored to ensure that navigation via `bm go` always succeeds.
* Tilde expansion (`~`) is supported to reduce typing and improve usability.
* Paths are resolved to absolute paths using `realpath()` before being saved.
* Invalid or non-existent paths are rejected before any changes are written to the file.
   

### Shell Integration for `bm go`:
* Child processes cannot modify the parent shell's working directory; therefore, shell-level integration is needed to change directories.
* To support this, `bm go` prints the resolved directory path to standard output instead of calling `cd` directly.
* A shell function wraps the `bm` binary and captures the output of `bm go`, and calling `cd` "under the hood" when appropriate.
* If there is an error, error messages are printed to standard error (`stderr`).


**Since memory is dynamically allocated and freed during each operation using `malloc()` and `free()`, `Valgrind` was used to ensure zero memory leaks in the program.**
   
## Known Limitations

- Paths longer than 512 characters are rejected.
  * I chose this because this wouldn't be an issue for most users and it saves some memory.
  * An alternative would be `PATH_MAX` (4096) from `<linux/limits.h>`
- Manual edits to the bookmark file are not validated.
- Incompatibility with Windows.

## Future Improvements

1. **Future versions may introduce a database-backed storage layer using SQLite.**
   * This would help protect against manual file edits.
   * This would simplify sorting and searching/querying.
   * Timestamps and access counts could be added to enable recent-usage sorting which can come in handy.

2. **Optional readability enhancing colorized output may be added.**

## License
This project is licensed under the MIT License.
