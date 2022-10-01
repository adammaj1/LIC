#!/bin/bash 
 
# script file for BASH 
# which bash
# save this file as m.sh
# chmod +x m.sh
# ./m.sh
# checked in https://www.shellcheck.net/




printf "make pgm files \n"
gcc basic_lic.c -lm -Wall -march=native -Wextra 

if [ $? -ne 0 ]
then
    echo ERROR: compilation failed !!!!!!
    exit 1
fi


export  OMP_DISPLAY_ENV="TRUE"
printf "display OMP info \n"

printf "run the compiled program\n"
time ./a.out > m.txt

export  OMP_DISPLAY_ENV="FALSE"

printf "change Image Magic settings\n"
export MAGICK_WIDTH_LIMIT=100MP
export MAGICK_HEIGHT_LIMIT=100MP


printf "convert all ppm files to png using Image Magic v 6 convert \n"
# for all ppm files in this directory
for file in *.ppm ; do
  # b is name of file without extension
  b=$(basename "$file" .ppm)
  # convert  using ImageMagic : -resize widthxheight || 
  convert "${b}".ppm -resize '100%' "${b}".png  # iWidth = iHeight* DisplayAspectRatio 
  echo "$file"
done


printf "delete all ppm files \n"
rm ./*.ppm


 
echo OK

printf "info about software \n"
bash --version
make -v
gcc --version
convert -version
convert -list resource
# end
