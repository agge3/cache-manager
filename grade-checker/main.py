import os
import subprocess
import shutil
import sys
import re
from sympy import primerange
from dateutil import parser


def _is_windows():
    """ Check if the OS is Windows. """
    return os.name == "nt"


# Function to check if files exist
def _check_files(files):
    """ Check if all files exist. """
    return all(_check_file(file) for file in files)


def _check_file(file):
    """ Check if a single file exists. """
    # Use `isfile()` vs. `exists()` because `exists()` can check for 
    # directories.
    if not os.path.isfile(file):
        print(f"Warning: File '{file}' does not exist.")
        return False
    return True


def _split_clazz_name(clazz):
    """ Split class name into words based on uppercase letters. """
    return re.findall(r'[A-Z][a-z]*', clazz)


class Shell:
    def __init__(self):
        self._os = os.name
        self._bash = self._get_bash_path()
        self._check_dep()


    def _get_bash_path(self):
        """ Find the appropriate bash executable depending on the OS. """
        if _is_windows():
            return self._get_git_bash_path()
        return "/usr/bin/env bash"


    def _get_git_bash_path(self):
        """ Find Git Bash executable on Windows. """
        possible_paths = [
            r"C:\Program Files\Git\bin\bash.exe",
            r"C:\Program Files (x86)\Git\bin\bash.exe",
        ]
        for path in possible_paths:
            if os.path.isfile(path):
                return path
        raise FileNotFoundError("Git Bash not found. Please install Git Bash.")


    def _check_dep(self):
        """
        Ensure required dependencies like `grep` are available.
        On Windows, Git Bash includes grep; ensure it works.
        """
        try:
            res = subprocess.run(
                "grep --version", shell=True, capture_output=True, text=True
            )
            if res.returncode != 0:
                raise RuntimeError(f"grep check failed with error: {res.stderr}")
            print("grep is available.")
        except FileNotFoundError:
            print("Error: `grep` command not found.")
            sys.exit(1)


    def cmd(self, cmd):
        """
        Run a shell command.
        On Windows, use Git Bash if specified; otherwise, run normally.
        """
        if _is_windows():
            # Wrap the command for Git Bash compatibility.
            # xxx why do we need to join here?
            bash_cmd = ' '.join(cmd)
            result = subprocess.run([self._bash, "-c", bash_cmd], capture_output=True, text=True)
        else:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True)

        return result.stdout, result.stderr, result.returncode


class Build:
    def __init__(self):
        if _is_windows():
            try:
                self.exec_name = self._find_msbuild()
            except FileNotFoundError as e:
                print(e)
                sys.exit(1)

            self.config = self._find_sln_path()
            if not self.config:
                print("No .sln file found in the current directory.")
                sys.exit(1)

            self.cmd = f'"{self.exec_name}" "{self.config}" /p:Configuration=Debug'
        else:
            self.exec_name = "/usr/bin/env cmake"
            self.config = "CMakeLists.txt"
            self.cmd = "mkdir build && cd build && cmake .. && cmake --build ."


    def _find_msbuild(self):
        """ Find msbuild.exe on Windows. """
        search_dirs = [
            r"C:\Program Files (x86)",
            r"C:\Program Files",
        ]
        exec_name = "msbuild.exe"
    
        # Search through the directories
        for root_dir in search_dirs:
            for root, dirs, files in os.walk(root_dir):
                if exec_name in files:
                    return os.path.join(root, exec_name)
        
        # Fallback to check system PATH
        for path in os.getenv("PATH", "").split(os.pathsep):
            msbuild_path = os.path.join(path, exec_name)
            if os.path.isfile(msbuild_path):
                return msbuild_path
    
        raise FileNotFoundError("msbuild.exe not found on this system.")


    def _find_sln_path(self):
        """ Find the .sln file on Windows. """
        search_command = 'findstr /S /I ".sln" *'
    
        try:
            result = subprocess.run(
                search_command,
                shell=True,
                capture_output=True,
                text=True,
                check=True,
            )
            matches = result.stdout.strip().splitlines()
            return matches[0] if matches else None
        except subprocess.CalledProcessError as e:
            print(f"Error while searching for .sln file: {e.stderr}")
            return None


    def make_run(self):
        """ Build and run the project. """
        if _is_windows():
            try:
                res = subprocess.run(self.cmd, shell=True, capture_output=True, text=True)
            except Exception as e:
                print(f"Error while building: {e}")
                sys.exit(1)

            if res.returncode != 0:
                print(f"Build failed with code {res.returncode}:")
                print(res.stderr)
                return "", False

            # Assuming the output directory is Debug or Release
            debug_dir = os.path.join(os.path.dirname(self.config), "Debug")
            exe_files = [f for f in os.listdir(debug_dir) if f.endswith(".exe")]

            if not exe_files:
                print("No executable found in the Debug directory.")
                sys.exit(1)

            exe_path = os.path.join(debug_dir, exe_files[0])
            print(f"Executing: {exe_path}")

            try:
                res = subprocess.run([exe_path], capture_output=True, text=True)
                print("Program output:")
            except Exception as e:
                print(f"Error while executing the executable: {e}")
                sys.exit(1)

            if res.returncode != 0:
                print(res.stdout)
                return res.stdout, False
            return res.stdout, True

        # For non-Windows (assuming Linux/Mac)
        res = subprocess.run(self.cmd, shell=True, capture_output=True, text=True)
        if res.returncode != 0:
            print(res.stderr)
            return "", False

        # Use grep to find the executable name
        cmd = f"grep -oP '(?<=add_executable\\()\\w+' {self.config}"
        res = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        if res.returncode != 0:
            executable_name = "main"
        else:
            executable_name = res.stdout.strip()

        cmd = f"./build/{executable_name}"
        res = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        if res.returncode != 0:
            return res.stdout, False
        return res.stdout, True


# May take one file, or multiple files, or nested lists of files. Handles the
# logic of handling those cases for a cleaner API where one file is processed
# at one time to the client.
class FileProcessor:
    def __init__(self, files, op):
        """
        Initialize the FileProcessor with files and the operation (op) to apply on each file.
        
        :param files: Can be a single file, a list of files, or nested lists of files.
        :param op: The operation to open the file handle as (e.g., `"r"`, `"w"`, etc.).
        """
        # Normalize files to a list, even if a single file is passed.
        self.files = self._normalize_files(files)
        self.op = op
        self._index = 0 # Index for iterating through files.
        self._curr = None
        self._fh = None


    # CREDIT: OpenAI's ChatGPT
    def _normalize_files(self, files):
        """
        Normalize the input so it always returns a flat list of file paths, 
        even if nested lists are provided.
        """
        if isinstance(files, str):  # Single file (string)
            return [files]
        elif isinstance(files, list):  # List of files or nested lists
            flat_files = []
            for item in files:
                if isinstance(item, str):  # If item is a file (string), add it
                    flat_files.append(item)
                elif isinstance(item, list):  # If item is a nested list, flatten it
                    flat_files.extend(self._normalize_files(item))
            return flat_files
        else:
            raise ValueError("Files should be a string, list of strings, or nested list of strings.")


    # CREDIT: OpenAI's ChatGPT
    def __iter__(self):
        """
        Make the class iterable, so we can use it in a loop to process files one at a time.
        """
        return self


    # CREDIT: OpenAI's ChatGPT
    def __next__(self):
        """
        Return the next file path, and apply the operation (op) to it.
        """
        if self._index < len(self.files):
            file_path = self.files[self._index]
            self._index += 1
            self._curr = file_path  # Keep track of the current file
          
            # Close the last open file handle (if there is one).
            if self._fh:
                self._fh.close()
            self._fh = self._open(file_path)
            return self._fh
        else:
            # Close the final open file handle (if there is one).
            if self._fh:
                self._fh.close()
            raise StopIteration


    # CREDIT: OpenAI's ChatGPT
    # xxx look into contextlib from contextmanager, `@contextmanager`
    def _open(self, file_path):
        """
        Open the file with the specified operation and return the file handle.
        
        :param file_path: The path to the file to be opened.
        :return: The file handle.
        """
        try:
            # Open the file in the given mode (op) and return the file handle
            fh = open(file_path, self.op)
            return fh  # Return the file handle so the client can read/write as needed
        except FileNotFoundError:
            raise Exception(f"File not found: {file_path}")
        except PermissionError:
            raise Exception(f"Permission denied: {file_path}")
        except Exception as e:
            raise Exception(f"An error occurred while processing {file_path}: {e}")


    # CREDIT: OpenAI's ChatGPT
    def get_type(self):
        """
        Return the file extension (type) of the current file being processed.
        """
        if self._curr is None:
            raise ValueError("No current file being processed. Make sure to call __next__() first.")

        _, ext = os.path.splitext(self._curr)
        return ext  # Returns the file extension (e.g., '.hpp', '.cpp', etc.)


class Grader:
    def __init__(self, shell, clazz):
        self.shell = shell
        self.clazz = clazz
        self.files = self._get_files()
        if not _check_files(self.files):
            print(f"Grader was unable to find files: {self.files}. Exiting...")
            sys.exit(1)

    def _get_files(self):
        files = {
            "hpp" : [],
            "cpp" : []
        }

        words = _split_clazz_name(self.clazz)
        args = ' '.join(words)

        # Run the shell scripts to find `.hpp` and `.cpp` files.
        for ext in ["hpp", "cpp"]:
            stdout, stderr, code = self.shell.cmd(f"./find_{ext}.sh {args}")
            if stdout.strip():  # Only add if there are results.
                files[ext] = stdout.strip().splitlines()

        return files


    """Check for the presence of functions in the class header and definition files."""
    def check_func(self):
        # xxx read in, maybe from json
        func_hpp = {
            f"class {self.clazz} {{" : False,
            "HashNode** getTable(" : False,
            "int getSize(" : False,
            "bool isEmpty(" : False,
            "int getNumberOfItems(" : False,
            "bool add(" : False,
            "bool remove(" : False,
            "void clear(" : False,
            "HashNode* getItem(" : False,
            "bool contains(" : False,
        }

        func_comments = func_hpp.copy()

        func_cpp = {
            k.split(' ', 1)[0] + f" {self.clazz}::{k.split(' ', 1)[1]}" : v
            for k, v in func_hpp.items()
        }

        # Check the `.hpp` file for function declarations and method headers.
        processor = FileProcessor(self.files["hpp"], 'r')
        for fh in processor:
            lines = fh.readlines()
            for idx, line in enumerate(lines):
                for fn, visited in func_hpp.items():
                    if visited:
                        continue
                    if fn in line:
                        func_hpp[fn] = True
                        if "*/" in lines[idx - 1] or "//" in lines[idx - 1]:
                            func_comments[fn] = True

        # Check the `.cpp` file for function definitions.
        processor = FileProcessor(self.files["cpp"], 'r')
        for fh in processor:
            buf = fh.read()
            for fn, visited in func_cpp.items():
                if visited:
                    continue
                if fn in buf:
                    func_cpp[fn] = True

        # Calculate the total score from functions that are there and their
        # method headers.
        func_score = 1
        clazz_comment = 0
        if func_hpp[f"class {self.clazz} {{"]:
            clazz_comment = 1
        func_hpp.pop(f"class {self.clazz} {{")

        func_frac = len(func_hpp) * 2
        comments_score = 1
        for k, v in func_hpp.items():
            if not v:
                func_score -= (1 / func_frac)
        for k, v in func_cpp.items():
            if not v:
                func_score -= (1 / func_frac)
        for k, v in func_comments.items():
            if not v:
                comments_score -= (1 / len(func_comments))

        return func_score, comments_score, clazz_comment


    def _check_header_dates(self, header):
        """
        This function extracts and parses date-like strings from the header.
        It returns a list of successfully parsed dates.
        """
        # Regex to match potential date formats in the header
        date_pattern = r"\b[\d/.-]+\b"
        date_strings = [match.group() for match in re.finditer(date_pattern, header)]

        parsed_dates = []
        for date_string in date_strings:
            try:
                parsed_date = parser.parse(date_string)  # Try to parse the date
                parsed_dates.append(parsed_date)  # Append the valid parsed date
            except ValueError:
                # If parsing fails, skip the invalid date
                continue
        return parsed_dates  # Return the list of valid dates    


    def check_headers(self):
        date_pattern = r"\d{2}/\d{2}/\d{2}"
        # Pattern for "Created by" or "Modified by" followed by a name
        name_pattern = r"(Created by|Modified by)\s+[\w\s]+"
    
        headers = { "hpp" : True, "cpp" : True }

        # Flatten dictionary and just send all files to FileProcessor; it will
        # determine file type.
        files = list(self.files.values())
        processor = FileProcessor(files, 'r')
        for fh in processor:
            lines = fh.readlines()
            # If file doesn't contain beginning comment block, it doesn't have
            # a header.
            if "/**" not in lines[0]:
                headers[fh.get_type()] = False
                continue
            
            # Find the end of the comment block.
            end = "".join(lines).find("*/")
            if end == -1:
                # Malformed comment block.
                headers[fh.get_type()] = False
                continue
            
            # Extract the header content.
            header = "".join(lines[:end]).strip()
            
            # Check if the file name is in the header.
            print(f"Looking for {fh.name} in header.")  # debug
            if fh.name() not in header:
                headers[fh.get_type()] = False
                continue
            
            # Check if header contains valid dates.
            if not self._check_header_dates(header):
                headers[fh.get_type()] = False
                continue
            
            # Check if the header contains the "Created by" or "Modified by" string.
            if not re.search(name_pattern, header):
                headers[fh.get_type()] = False
                continue

        score = 1
        for k, v in headers.items():
            if not v:
                score -= (1 / len(headers))
        return score


    def check_list(self):
        pts = 1
    
        found_lst = True
        lst_names = {
            "list",
            "SLL",
            "DLL",
            "SinglyLinkedList",
            "DoublyLinkedList"
        }

        # Flatten dictionary and just send all files to FileProcessor; it will
        # determine file type.
        files = list(self.files.values())
        processor = FileProcessor(files, 'r')
        for fh in processor: 
            found = False
            buf = fh.read()
            # Check if any list-related name exists in the buffer.
            if any(name in buf for name in lst_names):
                found = True
                break

            if not found:
                found_lst = False

        return pts if found_lst else 0


    def check_prime(self):
        pts = 1
    
        limit = 10000
        primes = list(primerange(2, limit + 1))

        processor = FileProcessor(self.files["hpp"], 'r')
        for fh in processor:
            buf = fh.read()
            # Check if any prime number is in the file.
            if any(str(prime) in buf for prime in primes):
                return pts

        return 0


# CREDIT: OpenAi's ChatGPT
class GradeReporter:
    def __init__(self, shell, clazz):
        self.shell = shell
        self.clazz = clazz
        self.grader = Grader(shell, clazz)
        self.report = {}

    def generate_report(self):
        score = 0
        self.report = {}

        # Extra Credit
        ec_args_lst = [
            "--smart_ptrs",
            "--templates",
            "--gtest",
        ]
        ec_args = ' '.join(ec_args_lst)
        stdout, stderr, code = self.shell.cmd(f"./check-ec.sh {ec_args}")
        try:
            ec_score = float(stdout)
            self.report["extra_credit_score"] = [str(ec_score)]
            score += ec_score
        except ValueError:
            self.report["extra_credit_score"] = ["N/A"]

        # Generic grading (applies to all milestones)
        try:
            headers_score = self.grader.check_headers()
            self.report["headers_score"] = [f"{headers_score:.2f}"]
            score += headers_score
        except Exception as e:
            self.report["headers_score"] = ["Error"]
        
        try:
            func_score, comments_score, clazz_comment = self.grader.check_func()
            self.report["functionality_score"] = [f"{func_score:.2f}"]
            self.report["comments_score"] = [f"{comments_score:.2f}"]
            self.report["class_comment_score"] = [f"{clazz_comment:.2f}"]
            score += func_score + comments_score + clazz_comment
        except Exception as e:
            self.report["functionality_score"] = ["Error"]
            self.report["comments_score"] = ["Error"]
            self.report["class_comment_score"] = ["Error"]

        # HashTable specific grading
        try:
            prime_score = self.grader.check_prime()
            self.report["prime_score"] = [f"{prime_score:.2f}"]
            score += prime_score
        except Exception as e:
            self.report["prime_score"] = ["Error"]

        try:
            list_score = self.grader.check_list()
            self.report["list_score"] = [f"{list_score:.2f}"]
            score += list_score
        except Exception as e:
            self.report["list_score"] = ["Error"]

        # Total score
        self.report["total_score"] = [f"{score:.2f}"]
        return self.report

    def save_report(self, filename="grade_report.txt"):
        """
        Saves the generated report in plain text format.
        Handles cases where report is empty or incomplete.
        """
        try:
            with open(filename, 'w') as f:
                for key, value in self.report.items():
                    f.write(f'{key}="{", ".join(value)}"\n')
            print(f"Report saved to {filename}.")
        except Exception as e:
            print(f"Error saving report: {e}")


# CREDIT: OpenAI's ChatGPT
class ReportDisplayer:
    def __init__(self, report_filename="grade_report.txt"):
        self.report_filename = report_filename

    def display_report(self):
        student_name = self.get_student_name_from_dir()
        if not student_name:
            student_name = "Unknown Student"
        
        print(f"Grade Report for {student_name}")
        print("=" * (len(student_name) + 14))  # Just to add a little formatting to the title

        try:
            with open(self.report_filename, 'r') as f:
                report_lines = f.readlines()
            if not report_lines:
                print("Report is empty or has no valid content.")
                return
        except FileNotFoundError:
            print(f"Error: Report file '{self.report_filename}' not found.")
            return
        except Exception as e:
            print(f"Error reading report file: {e}")
            return

        # Display simplified report
        for line in report_lines:
            key, value = line.strip().split("=")
            value = value.strip('"')
            print(f"{key}: {value}")

    def get_student_name_from_dir(self):
        """
        Gets the student name from the root directory.
        Handles cases where the directory name may be invalid.
        """
        student_name = os.path.basename(os.getcwd())  # Get the current working directory's name
        # If the directory name is empty or has an unusual format, return None to indicate failure
        if not student_name or re.search(r"[^a-zA-Z0-9_]", student_name):
            return None
        return student_name

class SummaryDisplayer:
    def __init__(self, report_dir):
        self.report_dir = report_dir  # Directory containing student reports

    def display_summary(self):
        student_scores = self.generate_summary_report()

        print("Grade Summary Report")
        print("=====================")

        for student_name, total_score in student_scores.items():
            print(f"{student_name}: {total_score:.2f}")

    def generate_summary(self):
        student_scores = {}
        # Loop through the report directory and find all grade reports
        for student_dir in os.listdir(self.report_dir):
            student_path = os.path.join(self.report_dir, student_dir)
            if os.path.isdir(student_path):  # Only process directories
                report_file = os.path.join(student_path, "grade_report.txt")
                if os.path.isfile(report_file):
                    try:
                        with open(report_file, 'r') as f:
                            report_lines = f.readlines()
                            # Look for the total score line in the report
                            for line in report_lines:
                                if "total_score" in line:
                                    score_value = line.split("=")[1].strip().strip('"')
                                    student_scores[student_dir] = float(score_value)
                                    break
                    except Exception as e:
                        print(f"Error reading report for {student_dir}: {e}")
                        student_scores[student_dir] = "Error"
                else:
                    student_scores[student_dir] = "No Report"

        return student_scores

# Example usage:

def generate_all_reports(shell, clazz, base_directory="students"):
    """
    Generate reports for all students in the base directory.
    Assumes each student's directory contains their source code and grading script.
    """
    student_reports = {}
    
    for student_dir in os.listdir(base_directory):
        student_path = os.path.join(base_directory, student_dir)
        if os.path.isdir(student_path):
            os.chdir(student_path)  # Change to the student's directory to simulate their environment
            reporter = GradeReporter(shell, clazz, student_name=student_dir)
            reporter.generate_report()
            reporter.save_report(filename="grade_report.txt")
            total_score = reporter.report.get("total_score", ["0"])[0]
            student_reports[student_dir] = total_score
            os.chdir(base_directory)  # Return to the base directory

    return student_reports


# Grade HashTable.
def grade_hash_table():
    shell = Shell()
    build = Build()

    grader = Grader(shell, "HashTable")
    score = 0

    # Helper script to grade extra credit.
    ec_args_lst = [
        "--smart_ptrs",
        "--templates",
        "--gtest",
    ]
    # Fixing the join statement
    ec_args = ' '.join(ec_args_lst)  # Join the list into a single string

    # Run the extra credit script
    stdout, stderr, code = shell.cmd(f"./check-ec.sh {ec_args}")
    try:
        score += float(stdout)  # Add the extra credit score
    except ValueError as e:
        print(e)

    # Generic grading (applies to all milestones).
    score += grader.check_headers()
    func_score, comments_score, clazz_comment = grader.check_func()
    score += func_score + comments_score + clazz_comment  # Simplified this part

    # HashTable specific grading.
    score += grader.check_prime()
    score += grader.check_list()

    return score  # Return the final grade score


def main():
    final_score = grade_hash_table()  # Capture the final score
    print(f"Final Score: {final_score}")  # Print the final score


if __name__ == "__main__":
    main()
