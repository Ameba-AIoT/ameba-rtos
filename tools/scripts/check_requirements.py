import importlib.metadata
import os
import platform
import subprocess
import sys

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
    installed_packages = {dist.metadata['Name'].lower(): dist.version for dist in importlib.metadata.distributions()}
    missing_or_unsatisfied = []
    uninstall_packages = []

    for requirement in requirements:

        if platform.system() != 'Windows':
            if 'platform_system=="Windows"' in requirement.replace(' ', ''):
                continue

        try:
            name, required_version = parse_requirement(requirement)
            if name.lower() in installed_packages:
                installed_version = installed_packages[name.lower()]
                if required_version and installed_version != required_version:
                    missing_or_unsatisfied.append(f"{YELLOW}{name} {installed_version} does not satisfy {requirement}{RESET}")
            else:
                missing_or_unsatisfied.append(f"{RED}{name} is not installed{RESET}")
                uninstall_packages.append((name, required_version))
        except Exception as e:
            print(f"Error parsing requirement '{requirement}': {e}")

    return missing_or_unsatisfied, uninstall_packages

def install_missing_packages(missing_packages):
    """
    Install each missing or unsatisfied package one by one.
    missing_packages: list of (name, version) tuples
    """
    for name, version in missing_packages:
        if version:
            package_spec = f"{name}=={version}"
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

