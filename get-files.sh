# you must set the PREFIX to the environment
# let's cmake create source list file and .pc file
echo "prefix=$PREFIX"
cmake . -DCMAKE_INSTALL_PREFIX=$PREFIX

# replace the ";" to " "
sed 's/;/ /g' files -i

# add "src" prefix
sed 's/ /&src\//g' files -i

# create src_files
cp files src_files
sed 's/files=/src_files=/g' src_files -i
sed 's/[^ ]*\.h//g' src_files -i

# show some information
echo "files is:"
cat files
cp -f files files.ng
echo " "
echo "src_files is:"
cat src_files
cp -f src_files source.ng
echo " "
echo "get files complete, now you may type \"make -f makefile.ng\" to compile."

