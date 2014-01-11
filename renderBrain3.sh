#!/bin/bash
if [[ $# -lt 5 ]]
then
  echo Usage: renderBrain.sh input_brain.nii.gz input_mask.nii.gz output_prefix axial_slice_number axistoslice
  echo Note: input_segmentation is a 0/1 binary image. 
  exit
fi 
input_brain=$1    # raw t1
input_seg=$2      # 0/1 binary segmentation, e.g. GM or WM+GM
output_prefix=$3  # output 
axial_slice_number=$4  #Tweak
axistoslice=$5         #Tweak
screenshot=${output_prefix}_volRender.jpg
slice=${output_prefix}_slice${axial_slice_number}.jpg
slicem=${output_prefix}_slice${axial_slice_number}_masked.jpg
cp $input_brain  inbrain.nii.gz 
ThresholdImage 3 $input_seg seg.nii.gz 1 Inf                      #Tweak
ImageMath 3 seg.nii.gz GetLargestComponent seg.nii.gz             #Tweak
ImageMath 3 seg.nii.gz FillHoles seg.nii.gz
ImageMath 3 ./biggestConnectedComponent.nii.gz ME seg.nii.gz 1    #Tweak
                                                                  #Tweak segmentation protocol
Atropos -d 3 -a $input_brain -x seg.nii.gz -o tissue.nii.gz -i kmeans[3] -m [0.1,1x1x1] -c [3,0]
ThresholdImage 3 tissue.nii.gz tissueb.nii.gz  2 3                #Tweak
SmoothImage 3  tissueb.nii.gz 0.8 tissueb.nii.gz                  #Tweak
MultiplyImages 3  inbrain.nii.gz tissueb.nii.gz ./biggestConnectedComponent.nii.gz
ExtractSliceFromImage 3 inbrain.nii.gz  ${output_prefix}_slice.nii.gz $axistoslice ${axial_slice_number}
ConvertToJpg ${output_prefix}_slice.nii.gz  $slice
MultiplyImages 3 inbrain.nii.gz seg.nii.gz  temp.nii.gz
ExtractSliceFromImage 3 ./temp.nii.gz  ${output_prefix}_slice.nii.gz $axistoslice ${axial_slice_number}
ConvertToJpg ${output_prefix}_slice.nii.gz  $slicem
c3d ./biggestConnectedComponent.nii.gz -stretch 10% 98% 0 255 -clip 0 255 -o out.vtk #Tweak 10% higher or lower
./bin/volumeRender out.vtk $screenshot 8 0 90 &  #Tweak last 3 parameters 
sleep 10
rm tissueb.nii.gz tissue.nii.gz out.vtk ${output_prefix}_slice.nii.gz temp.nii.gz seg.nii.gz inbrain.nii.gz ./biggestConnectedComponent.nii.gz 
