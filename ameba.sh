PREBUILTS_VERSION=1.0.3
PREBUILTS_WIN_URL='https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/prebuilts-v1.0.3/prebuilts-win-1.0.3.zip'
PREBUILTS_LINUX_URL='https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/prebuilts-v1.0.3/prebuilts-linux-1.0.3.tar.gz'
PREBUILTS_WIN_URL_ALIYUN='https://rs-wn.oss-cn-shanghai.aliyuncs.com/prebuilts-win-1.0.3.zip'
PREBUILTS_LINUX_URL_ALIYUN='https://rs-wn.oss-cn-shanghai.aliyuncs.com/prebuilts-linux-1.0.3.tar.gz'

function get_script_dir
{
    SCRIPT_NAME="$(readlink -f "${SELF_PATH}")"
    SCRIPT_DIR="$(dirname "${SCRIPT_NAME}")"

    if [ "$SCRIPT_DIR" = '.' ]
    then
       SCRIPT_DIR="$(pwd)"
    fi
    echo "$SCRIPT_DIR"
}

if [ -n "${BASH_SOURCE-}" ]
then
    SELF_PATH="${BASH_SOURCE}"
elif [ -n "${ZSH_VERSION-}" ]
then
    SELF_PATH="${(%):-%x}"
fi

BASE_DIR="$(get_script_dir)"

case `uname` in
Linux)

    if [ -n "$RTK_TOOLCHAIN_DIR" ]; then
        RTK_TOOLCHAIN_DIR="$RTK_TOOLCHAIN_DIR"
        if [[ "${RTK_TOOLCHAIN_DIR:0:1}" == "~" ]]; then
            RTK_TOOLCHAIN_DIR="$HOME${RTK_TOOLCHAIN_DIR:1}"
        fi
    else
        RTK_TOOLCHAIN_DIR=/opt/rtk-toolchain
    fi

	VENV_CMD="python -m venv $BASE_DIR/.venv"
    ACTIVE_CMD="source $BASE_DIR/.venv/bin/activate"
    PREBUILTS_DIR=$RTK_TOOLCHAIN_DIR/prebuilts-linux-$PREBUILTS_VERSION
    PREBUILTS_ZIP_FILE=$PREBUILTS_DIR.tar.gz
    DOWNLOAD_URL=$PREBUILTS_LINUX_URL
    DOWNLOAD_URL_ALIYUN=$PREBUILTS_LINUX_URL_ALIYUN
    alias menuconfig.py='python menuconfig.py'
	alias build.py='python build.py'
	alias flash.py='python flash.py'
	alias monitor.py='python monitor.py'
    ;;
*_NT*)

    if [ -n "$RTK_TOOLCHAIN_DIR" ]; then
        RTK_TOOLCHAIN_DIR=$(cygpath -u "$RTK_TOOLCHAIN_DIR")
    else
        RTK_TOOLCHAIN_DIR=/c/rtk-toolchain
    fi

	VENV_CMD="python -m virtualenv $BASE_DIR/.venv"
    ACTIVE_CMD="source $BASE_DIR/.venv/Scripts/activate"
    PREBUILTS_DIR=$RTK_TOOLCHAIN_DIR/prebuilts-win-$PREBUILTS_VERSION
    PREBUILTS_ZIP_FILE=$PREBUILTS_DIR.zip
    DOWNLOAD_URL=$PREBUILTS_WIN_URL
    DOWNLOAD_URL_ALIYUN=$PREBUILTS_WIN_URL_ALIYUN

    alias menuconfig.py='winpty python menuconfig.py'
	alias build.py='winpty python build.py'
	alias flash.py='winpty python flash.py'
	alias monitor.py='winpty python monitor.py'
    ;;
esac




function update_prebuilts
{
    case $(uname) in
        Linux)
            last_matched_folder=$(find "$RTK_TOOLCHAIN_DIR" -type d -name 'prebuilts-linux*' | sort | tail -n 1)
            ;;
        *_NT*)
            last_matched_folder=$(find "$RTK_TOOLCHAIN_DIR" -type d -name 'prebuilts-win*' | sort | tail -n 1)
            ;;
    esac

    if [[ -n "$last_matched_folder" ]]; then
        echo "use $last_matched_folder to update prebuilts"
        source "$last_matched_folder/setenv.sh"

        if [ ! -f "$PREBUILTS_ZIP_FILE" ]; then
            echo "download.... "
            wget -P $RTK_TOOLCHAIN_DIR $DOWNLOAD_URL

            if [ $? -ne 0 ]; then
                echo "Try to download from $DOWNLOAD_URL_ALIYUN"
                wget -P $RTK_TOOLCHAIN_DIR $DOWNLOAD_URL_ALIYUN
                if [ $? -ne 0 ]; then
                    echo "Download failed. Please check your network or download manually"
                    return 1
                fi
            fi
        fi

        echo "unzip ... "
        case $(uname) in
            Linux)
                tar -xzf $PREBUILTS_DIR.tar.gz -C $RTK_TOOLCHAIN_DIR
                ;;
            *_NT*)
                7z x $PREBUILTS_DIR.zip -o$RTK_TOOLCHAIN_DIR
                ;;
        esac

        if [ $? -ne 0 ]; then
            echo "Unzip failed. Please unzip $PREBUILTS_DIR.zip manually"
            return 1
        fi

        source "$PREBUILTS_DIR/setenv.sh"
    else
        echo "RTK software suite not exist, please download from $DOWNLOAD_URL or $DOWNLOAD_URL_ALIYUN and unzip it at $RTK_TOOLCHAIN_DIR"
        return 1
    fi

    return 0
}


if [ -f "$PREBUILTS_DIR/setenv.sh" ]; then
    source $PREBUILTS_DIR/setenv.sh
else

	rm -rf $BASE_DIR/.venv
    update_prebuilts
    if [ $? -ne 0 ]; then
        return
    fi

fi


EXPECTED_VENV_PATH="$BASE_DIR/.venv"
if [ -z "$VIRTUAL_ENV" ] || [ "$VIRTUAL_ENV" != "$EXPECTED_VENV_PATH" ]; then
    if [ -d "$EXPECTED_VENV_PATH" ]; then
        eval $ACTIVE_CMD
    else
        echo "python virtual environment is not detected, it will be installed automatically..."
        eval $VENV_CMD
        eval $ACTIVE_CMD
        pip install -r $BASE_DIR/tools/requirements.txt
        echo "These python modules are installed:"
        pip list
    fi
fi

python $BASE_DIR/tools/scripts/check_requirements.py

if [ "$(uname)" = "Linux" ]; then
    # 1. Build list of directories to scan
    SCAN_DIRS=()
    # Add all *_gcc_project directories in current directory (one level)
    for d in ./*_gcc_project; do
        [ -d "$d" ] && SCAN_DIRS+=("$d")
    done
    # Add ./tools directory if it exists
    [ -d "./tools" ] && SCAN_DIRS+=("./tools")
    # Example: Add a new directory if needed in future
    # SCAN_DIRS+=("./new_directory")

    # 2. Define file patterns to process (can be expanded)
    FILE_PATTERNS=("*.sh")

    # 3. Find scripts and batch set execute permission (fast)
    # Limit search to max 4 levels, process in parallel based on CPU cores
    for base in "${SCAN_DIRS[@]}"; do
        for pattern in "${FILE_PATTERNS[@]}"; do
            find "$base" -maxdepth 5 -type f -name "$pattern" -print0
        done
    done \
    | xargs -0 -r -P "$(nproc)" -I{} bash -c '
        file="$1"
        if [ ! -x "$file" ]; then
            chmod +x "{}" 2>/dev/null
            if [ $? -ne 0 ]; then
                echo "[WARNING] Failed to add execute permission to binary: $file. Please check permissions or ownership." >&2
            fi
        fi '

    # 4. Fix execute permission for binaries without extensions under PREBUILTS_DIR
    if [ -d "$PREBUILTS_DIR" ]; then
        find "$PREBUILTS_DIR" -type f ! -name "*.*" -print0 \
        | xargs -0 -r -P "$(nproc)" bash -c '
            file="$1"
            if file "$file" | grep -q 'executable'; then
                if [ ! -x "$file" ]; then
                    if [ ! -x "$file" ]; then
                        chmod +x "$file" 2>/dev/null
                        if [ $? -ne 0 ]; then
                            echo "[WARNING] Failed to add execute permission to binary: $file. Please check permissions or ownership." >&2
                        fi
                    fi
                fi
            fi '
    else
        echo "[WARNING] PREBUILTS_DIR not found: $PREBUILTS_DIR" >&2
    fi
fi

info1="First choose IC platform : cd [IC]_gcc_project"
info2="Configure command: menuconfig.py"
info3="Build command: build.py"
info4="Flash command:  flash.py -p COMx"
info5="Monitor command:  monitor.py -p COMx"

echo "================================================================================"
echo "|  $info1"
echo "|  $info2"
echo "|  $info3"
echo "|  $info4"
echo "|  $info5"
echo "================================================================================"
