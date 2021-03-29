#! /bin/bash
SCRIPT_PATH=$(cd "$(dirname "$0")"; pwd -P)
echo "$SCRIPT_PATH"
for csxs in $(defaults domains | tr ',' '\n' | grep com.adobe.CSXS)
do
    echo "Editing CEP domain: $csxs"
    $(defaults write $csxs PlayerDebugMode -string 1)
done

if [ ! -d '/Library/Application Support/Adobe/CEP/extensions/renderBeamerUI' ]
then
   $(sudo mkdir -p '/Library/Application Support/Adobe/CEP/extensions/renderBeamerUI')
fi

if [ -d '/Library/Application Support/Adobe/CEP/extensions/com.acc.renderBeamerUI' ]
then
    $(sudo rm -R '/Library/Application Support/Adobe/CEP/extensions/com.acc.renderBeamerUI')
fi

if [ -f "$SCRIPT_PATH/renderBeamerUI.zxp" ]
then
    sudo unzip -o "$SCRIPT_PATH/renderBeamerUI.zxp" -d "/Library/Application Support/Adobe/CEP/extensions/renderBeamerUI"
else
    echo 'File $SCRIPT_PATH/renderBeamerUI.zxp not found!'
fi

if [ -d '/Library/Application Support/Adobe/Common/Plug-ins/7.0/MediaCore/' ]
then
    if [ -f "$SCRIPT_PATH/renderBeamer_v18_mac.zip" ]
    then
        sudo unzip -o "$SCRIPT_PATH/renderBeamer_v18_mac.zip" -d "/Library/Application Support/Adobe/Common/Plug-ins/7.0/MediaCore/"
    else
        echo 'File $SCRIPT_PATH/renderBeamer18_mac.zip  not found!'
    fi
fi
echo 'Done!'