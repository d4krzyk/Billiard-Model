// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <vtkQuad.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>
#include <vtkPolyDataMapper.h>
#include <vtkTexture.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkImageData.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkCellArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>

#include <cmath>
#include <cstdlib>
#include <random>
#include <vtkPolygon.h>
#include <vtkNamedColors.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkJPEGReader.h>
#include <vtkImageReader2Factory.h>
#include <vtkSmoothPolyDataFilter.h>

namespace
{
/**
 * Deform the sphere source using a random amplitude and modes and render it in
 * the window
 *
 * @param sphere the original sphere source
 * @param mapper the mapper for the scene
 * @param window the window to render to
 * @param randEng the random number generator engine
 */
} // namespace

int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
  QApplication app(argc, argv);

  // main window
  QMainWindow mainWindow;
  mainWindow.resize(1200, 900);
  
  // render area
  QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget = new QVTKOpenGLNativeWidget();
  mainWindow.setCentralWidget(vtkRenderWidget);

  // VTK part
  vtkNew<vtkGenericOpenGLRenderWindow> window;
  vtkRenderWidget->setRenderWindow(window.Get());

  ////////////////////////////
  
  // Tworzenie PolyData
  vtkNew<vtkPolyData> polyData;
  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> quads;
  // Parametry siatki
  int pointsX = 400; // Liczba punktów w osi X
  int pointsY = 200;  // Liczba punktów w osi Y
  double width = 10.0;  // Szerokoœæ p³aszczyzny
  double height = 5.0;   // Wysokoœæ p³aszczyzny
  double radius = 8.0; // wielkosc dziur

  vtkNew<vtkImageReader2Factory> readerFactory;
  vtkSmartPointer<vtkImageReader2> textureFile;
  const char* texturePath = "billard_texture.jpg";
  textureFile.TakeReference(readerFactory->CreateImageReader2(texturePath));
  textureFile->SetFileName(texturePath);
  textureFile->Update();

  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(textureFile->GetOutputPort());
  texture->InterpolateOn();


  vtkNew<vtkFloatArray> textureCoordinates;
  textureCoordinates->SetNumberOfComponents(2);  // Dwa komponenty dla (u, v)
  textureCoordinates->SetName("Texture Coordinates");

  
  

  polyData->GetPointData()->SetTCoords(textureCoordinates);

  
  auto distance = [](double x1, double y1, double x2, double y2) {
      return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
      };

  // Dodawanie punktów do siatki
  for (int j = 0; j < pointsY; j++) {
      for (int i = 0; i < pointsX; i++) {
          double x = i * (width / (pointsX - 1));
          double y = j * (height / (pointsY - 1));
          double z = 0.0;
          int distFromUpTab = 18;
          // Podnosimy punkty na krawêdziach, aby stworzyæ efekt band
          if (j <= (distFromUpTab - 2) || j >= pointsY - (distFromUpTab - 2) ||
              i <= (distFromUpTab - 2) || i >= pointsX - (distFromUpTab - 2)) {
              z = 0.3; // Podniesienie krawêdzi 
          }
          if (distance(i, j, distFromUpTab, distFromUpTab) < radius) {
              z = -0.5; // Obni¿enie punktu w okolicy dziurki
          }
          if (distance(i, j, distFromUpTab, pointsY - distFromUpTab) < radius) {
              z = -0.5;
          }
          if (distance(i, j, pointsX - distFromUpTab, distFromUpTab) < radius) {
              z = -0.5;
          }
          if (distance(i, j, pointsX - distFromUpTab, pointsY - distFromUpTab) < radius) {
              z = -0.5; 
          }
          if (distance(i, j, pointsX / 2, distFromUpTab - 2) < radius) {
              z = -0.5;
          }
          if (distance(i, j, pointsX / 2, pointsY - distFromUpTab + 2) < radius) {
              z = -0.5;
          }
          points->InsertNextPoint(x, y, z);
          // Dodawanie wspó³rzêdnych UV
          double u = (i / double(pointsX - 1));  // U wspó³rzêdna
          double v = (j / double(pointsY - 1));  // V wspó³rzêdna
          textureCoordinates->InsertNextTuple2(u, v);

      }
  }
  // Dodawanie kwadratów do siatki
  for (int j = 0; j < pointsY - 1; j++) {
      for (int i = 0; i < pointsX - 1; i++) {
          vtkNew<vtkQuad> quad;
          quad->GetPointIds()->SetId(0, j * pointsX + i);
          quad->GetPointIds()->SetId(1, j * pointsX + (i + 1));
          quad->GetPointIds()->SetId(2, (j + 1) * pointsX + (i + 1));
          quad->GetPointIds()->SetId(3, (j + 1) * pointsX + i);
          quads->InsertNextCell(quad);
      }
  }
  // Dodawanie punktów i komórek (kwadratów) do PolyData
  polyData->SetPoints(points);
  polyData->SetPolys(quads);

  vtkNew<vtkSmoothPolyDataFilter> smoother;
  smoother->SetInputData(polyData);
  smoother->SetNumberOfIterations(60);  // Liczba iteracji wyg³adzania
  smoother->SetRelaxationFactor(0.05);   // Czynnik relaksacji (im wy¿szy, tym szybciej wyg³adza)
  smoother->Update();
  
  polyData->ShallowCopy(smoother->GetOutput());
  // Mapper i Actor
  vtkNew<vtkPolyDataMapper> mapper;
  vtkNew<vtkActor> actor;
  mapper->SetInputData(polyData);
  actor->GetProperty()->SetRepresentationToPoints(); // Ustawienie na punkty
  actor->GetProperty()->SetPointSize(2); // Rozmiar punktów
  //actor->GetProperty()->SetColor(0.2, 0.8, 0.4); // Kolor siatki
  actor->SetMapper(mapper);
  actor->SetTexture(texture);
  actor->GetProperty()->SetRepresentationToSurface();
  //actor->GetProperty()->SetRepresentationToWireframe();
  actor->GetProperty()->SetEdgeVisibility(false);


  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);

  window->AddRenderer(renderer);


  mainWindow.show();

  return app.exec();
}
