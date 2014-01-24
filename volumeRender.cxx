#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkMultiThreader.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkXMLImageDataReader.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>

static void CreateImageData(vtkImageData* im);

int main(int argc, char *argv[])
{
  vtkSmartPointer<vtkImageData> imageData  = NULL;
  vtkSmartPointer<vtkImageData> imageData2 = NULL;
  if (argc < 3)
    {
    // CreateImageData(imageData);
    std::cout << "Usage: volumeRender input.vtk output_screenshot.png <magnification_factor> <elevation> <azimuth> <roll> <image-2>\n";
    std::cout << "Default magnification_factor=3, elevation=0, azimuth=0\n";
    return EXIT_FAILURE;
    }
  else
    {
    vtkSmartPointer<vtkStructuredPointsReader> reader =
      vtkSmartPointer<vtkStructuredPointsReader>::New();
    reader->SetFileName(argv[1]);
    reader->Update();
    imageData = reader->GetOutput();
    }

// set the camera
  double azimuth=0;
  double elevation=0;
  double mag=3;
  double roll=0;
  std::string image2fn;
    if (argc > 7 ) 
    {
      mag = atof(argv[3]);
      azimuth = atof(argv[5]);
      elevation = atof(argv[4]);
      roll = atof( argv[6] );

      vtkSmartPointer<vtkStructuredPointsReader> reader =
	vtkSmartPointer<vtkStructuredPointsReader>::New();
      reader->SetFileName(argv[7]);
      reader->Update();
      imageData2 = reader->GetOutput();
    }    
    if (argc > 6) 
    {
      mag = atof(argv[3]);
      azimuth = atof(argv[5]);
      elevation = atof(argv[4]);
      roll = atof( argv[6] );
    }    
    if (argc > 5 ) 
    {
      mag = atof(argv[3]);
      azimuth = atof(argv[5]);
      elevation = atof(argv[4]);
    }    
    if (argc >  4) 
    {
      mag = atof(argv[3]);
      elevation = atof(argv[4]);
    }
  if (argc > 3 )
    mag = atof(argv[3]);
    

  vtkSmartPointer<vtkRenderWindow> renWin =
    vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderer> ren1 =
    vtkSmartPointer<vtkRenderer>::New();
  ren1->SetBackground(0.0,0.0,0.0);

  renWin->AddRenderer(ren1);

  renWin->SetSize(301,300); // intentional odd and NPOT  width/height

  vtkSmartPointer<vtkRenderWindowInteractor> iren =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);

  renWin->Render(); // make sure we have an OpenGL context.

  vtkSmartPointer<vtkMultiThreader> mThreader = vtkSmartPointer<vtkMultiThreader>::New();

  mThreader->SetGlobalMaximumNumberOfThreads(1);

  vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
    vtkSmartPointer<vtkSmartVolumeMapper>::New();
  volumeMapper->SetBlendModeToComposite(); // composite first
#if VTK_MAJOR_VERSION <= 5
  volumeMapper->SetInputConnection(imageData->GetProducerPort());
#else
  volumeMapper->SetInputData(imageData);
#endif
  vtkSmartPointer<vtkVolumeProperty> volumeProperty =
    vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
//  double opacityLevel = 2048;
//  double opacityWindow = 4096;
  compositeOpacity->AddPoint(0.0,0.0);
//  compositeOpacity->AddPoint(80.0,1.0);
//  compositeOpacity->AddPoint(80.1,0.0);
//  compositeOpacity->AddPoint(255.0,0.0);
  compositeOpacity->AddPoint(255.0,1.0);

//  compositeOpacity->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,
//                                opacityLevel + 0.5*opacityWindow, 1.0 );
  volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.
  //  volumeProperty->SetGradientOpacity(compositeOpacity); // composite first.

  vtkSmartPointer<vtkColorTransferFunction> color =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  color->AddRGBSegment(0, 0.0, 0.0, 0.0,
                       255, 1.0, 1.0, 1.0 );
  
  volumeProperty->SetColor(color);

  vtkSmartPointer<vtkVolume> volume =
    vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  ren1->AddViewProp(volume);

  if ( imageData2 )
    {
      vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper2 =
	vtkSmartPointer<vtkSmartVolumeMapper>::New();
      volumeMapper2->SetBlendModeToComposite(); // composite first
      volumeMapper2->SetInputData(imageData2);
      vtkSmartPointer<vtkVolumeProperty> volumeProperty2 =
	vtkSmartPointer<vtkVolumeProperty>::New();
      volumeProperty2->ShadeOff();
      volumeProperty2->SetInterpolationType(VTK_LINEAR_INTERPOLATION);
      vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity2 =
	vtkSmartPointer<vtkPiecewiseFunction>::New();
      compositeOpacity2->AddPoint(0.0,0.0);
      compositeOpacity2->AddPoint(124.0,0.);
      compositeOpacity2->AddPoint(125.0,0.25);
      compositeOpacity2->AddPoint(255.0,0.75);
      volumeProperty2->SetScalarOpacity(compositeOpacity2); // composite first.
      volumeProperty2->SetGradientOpacity(compositeOpacity); // composite first.
      vtkSmartPointer<vtkColorTransferFunction> color2 =
	vtkSmartPointer<vtkColorTransferFunction>::New();
      color2->AddRGBSegment(0, 0.0, 0.0, 0.0,
                       255, 1.0, 0.0, 0.0 );
      volumeProperty2->SetColor(color2);
      vtkSmartPointer<vtkVolume> volume2 =
	vtkSmartPointer<vtkVolume>::New();
      volume2->SetMapper(volumeMapper2);
      volume2->SetProperty(volumeProperty2);
      ren1->AddViewProp(volume2);
    }

  ren1->ResetCamera();
  vtkSmartPointer<vtkCamera> cam;
  cam = ren1->GetActiveCamera();
  cam->Azimuth(azimuth);
  cam->Elevation(elevation);
  cam->Roll(roll);
  // Render composite. In default mode. For coverage.
  renWin->Render();

  // Screenshot
  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = 
  vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renWin);
  windowToImageFilter->SetMagnification(mag); //set the resolution of the output image (3 times the current resolution of vtk render window)
  windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
  windowToImageFilter->Update();
  
  vtkSmartPointer<vtkPNGWriter> writer = 
  vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName(argv[2]);
  writer->SetInputConnection(windowToImageFilter->GetOutputPort());
  writer->Write();
  renWin->Render();
  iren->Start();
  return EXIT_SUCCESS;
}

void CreateImageData(vtkImageData* imageData)
{
  // Create a spherical implicit function.
  vtkSmartPointer<vtkSphere> sphere =
    vtkSmartPointer<vtkSphere>::New();
  sphere->SetRadius(0.1);
  sphere->SetCenter(0.0,0.0,0.0);

  vtkSmartPointer<vtkSampleFunction> sampleFunction =
    vtkSmartPointer<vtkSampleFunction>::New();
  sampleFunction->SetImplicitFunction(sphere);
  sampleFunction->SetOutputScalarTypeToDouble();
  sampleFunction->SetSampleDimensions(127,127,127); // intentional NPOT dimensions.
  sampleFunction->SetModelBounds(-1.0,1.0,-1.0,1.0,-1.0,1.0);
  sampleFunction->SetCapping(false);
  sampleFunction->SetComputeNormals(false);
  sampleFunction->SetScalarArrayName("values");
  sampleFunction->Update();

  vtkDataArray* a = sampleFunction->GetOutput()->GetPointData()->GetScalars("values");
  double range[2];
  a->GetRange(range);

  vtkSmartPointer<vtkImageShiftScale> t =
    vtkSmartPointer<vtkImageShiftScale>::New();
  t->SetInputConnection(sampleFunction->GetOutputPort());

  t->SetShift(-range[0]);
  double magnitude=range[1]-range[0];
  if(magnitude==0.0)
    {
    magnitude=1.0;
    }
  t->SetScale(255.0/magnitude);
  t->SetOutputScalarTypeToUnsignedChar();

  t->Update();

  imageData->ShallowCopy(t->GetOutput());
}
