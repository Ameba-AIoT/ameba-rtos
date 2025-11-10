import os
import platform
import subprocess
import sys
try:
    from packaging.requirements import Requirement
    from packaging.version import parse as parse_version
except ImportError:
    print(" 'packaging' not found. Installing it now...")
    subprocess.check_call([sys.executable, '-m', 'pip', 'install', 'packaging'])
    from packaging.requirements import Requirement
    from packaging.version import parse as parse_version

try:
    from importlib.metadata import distributions
except ImportError:
    # compatibility for python < 3.8
    try:
        from importlib_metadata import distributions
    except ImportError:
        print(" 'importlib-metadata' not found. Installing it now...")
        subprocess.check_call([sys.executable, '-m', 'pip', 'install', 'importlib-metadata'])
        from importlib_metadata import distributions

BLACK = '\033[30m'
RED = '\033[31m'
GREEN = '\033[32m'
YELLOW = '\033[33m'
BLUE = '\033[34m'
MAGENTA = '\033[35m'
CYAN = '\033[36m'
WHITE = '\033[37m'
RESET = '\033[0m'

def read_requirements(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        return [line.strip() for line in file if not line.startswith('#') and line.strip()]

def parse_requirement(requirement):
    parts = requirement.split('==')
    name = parts[0].strip()
    version = None if len(parts) < 2 else parts[1].strip()
    version = version.split(';')[0].strip()
    return name, version

def check_installed_packages(requirements):
    installed_packages = {dist.metadata['Name'].lower(): dist.version for dist in distributions()}
    missing_or_unsatisfied = []
    packages_to_install = []

    for requirement_str in requirements:
        try:
            req = Requirement(requirement_str)
            package_name = req.name.lower()

            if not req.marker or req.marker.evaluate():
                if package_name in installed_packages:
                    installed_version = parse_version(installed_packages[package_name])
                    if req.specifier and not req.specifier.contains(installed_version):
                        missing_or_unsatisfied.append(f"{YELLOW}{package_name} {installed_version} does not satisfy {requirement_str}{RESET}")
                        packages_to_install.append((req.name, str(req.specifier)))
                else:
                    missing_or_unsatisfied.append(f"{RED}{package_name} is not installed{RESET}")
                    packages_to_install.append((req.name, str(req.specifier)))
        except Exception as e:
            print(f"Error parsing requirement '{requirement_str}': {e}")

    return missing_or_unsatisfied, packages_to_install

def install_missing_packages(missing_packages):
    """
    Install each missing or unsatisfied package one by one.
    missing_packages: list of (name, version) tuples
    """
    for name, version in missing_packages:
        if version:
            package_spec = f"{name}{version}"
        else:
            package_spec = name
        print(f"Installing: {package_spec}")
        try:
            subprocess.check_call([sys.executable, '-m', 'pip', 'install', package_spec])
            print(f"{GREEN} {name} installed successfully.{RESET}")
        except subprocess.CalledProcessError as e:
            print(f"{RED} Failed to install {package_spec}: {e}{RESET}")

def main():
    script_path = os.path.abspath(__file__)
    script_dir = os.path.dirname(script_path)
    requirements_file = os.path.join(os.path.dirname(script_dir), 'requirements.txt')
    requirements = read_requirements(requirements_file)
    issues, missing= check_installed_packages(requirements)

    if issues:
        print("PIP CHECK... The following packages are not satisfied or not installed:")
        for issue in issues:
            print(issue)
        if missing:
            print("Installing missing/invalid packages...")
            install_missing_packages(missing)
    else:
        print(f"{GREEN}PIP CHECK... All packages are installed correctly!{RESET}")

if __name__ == "__main__":
    main()

