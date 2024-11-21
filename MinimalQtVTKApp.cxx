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
#include <vtkLight.h>
#include <vtkLightActor.h>
#include <vtkLightCollection.h>
#include <vtkNamedColors.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>
#include <vtkPolyDataMapper.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTransform.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkImageData.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkCellArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <cmath>
#include <cstdlib>
#include <random>
#include <vtkRendererCollection.h>
#include <vtkPolygon.h>
#include <vtkNamedColors.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkJPEGReader.h>
#include <vtkImageReader2Factory.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkCubeSource.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTexturedSphereSource.h>
#include <vtkCamera.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkBoxWidget.h>
#include <vtkInteractorStyleTrackballCamera.h>
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
void SetUpCamera(vtkSmartPointer<vtkRenderer> renderer, double width, double height, double z)
{
    // Œrodek sto³u
    double tableCenter[3] = { width / 2.0, height / 2.0 - 4, z + 2.0 };

    // Tworzymy kamerê
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

    // Pozycja kamery - ustawiamy j¹ wy¿ej i dalej od sto³u, aby uzyskaæ lepszy widok
    double cameraPosition[3] = { width / 2.0, height / 2.0, z + 5.0 }; // Kamera znajduje siê nad sto³em
    camera->SetPosition(cameraPosition);
    camera->SetFocalPoint(tableCenter);  // Kamera patrzy na œrodek sto³u
    // Ustawienie k¹ta widzenia kamery
    camera->UseHorizontalViewAngleOn();

    // Ustawienie k¹ta obrotu kamery, aby ograniczyæ obrót do osi Y (lewo/prawo)
    //camera->Elevation(10.00);
    //camera->Yaw(-60.00);
    //camera->Roll(90.00);
    //camera->Azimuth(45.00);
    camera->SetViewUp(0.0, 0.0, 1.0);
    // Dodajemy ograniczenie obrotu kamery tylko wokó³ osi Y
    // Obrót w lewo lub w prawo odbywa siê poprzez azimuth
    //camera->Elevation(5.00);
    //camera->Azimuth(190.00);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
}



class SmoothCameraRotation
{
public:
    SmoothCameraRotation(vtkRenderer* renderer, vtkRenderWindowInteractor* interactor, vtkCamera* camera)
        : Renderer(renderer), Interactor(interactor), CurrentStep(0), TotalSteps(90)
    {
        AngleIncrement = 45.0 / TotalSteps; // 45 stopni w 90 krokach
    }

    // Funkcja startuj¹ca animacjê
    void Start()
    {
        vtkNew<vtkCallbackCommand> timerCallback;
        timerCallback->SetCallback(SmoothCameraRotation::RotateCamera);
        timerCallback->SetClientData(this); // Przekazanie wskaŸnika do obiektu
        Interactor->AddObserver(vtkCommand::TimerEvent, timerCallback);
        TimerId = Interactor->CreateRepeatingTimer(10); // Co 10 ms
    }

    // Funkcja callback do obs³ugi rotacji kamery
    static void RotateCamera(vtkObject*, unsigned long, void* clientData, void*)
    {

        auto* self = static_cast<SmoothCameraRotation*>(clientData);
        if (!self || !self->Renderer)
            return;

        vtkCamera* camera = self->Renderer->GetActiveCamera();
        
        if (!camera)
            return;

        if (self->CurrentStep < self->TotalSteps)
        {
            if (self->CurrentStep == 0)
            {
                camera->SetViewUp(0, 0, 1);
            }
            camera->Azimuth(self->AngleIncrement); // Obrót kamery
            self->Interactor->GetRenderWindow()->Render(); // Odœwie¿ widok
            ++self->CurrentStep;
        }
        else
        {
            // Zakoñczenie animacji
            self->Interactor->DestroyTimer(self->TimerId);
        }
    }

private:
    vtkRenderer* Renderer;
    vtkRenderWindowInteractor* Interactor;
    int TimerId;
    int CurrentStep;
    const int TotalSteps;
    double AngleIncrement;
};
class CameraInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static CameraInteractorStyle* New();
    vtkTypeMacro(CameraInteractorStyle, vtkInteractorStyleTrackballCamera);

    // Funkcja do obracania kamery o 45 stopni w azymucie

    CameraInteractorStyle() : IsDrag(false) {}


    void OnRightButtonDown() override
    {
        // Uzyskanie renderera z interaktora
        vtkRenderWindowInteractor* interactor = this->GetInteractor();
        vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
        if (renderer)
        {
            auto cameraAnimation = new SmoothCameraRotation(renderer, interactor, camera);
            cameraAnimation->Start();
            
        }

        vtkInteractorStyleTrackballCamera::OnRightButtonDown(); // Wywo³anie domyœlnej obs³ugi zdarzenia
    }
    void OnLeftButtonDown() override
    {
        //IsDrag = true;
        // Uzyskanie renderera z interaktora
        //vtkRenderWindowInteractor* interactor = this->GetInteractor();
        //vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

        //if (renderer)
        //{
        //    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
        //    camera->Azimuth(45); // Obrót o 45 stopni w azymucie
        //    interactor->GetRenderWindow()->Render(); // Aktualizuj okno renderowania
        //}

        //vtkInteractorStyleTrackballCamera::OnLeftButtonDown(); // Wywo³anie domyœlnej obs³ugi zdarzenia
    }
    // Wykonywana podczas przesuwania myszk¹, ale zablokowana, gdy IsDrag jest false
    void OnMouseMove() override
    {
        if (IsDrag)
        {
            vtkInteractorStyleTrackballCamera::OnMouseMove(); // Obrót kamery tylko wtedy, gdy przycisk jest wciœniêty
        }
    }

private:
    bool IsDrag; // Flaga mówi¹ca, czy myszka jest wciœniêta
};

// Definicja metody New, by poprawnie tworzyæ obiekt
vtkStandardNewMacro(CameraInteractorStyle);


// Klasa reprezentuj¹ca kulê z ruchem
class MovingSphere {
public:
    vtkSmartPointer<vtkActor> Actor;
    double Velocity[3]; // Prêdkoœæ w osiach X, Y, Z
    double Bounds[4];   // Min i max dla obszaru w X i Y (Bounds: [minX, maxX, minY, maxY])

    MovingSphere(vtkSmartPointer<vtkActor> actor, double bounds[4]) : Actor(actor) {
        Velocity[0] = 0.01; Velocity[1] = 0.015; Velocity[2] = 0.0; // Ustaw prêdkoœci pocz¹tkowe
        for (int i = 0; i < 4; i++) Bounds[i] = bounds[i];
    }

    void UpdatePosition(double dt) {
        double position[3];
        Actor->GetPosition(position);

        // Aktualizacja pozycji
        double newX = position[0] + Velocity[0] * dt;
        double newY = position[1] + Velocity[1] * dt;

        // Kolizje z granicami
        if (newX < Bounds[0] || newX > Bounds[1]) Velocity[0] = -Velocity[0]; // Odbicie w X
        if (newY < Bounds[2] || newY > Bounds[3]) Velocity[1] = -Velocity[1]; // Odbicie w Y

        // Aktualizacja pozycji z odbiciami
        Actor->SetPosition(
            position[0] + Velocity[0] * dt,
            position[1] + Velocity[1] * dt,
            position[2] + Velocity[2] * dt
        );
    }
};

// Funkcja aktualizuj¹ca pozycjê w timerze
void UpdateSpherePosition(vtkObject* caller, unsigned long, void* clientData, void*) {
    vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);
    MovingSphere* movingSphere = static_cast<MovingSphere*>(clientData);

    movingSphere->UpdatePosition(1.0); // Aktualizacja pozycji (dt = 1.0 jako arbitralny czas krokowy)
    interactor->GetRenderWindow()->Render(); // Renderuj po aktualizacji
}



// Funkcja do generowania œcie¿ki do tekstury
char* getTexturePath(double r, double g, double b, bool hasStripe) {
    // Kolory kul (w formacie RGB)
    if (!hasStripe) {
        if (r == 1.0 && g == 1.0 && b == 1.0) return "textures/white_ball_texture.jpg";  // Bia³a kula
        if (r == 1.0 && g == 0.0 && b == 0.0) return "textures/red_ball_texture.jpg";    // Czerwona kula
        if (r == 0.0 && g == 1.0 && b == 0.0) return "textures/green_ball_texture.jpg";  // Zielona kula
        if (r == 0.0 && g == 0.0 && b == 1.0) return "textures/blue_ball_texture.jpg";   // Niebieska kula
        if (r == 1.0 && g == 1.0 && b == 0.0) return "textures/yellow_ball_texture.jpg"; // ¯ó³ta kula
        if (r == 1.0 && g == 0.0 && b == 1.0) return "textures/purple_ball_texture.jpg"; // Fioletowa kula
        if (r == 0.0 && g == 1.0 && b == 1.0) return "textures/orange_ball_texture.jpg"; // Pomarañczowa kula
        if (r == 0.5 && g == 0.5 && b == 0.5) return "textures/brown_ball_texture.jpg";  // Br¹zowa kula
    }
    if (hasStripe) {
        // Dla kul z paskiem dodaj prefiks 'st_' przed kolorem
        if (r == 0.5 && g == 0.5 && b == 1.0) return "textures/st_blue_ball_texture.jpg";   // Niebieska z paskiem
        if (r == 1.0 && g == 0.0 && b == 1.0) return "textures/st_yellow_ball_texture.jpg"; // ¯ó³ta z paskiem
        if (r == 0.0 && g == 1.0 && b == 0.0) return "textures/st_red_ball_texture.jpg";    // Czerwona z paskiem
        if (r == 1.0 && g == 0.0 && b == 0.0) return "textures/st_green_ball_texture.jpg";  // Zielona z paskiem
        if (r == 0.0 && g == 0.0 && b == 1.0) return "textures/st_blue_ball_texture.jpg";   // Niebieska z paskiem
        if (r == 0.0 && g == 0.5 && b == 0.5) return "textures/st_orange_ball_texture.jpg"; // Pomarañczowa z paskiem
        if (r == 0.5 && g == 0.0 && b == 0.0) return "textures/st_purple_ball_texture.jpg"; // Fioletowa z paskiem
        if (r == 0.5 && g == 0.5 && b == 0.5) return "textures/st_brown_ball_texture.jpg";     // Br¹zowa z paskiem
    }
    return "textures/black_ball_texture.jpg";  // Kula 8 (czarna)
}

// Funkcja do tworzenia kuli bilarda
vtkSmartPointer<vtkActor> createBall(double x, double y, double z, double r, double g, double b, bool hasStripe) {
    vtkNew<vtkTexturedSphereSource> sphereSource;
    //sphereSource->SetCenter(x, y, z);  // Pozycja kuli
    sphereSource->SetRadius(0.15);    // Promieñ kuli
    sphereSource->SetPhiResolution(50);  // Rozdzielczoœæ kuli
    sphereSource->SetThetaResolution(50);  // Rozdzielczoœæ kuli



    // £adowanie tekstury z pliku
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> textureFile;
    // Pobranie œcie¿ki do tekstury
    const char* texturePath = getTexturePath(r, g, b, hasStripe);

    textureFile.TakeReference(readerFactory->CreateImageReader2(texturePath));
    textureFile->SetFileName(texturePath);
    //textureFile.TakeReference(readerFactory->CreateImageReader2("textures/test.jpg"));
    //textureFile->SetFileName("textures/test.jpg");
    textureFile->Update();
    //texturePath = "textures/white_ball_texture.jpg";
    
    if (textureFile->GetOutput()->GetDimensions()[0] == 0) {
        //std::cerr << "Error: Failed to load texture from " << texturePath << std::endl;
    }
    else {
        //std::cout << "Texture loaded successfully: " << texturePath << std::endl;
    }

    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(textureFile->GetOutputPort());

    // Mapowanie tekstury na kulê
    vtkNew<vtkTextureMapToSphere> textureMapToSphere;
    textureMapToSphere->SetInputConnection(sphereSource->GetOutputPort());

    // Tworzymy transformacjê wspó³rzêdnych tekstury
    vtkNew<vtkTransformTextureCoords> transformTexture;
    transformTexture->SetInputConnection(textureMapToSphere->GetOutputPort());
    transformTexture->AddPosition(-0.75, 0.0, 0.0);
    // Skalowanie wspó³rzêdnych tekstury w poziomie
    transformTexture->SetScale(-0.5, -1.0, 1.0);  // Rozci¹ganie tekstury w osi X
    
    vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    sphereMapper->SetInputConnection(transformTexture->GetOutputPort());

    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->SetTexture(texture); 
    //texture->SetInputData(textureImage);
    
    sphereActor->SetPosition(x, y, z);
    sphereActor->GetProperty()->SetSpecular(0.4);  // Dodaj po³ysk
    sphereActor->GetProperty()->SetSpecularPower(50.0);
    sphereActor->GetProperty()->SetColor(1, 1, 1);  // Kolor kuli
    sphereActor->RotateY(-180);
    sphereActor->RotateZ(270);
    sphereActor->RotateX(90);
    return sphereActor;
}



int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
  QApplication app(argc, argv);

  qDebug("Tworzenie glownego okna...");
  QMainWindow mainWindow;
  mainWindow.resize(1200, 900);
  qDebug("Tworzenie widgetu renderowania...");
  // render area
  QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget = new QVTKOpenGLNativeWidget();

  mainWindow.setCentralWidget(vtkRenderWidget);

  qDebug("Tworzenie okna renderowania...");
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> window = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

  vtkRenderWidget->setRenderWindow(window.Get());

  // VTK part

  vtkNew<vtkRenderer> renderer;
  window->AddRenderer(renderer);

  //vtkNew<vtkRenderWindowInteractor> interactor;
  //interactor->SetRenderWindow(window.Get());

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
  const char* texturePath = "textures/billard_texture.jpg";
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


  qDebug("Tworzenie stolu bilardowego...");
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
              if (i == (distFromUpTab - 2)) {
                  x = x + 0.30;
              }
              else if (j == (distFromUpTab - 2)){
                  y = y + 0.30;
              }
              else if (i == pointsX - (distFromUpTab - 2)) {
                  x = x - 0.30;
              }
              else if (j == pointsY - (distFromUpTab - 2)) {
                  y = y - 0.30;
                  
              }
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
  qDebug("Wygladzenie stolu...");
  vtkNew<vtkSmoothPolyDataFilter> smoother;
  smoother->SetInputData(polyData);
  smoother->SetNumberOfIterations(100);  // Liczba iteracji wyg³adzania
  smoother->SetRelaxationFactor(0.06);   // Czynnik relaksacji (im wy¿szy, tym szybciej wyg³adza)
  smoother->Update();

  ///ŒWIAT£O
  qDebug("Dodawanie swiatla...");
  vtkNew<vtkLight> light;
  light->SetLightTypeToSceneLight();   // Œwiat³o sceniczne
  light->SetPosition(width / 2 - 3, height / 2 - 3, 5);       // Pozycja œwiat³a (z góry nad œrodkiem sceny)
  light->SetFocalPoint(width / 2 - 1, height / 2 - 1, 0.5);      // Kierunek œwiat³a (œrodek sceny)
  light->SetColor(1.0, 1.0, 1.0);     // Bia³e œwiat³o
  light->SetIntensity(0.35);

  vtkNew<vtkLight> light2;
  light2->SetLightTypeToSceneLight();   // Œwiat³o sceniczne
  light2->SetPosition(width / 2 + 3, height / 2 + 3, 5);       // Pozycja œwiat³a (z góry nad œrodkiem sceny)
  light2->SetFocalPoint(width / 2 + 1, height / 1 + 1, 0.5);      // Kierunek œwiat³a (œrodek sceny)
  light2->SetColor(1.0, 1.0, 1.0);     // Bia³e œwiat³o
  light2->SetIntensity(0.35);

  vtkNew<vtkLight> light3;
  light3->SetLightTypeToSceneLight();   // Œwiat³o sceniczne
  light3->SetPosition(width / 2 - 3, height / 2 + 3, 5);       // Pozycja œwiat³a (z góry nad œrodkiem sceny)
  light3->SetFocalPoint(width / 2 + 1, height / 1 - 1, 0.5);      // Kierunek œwiat³a (œrodek sceny)
  light3->SetColor(1.0, 1.0, 1.0);     // Bia³e œwiat³o
  light3->SetIntensity(0.35);

  vtkNew<vtkLight> light4;
  light4->SetLightTypeToSceneLight();   // Œwiat³o sceniczne
  light4->SetPosition(width / 2 + 3, height / 2 - 3, 5);       // Pozycja œwiat³a (z góry nad œrodkiem sceny)
  light4->SetFocalPoint(width / 2 - 1, height / 1 + 1, 0.5);      // Kierunek œwiat³a (œrodek sceny)
  light4->SetColor(1.0, 1.0, 1.0);     // Bia³e œwiat³o
  light4->SetIntensity(0.35);
  
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

  
  renderer->AddActor(actor);
  renderer->AddLight(light);          // Dodanie œwiat³a do renderera
  renderer->AddLight(light2);
  renderer->AddLight(light3);
  renderer->AddLight(light4);
  qDebug("Ustawianie kamery...");
  // Ustawienie kamery
  SetUpCamera(renderer, width, height, 0.5);

  // Ustawienie nowego stylu interakcji dla renderera
  vtkSmartPointer<CameraInteractorStyle> style = vtkSmartPointer<CameraInteractorStyle>::New();
  vtkSmartPointer<vtkRenderWindowInteractor> interactor = window->GetInteractor();
  interactor->SetInteractorStyle(style);


  // Tworzenie obiektu reprezentuj¹cego pozycjê œwiat³a
  vtkSmartPointer<vtkSphereSource> lightPositionSphere = vtkSmartPointer<vtkSphereSource>::New();
  lightPositionSphere->SetRadius(0.2); // Ma³a sfera reprezentuj¹ca pozycjê œwiat³a
  lightPositionSphere->SetThetaResolution(10);
  lightPositionSphere->SetPhiResolution(10);
  lightPositionSphere->SetCenter(light->GetPosition());

  vtkSmartPointer<vtkPolyDataMapper> lightPositionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  lightPositionMapper->SetInputConnection(lightPositionSphere->GetOutputPort());

  vtkSmartPointer<vtkActor> lightPositionActor = vtkSmartPointer<vtkActor>::New();
  lightPositionActor->SetMapper(lightPositionMapper);
  lightPositionActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // Czerwony kolor dla pozycji œwiat³a

  renderer->SetBackground2(0.002, 0.0022, 0.04);
  renderer->SetBackground(0.0, 0.0, 0.0);
  renderer->GradientBackgroundOn();
  //renderer->AddActor(lightPositionActor);  // Dodanie reprezentacji pozycji œwiat³a

  ///BILE
  qDebug("Dodawanie bil...");
  std::vector<std::tuple<double, double, double, double, double, double, bool>> balls = {
      {      width / 2 - 2.000, height / 2 + 0.000 , 0.15,  1.0, 1.0, 1.0, false},      // Bia³a kula (cue ball)
      {1.0 + width / 2 + 0.000, height / 2 + 0.000 , 0.15,  1.0, 0.0, 0.0, false},      // Czerwona kula (3)
      {1.0 + width / 2 + 0.275, height / 2 + 0.155 , 0.15,  0.0, 1.0, 0.0, false},      // Zielona kula (6)
      {1.0 + width / 2 + 0.275, height / 2 - 0.155 , 0.15,  1.0, 0.0, 1.0, true},       // ¯ó³ta z paskiem (9)
      {1.0 + width / 2 + 0.545, height / 2 + 0.305 , 0.15,  0.0, 0.0, 1.0, true},       // Niebieska z paskiem (10)
      {1.0 + width / 2 + 0.545, height / 2 - 0.305 , 0.15,  0.5, 0.0, 0.0, true},       // Fioletowa z paskiem (12)
      {1.0 + width / 2 + 0.820, height / 2 + 0.155 , 0.15,  0.0, 1.0, 0.0, true},       // Czerwona z paskiem (11)
      {1.0 + width / 2 + 0.820, height / 2 - 0.155 , 0.15,  0.5, 0.5, 0.5, false},      // Br¹zowa kula (7)
      {1.0 + width / 2 + 0.820, height / 2 + 0.460 , 0.15,  0.5, 0.5, 0.5, true},       // Br¹zowa z paskiem (15)
      {1.0 + width / 2 + 0.820, height / 2 - 0.460 , 0.15,  0.0, 0.0, 1.0, false},      // Niebieska kula (2)
      {1.0 + width / 2 + 1.095, height / 2 - 0.305 , 0.15,  0.0, 1.0, 1.0, false},      // Pomarañczowa kula (5)
      {1.0 + width / 2 + 1.095, height / 2 + 0.305 , 0.15,  1.0, 0.0, 0.0, true},       // Zielona z paskiem (14)
      {1.0 + width / 2 + 1.095, height / 2 + 0.000 , 0.15,  1.0, 1.0, 0.0, false},      // ¯ó³ta kula (1)
      {1.0 + width / 2 + 1.095, height / 2 - 0.615 , 0.15,  0.0, 0.5, 0.5, true},       // Pomarañczowa z paskiem (13)
      {1.0 + width / 2 + 1.095, height / 2 + 0.615 , 0.15,  1.0, 0.0, 1.0, false},      // Fioletowa kula (4)
      {1.0 + width / 2 + 0.545, height / 2 + 0.000 , 0.15,  0.0, 0.0, 0.0, false}       // Kula 8 (czarna)
  };
  double bounds[4] = { 16.0, width - 16.0, 16.0,  height - 16.0 }; // minX, maxX, minY, maxY
  qDebug("Fizyka bili...");
  // Wektor przechowuj¹cy obiekty MovingSphere
  auto movingSpheres = std::make_shared<std::vector<std::unique_ptr<MovingSphere>>>();
  // Tworzenie kul na podstawie pozycji
  for (const auto& ball : balls) {
      
      double x, y, z, r, g, b;
      bool hasStripe;
      std::tie(x, y, z, r, g, b, hasStripe) = ball;
      // Tworzenie kuli i przypisanie tekstury
      vtkSmartPointer<vtkActor> ballActor = createBall(x, y, z, r, g, b, hasStripe);
      // Tworzenie obiektu ruchomej kuli
      auto movingSphere = std::make_unique<MovingSphere>(ballActor, bounds);
      movingSpheres->push_back(std::move(movingSphere));
      renderer->AddActor(ballActor);
  }
  qDebug("Dodano bile...");
  /////// RUCH 
  // Dodanie callbacku do aktualizacji pozycji
  // Callback do aktualizacji pozycji wszystkich kul



  //vtkNew<vtkCallbackCommand> timerCallback;
  //qDebug("Poruszanie bil...");
  //timerCallback->SetCallback([](vtkObject*, unsigned long, void* clientData, void*) {
  //    auto* spheres = static_cast<std::vector<std::unique_ptr<MovingSphere>>*>(clientData);
  //    for (auto& sphere : *spheres) {
  //        sphere->UpdatePosition(1.0); // Aktualizuj pozycjê ka¿dej kuli
  //    }
  //    });
  //// Przekazanie wskaŸnika do callbacka
  //timerCallback->SetClientData(movingSpheres.get());

  qDebug("Dodawanie obserwatora do interaktora...");
  // Dodanie obserwatora do interaktora
  //interactor->AddObserver(vtkCommand::TimerEvent, timerCallback);

  // Timer (czas kroku w milisekundach)
  //interactor->CreateRepeatingTimer(10);

  qDebug("Renderowanie...");
  // Start renderowania
  //renderer->ResetCamera();
  window->Render();
  interactor->Start();

  
  //interactor->Start();
  qDebug("Startowanie interaktora...");
  mainWindow.show();
  qDebug("Wyswietlenie okna...");
  return app.exec();
}
