cd

file=$(pwd)/.local/share/applications/panda-client.desktop

if [ -f $file ] ; then
    rm $file
fi

touch $file

echo "[Desktop Entry]" >> $file
echo "Type=Application" >> $file
echo "Exec=$(pwd)/client/build/client" >> $file
echo "Path=$(pwd)/client/" >> $file
echo "Name=Client" >> $file
echo "Icon=$(pwd)/client/symbols/app.png" >> $file
echo "Terminal=false" >> $file
echo "StartupWMClass=$(pwd)/client/symbols/app.png" >> $file

chmod +x $file

echo "Created for symbol: ${file}"
