echo $#
if [[ $# -lt 3 ]]
then
  echo Usage: renderBrain.sh input_brain.nii.gz input_segmentation.nii.gz  axial_slice_number   input_affine.mat  input_template.nii.gz  output_prefix
  echo Note: input_segmentation is a 0/1 binary image. if no affine is needed, use \"identity.mat\" as input_affine
  exit
fi 

input_brain=$1
input_seg=$2  # 0/1 binary segmentation, e.g. GM or WM+GM
output_prefix=$3
axial_slice_number=$4
input_affine=$5
input_template=$6

vtkVol=${output_prefix}.vtk
screenshot=${output_prefix}_volRender.jpg
slice=${output_prefix}_slice${axial_slice_number}.jpg

ThresholdImage 3 $input_seg seg.nii.gz 1 Inf
ImageMath 3 $input_seg GetLargestComponent $input_seg
ImageMath 3 ./biggestConnectedComponent.nii.gz ME $input_seg 1 
# if [[ ! -s tissueb.nii.gz ]] ; then 
    Atropos -d 3 -a $input_brain -x $input_seg -o tissue.nii.gz -i kmeans[3] -m [0.1,1x1x1] -c [3,0]
    ThresholdImage 3 tissue.nii.gz tissueb.nii.gz  2 3 
    SmoothImage 3  tissueb.nii.gz 0.8 tissueb.nii.gz 
# fi 
MultiplyImages 3  ${input_brain} tissueb.nii.gz ./biggestConnectedComponent.nii.gz 
c3d ./biggestConnectedComponent.nii.gz -stretch 10% 98% 0 255 -clip 0 255 -o $vtkVol
./bin/volumeRender $vtkVol $screenshot 8 0 90 &
# sleep 10
# convert $screenshot -shave 255x255 $screenshot
rm *vtk 
