#!/bin/bash
if [[ $# -lt 1 ]]
then
  echo Usage: $0 input_brain.nii.gz input_mask.nii.gz output_prefix axial_slice_number axistoslice
  echo Note: input_segmentation is a 0/1 binary image.
  exit
fi
input_brain=$1    # raw t1
input_seg=$2      # 0/1 binary segmentation, e.g. GM or WM+GM
output_prefix=$3  # output
screenshot=${output_prefix}_volRender.jpg
screenshot2=${output_prefix}_volRender.png
ThresholdImage 3 $input_seg tissueb.nii.gz  2 6                #Tweak
ImageMath 3 tissueb.nii.gz ME tissueb.nii.gz 2
ImageMath 3 tissueb.nii.gz GetLargestComponent tissueb.nii.gz
ImageMath 3 tissueb.nii.gz MD tissueb.nii.gz 2
SmoothImage 3  tissueb.nii.gz 0.5 tissueb.nii.gz                  #Tweak
MultiplyImages 3  $input_brain tissueb.nii.gz \
  ./biggestConnectedComponent.nii.gz
/Applications/Convert3DGUI.app/Contents/bin/c3d \
  ./biggestConnectedComponent.nii.gz \
  -stretch 10% 98% 0 255 -clip 0 255 -o out.vtk #Tweak 10% higher or lower
echo render $input_brain
~/code/RenderBrain/bin/volumeRender out.vtk $screenshot 10 0 90 270 #Tweak last 3 parameters
convert -trim $screenshot $screenshot2
# convert $screenshot2 -transparent black $screenshot2
convert $screenshot2 -fuzz 2% -transparent black $screenshot2
# jpegtran -rotate 90 $screenshot $screenshot
rm tissueb.nii.gz out.vtk biggestConnectedComponent.nii.gz  $screenshot
