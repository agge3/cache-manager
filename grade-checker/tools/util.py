import os


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
