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

    ;;
esac


alias build.py='python build.py'

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

if [ -d "$BASE_DIR/.venv" ]; then
    eval $ACTIVE_CMD
else
    echo python virtual environment is not detected, it will be installed automatically...
	eval $VENV_CMD
	eval $ACTIVE_CMD
	pip install -r $BASE_DIR/tools/requirements.txt
	echo "These python modules are installed:"
	pip list
fi

python $BASE_DIR/tools/scripts/check_requirements.py

if [ "$(uname)" = "Linux" ]; then
    chmod -R +x $PREBUILTS_DIR
fi

info1="First choose IC platform : cd [IC]_gcc_project"
info2="Configure command: menuconfig.py"
info3="Build command: build.py"

echo "================================================================================"
echo
echo "|  $info1"
echo "|  $info2"
echo "|  $info3"
echo
echo "================================================================================"
