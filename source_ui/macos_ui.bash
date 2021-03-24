#! /bin/bash
for csxs in $(defaults domains | tr ',' '\n' | grep com.adobe.CSXS)
do  
   echo "Editing CEP domain: $csxs"
   $(defaults write $csxs PlayerDebugMode -string 1)
done
if [ ! -d '/Library/Application Support/Adobe/CEP/extensions/' ]
then
   mkdir '/Library/Application Support/Adobe/CEP/extensions/'
fi
if [ -d './com.acc.renderBeamerUI/' ]
then
    $(cp -R com.acc.renderBeamerUI '/Library/Application Support/Adobe/CEP/extensions/')
else
	echo 'Directory ./com.acc.renderBeamerUI/ not found!'
fi
echo 'Done!'