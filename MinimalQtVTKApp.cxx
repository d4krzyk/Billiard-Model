// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#define _USE_MATH_DEFINES
#include <QVTKOpenGLNativeWidget.h>
#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QSurfaceFormat>
#include <QTimer>
#include <QKeyEvent>
#include <qthread.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
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
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkRendererCollection.h>
#include <vtkPolygon.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkImageReader2Factory.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkCubeSource.h>
#include <vtkCamera.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkBoxWidget.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkCellPicker.h>
#include <vtkTextActor.h>
#include <vtkCylinderSource.h>
#include <vtkTextProperty.h>
#include <vtkline.h>
#include <algorithm>
#include <tuple>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <random>
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








// Sta³e

// Zmienna globalna dla vtkActor
vtkSmartPointer<vtkActor> TableActor = vtkSmartPointer<vtkActor>::New();
std::vector<vtkSmartPointer<vtkActor>> ballActors;
vtkSmartPointer<vtkTextActor> textActorScore = vtkSmartPointer<vtkTextActor>::New();
vtkSmartPointer<vtkTextActor> GameOverActor = vtkSmartPointer<vtkTextActor>::New();
vtkSmartPointer<vtkTextActor> ShotPowerActor = vtkSmartPointer<vtkTextActor>::New();
int SCORE = 0;
bool GameIsOver = false;
double width = 10.0;  // Szerokoœæ p³aszczyzny
double height = 5.0;   // Wysokoœæ p³aszczyzny
const double minX = width / 15.75;  // Minimalna dopuszczalna pozycja X
const double maxX = width - width / 15.75;  // Maksymalna dopuszczalna pozycja X
const double minY = height / 7.75;  // Minimalna dopuszczalna pozycja Y
const double maxY = height - height / 7.75;  // Maksymalna dopuszczalna pozycja Y
const double resistanceFactor = 0.99;  // Wspó³czynnik oporu (tarcia)
const double restitutionCoefficient = 0.97; // Wspó³czynnik odbicia (0.0 - brak odbicia, 1.0 - idealne odbicie)
// Przyk³adowe wspó³rzêdne œrodków dziurek
  //const double holeRadius = 10.0; // Promieñ dziurki
const double hole1[2] = { width / 15.75, height - height / 7.75 }; // Wspó³rzêdne pierwszej dziurki
const double hole2[2] = { width / 2, height - height / 7.75 }; // Wspó³rzêdne drugiej dziurki
const double hole3[2] = { width - width / 15.75, height - height / 7.75 }; // Trzecia dziurka
const double hole4[2] = { width / 15.75, height / 7.75 }; // Czwórka
const double hole5[2] = { width / 2, height / 7.75 }; // Pi¹tka
const double hole6[2] = { width - width / 15.75, height / 7.75 }; // Szóstka
const double ballRadius = 0.15;
double maxSpeedX = 0.300;  // Maksymalna prêdkoœæ
double maxSpeedY = 0.300;
bool StickShot = false;
double ShotSpeedX = 0.00;
double ShotSpeedY = 0.00;
double velocityAngle = 0.0;
double shotSpeedMagnitude = 0.16;
bool allBallsStopped = true;
bool isWhiteBallOnTable = true;
double gradientLength = 3.0; // D³ugoœæ gradientu
/////// RUCH 
// Funkcja sprawdzaj¹ca, czy kula wpad³a do którejkolwiek z dziurek

auto distanceCheck = [](double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    };

// Funkcja do generowania œcie¿ki do tekstury
char* getTexturePath(double r, double g, double b, bool hasStripe) {
    // Kolory kul (w formacie RGB)
    if (!hasStripe) {
        if (r == 1.0 && g == 1.0 && b == 1.0) return (char*)"textures/white_ball_texture.jpg";  // Bia³a kula
        if (r == 1.0 && g == 0.0 && b == 0.0) return (char*)"textures/red_ball_texture.jpg";    // Czerwona kula
        if (r == 0.0 && g == 1.0 && b == 0.0) return (char*)"textures/green_ball_texture.jpg";  // Zielona kula
        if (r == 0.0 && g == 0.0 && b == 1.0) return (char*)"textures/blue_ball_texture.jpg";   // Niebieska kula
        if (r == 1.0 && g == 1.0 && b == 0.0) return (char*)"textures/yellow_ball_texture.jpg"; // ¯ó³ta kula
        if (r == 1.0 && g == 0.0 && b == 1.0) return (char*)"textures/purple_ball_texture.jpg"; // Fioletowa kula
        if (r == 0.0 && g == 1.0 && b == 1.0) return (char*)"textures/orange_ball_texture.jpg"; // Pomarañczowa kula
        if (r == 0.5 && g == 0.5 && b == 0.5) return (char*)"textures/brown_ball_texture.jpg";  // Br¹zowa kula
    }
    if (hasStripe) {
        // Dla kul z paskiem dodaj prefiks 'st_' przed kolorem
        if (r == 0.5 && g == 0.5 && b == 1.0) return (char*)"textures/st_blue_ball_texture.jpg";   // Niebieska z paskiem
        if (r == 1.0 && g == 0.0 && b == 1.0) return (char*)"textures/st_yellow_ball_texture.jpg"; // ¯ó³ta z paskiem
        if (r == 0.0 && g == 1.0 && b == 0.0) return (char*)"textures/st_red_ball_texture.jpg";    // Czerwona z paskiem
        if (r == 1.0 && g == 0.0 && b == 0.0) return (char*)"textures/st_green_ball_texture.jpg";  // Zielona z paskiem
        if (r == 0.0 && g == 0.0 && b == 1.0) return (char*)"textures/st_blue_ball_texture.jpg";   // Niebieska z paskiem
        if (r == 0.0 && g == 0.5 && b == 0.5) return (char*)"textures/st_orange_ball_texture.jpg"; // Pomarañczowa z paskiem
        if (r == 0.5 && g == 0.0 && b == 0.0) return (char*)"textures/st_purple_ball_texture.jpg"; // Fioletowa z paskiem
        if (r == 0.5 && g == 0.5 && b == 0.5) return (char*)"textures/st_brown_ball_texture.jpg";     // Br¹zowa z paskiem
    }
    return (char*)"textures/black_ball_texture.jpg";  // Kula 8 (czarna)
}

void processShot()
{
    // Obliczanie nowego wektora prêdkoœci z k¹ta
    ShotSpeedX = shotSpeedMagnitude * std::cos(velocityAngle);  // Nowa prêdkoœæ X po obrocie
    ShotSpeedY = shotSpeedMagnitude * std::sin(velocityAngle);  // Nowa prêdkoœæ Y po obrocie
}

void SetUpCamera(vtkSmartPointer<vtkRenderer> renderer, double width, double height, double z)
{
    // Œrodek sto³u
    double tableCenter[3] = { width , height - 5, z };

    // Tworzymy kamerê
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

    // Pozycja kamery - ustawiamy j¹ wy¿ej i dalej od sto³u, aby uzyskaæ lepszy widok
    double cameraPosition[3] = { width , height, z + 3.0 }; // Kamera znajduje siê nad sto³em
    camera->SetPosition(cameraPosition);
    camera->SetFocalPoint(tableCenter);  // Kamera patrzy na œrodek sto³u
    // Ustawienie k¹ta widzenia kamery
    camera->UseHorizontalViewAngleOn();

    camera->SetViewUp(0.0, 0.0, 1.0);
    camera->Azimuth(90);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
}

class SmoothCameraRotation
{
public:
    SmoothCameraRotation(vtkRenderer* renderer, vtkRenderWindowInteractor* interactor, double angle)
        : Renderer(renderer), Interactor(interactor), CurrentStep(0), TotalSteps(5)
    {
        vtkCamera* camera = renderer->GetActiveCamera();
        // Zmienna do przechowywania pozycji kamery
        //double position[3];
        //camera->GetPosition(position);
        //// Wyœwietlenie pozycji kamery za pomoc¹ qDebug
        //    // Wyœwietlenie pozycji kamery za pomoc¹ qDebug
        //QString message = QString("Pozycja kamery: x = %1, y = %2, z = %3")
        //    .arg(position[0]) // Podstawienie wartoœci x
        //    .arg(position[1]) // Podstawienie wartoœci y
        //    .arg(position[2]); // Podstawienie wartoœci z
        //// Wyœwietlenie komunikatu za pomoc¹ qDebug
        //qDebug()<< message;
        //camera->GetFocalPoint(position);
        //message = QString("Pozycja punktu patrzenia kamery: x = %1, y = %2, z = %3")
        //    .arg(position[0]) // Podstawienie wartoœci x
        //    .arg(position[1]) // Podstawienie wartoœci y
        //    .arg(position[2]); // Podstawienie wartoœci z
        //// Wyœwietlenie komunikatu za pomoc¹ qDebug
        //qDebug() << message;
        AngleIncrement = angle / TotalSteps; // 45 stopni w 90 krokach
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

    CameraInteractorStyle() : IsDrag(false), IsSelected(false), trackedActor(nullptr) {}

    virtual void OnChar() override
    {
        const char* key = this->GetInteractor()->GetKeySym();
        //Disable vtk processing of 'e' press
        if (strcmp(key, "w") == 0)
        {
            vtkInteractorStyle::OnKeyPress();
        }
		else if (strcmp(key, "s") == 0)
		{
			vtkInteractorStyle::OnKeyPress();
		}
		else if (strcmp(key, "a") == 0)
		{
			vtkInteractorStyle::OnKeyPress();
		}
		else if (strcmp(key, "d") == 0)
		{
			vtkInteractorStyle::OnKeyPress();
		}
		else if (strcmp(key, "f") == 0)
		{
			vtkInteractorStyle::OnKeyPress();
		}

    }

    void OnKeyDown() override
    {
        // Pobierz wciœniêty klawisz
        std::string key = this->GetInteractor()->GetKeySym();
        vtkRenderWindowInteractor* interactor = this->GetInteractor();
        vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        // Obs³uga prawej strza³ki

        if (key == "Right")
        {
            double angle = 5;
            if (renderer)
            {

                auto cameraAnimation = new SmoothCameraRotation(renderer, interactor, angle);
                
                cameraAnimation->Start();
            }
        }
        if (key == "Left")
        {
            double angle = -5;
            if (renderer)
            {
                auto cameraAnimation = new SmoothCameraRotation(renderer, interactor, angle);
                cameraAnimation->Start();
            }
        }
		if (key == "a")
		{
            velocityAngle += 0.02;
            processShot();
			//qDebug() << "a";
		}
        if (key == "d")
        {
            velocityAngle -= 0.02;
            processShot();
            //qDebug() << "d";
        }
		if (key == "s")
		{
            if (shotSpeedMagnitude > 0.02)
            {
                shotSpeedMagnitude -= 0.0005;
                processShot();
                
                //qDebug() << "s";
            }
			
		}
        if (key == "w")
        {
            if (shotSpeedMagnitude < maxSpeedX)
            {
                shotSpeedMagnitude += 0.0005;
                processShot();
                //qDebug() << "w";
            }
        }
        if (allBallsStopped)
        {
            if (key == "f")
            {
                processShot();
                //qDebug() << "f";
                StickShot = true;
            }
        }
        // Wywo³anie oryginalnej implementacji (opcjonalne)
        //vtkInteractorStyleTrackballCamera::OnKeyDown();
    }
    void OnLeftButtonDown() override
    {
        // Pobierz wspó³rzêdne klikniêcia myszk¹
        
        if (!IsSelected) {
            vtkRenderWindowInteractor* interactor = this->GetInteractor();
            vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
            int* clickPos = interactor->GetEventPosition();
            // Wykonaj picking
            vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
            picker->SetTolerance(0.001);

            if (picker->Pick(clickPos[0], clickPos[1], 0, renderer)) {
                vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();
                if (pickedActor == ballActors[0]) {
                    std::cout << "Kliknieta biala kula" << std::endl;

                    double pos[2];
                    pickedActor->GetPosition(pos);
                    if (pos[2] > 2.00 && !isWhiteBallOnTable && allBallsStopped)
                    {
                        
                        this->trackedActor = pickedActor;
                        this->IsSelected = true;
                        std::cout << "Wybor miejsca dla kuli" << std::endl;
                        this->trackedActor->GetProperty()->SetColor(0.5, 1.0, 0.5);   // Kolor poœwiaty (¿ó³ty)
                        this->trackedActor->GetProperty()->SetOpacity(0.5);          // Przezroczystoœæ
                        this->trackedActor->SetScale(1.25);
                    }
                }
            }
        }
        // Wywo³anie oryginalnej implementacji
        //vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
    // Wykonywana podczas przesuwania myszk¹, ale zablokowana, gdy IsDrag jest false
    void OnMiddleButtonDown() override
    {
        // Zablokowanie przesuwania kamery œrodkowym przyciskiem myszy
        // Nie wywo³uj funkcji bazowej, aby nie przesuwaæ kamery
        // vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
    }
    void OnRightButtonDown() override {
 
        if (IsSelected)
        {
            vtkRenderWindowInteractor* interactor = this->GetInteractor();
            vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();


            // Pobierz bie¿¹c¹ pozycjê kursora
            int* mousePos = interactor->GetEventPosition();
            // Stwórz picker do wykrywania klikniêæ na aktorach
            vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
            picker->SetTolerance(0.001);  // Ustaw tolerancjê pickingu
            if (picker->Pick(mousePos[0], mousePos[1], 0, renderer)) {
                vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

                if (pickedActor == TableActor) {
                    // Pobierz wspó³rzêdne punktu na powierzchni obiektu
                    double pickedPos[3];
                    picker->GetPickPosition(pickedPos);  // Pobierz punkt na powierzchni obiektu

                    // Wydrukuj wynik
                    /*std::cout << "Klikniêto aktor o wspó³rzêdnych X: " << pickedPos[0]
                        << ", Y: " << pickedPos[1]
                        << ", Z: " << pickedPos[2] << std::endl;*/

                    // Ustaw pozycjê trackedActor na wybranej lokalizacji (na powierzchni obiektu)
                    if (pickedPos[0] > minX && pickedPos[0] < maxX && pickedPos[1] > minY && pickedPos[1] < maxY)
                    {
                        this->trackedActor->SetPosition(pickedPos[0], pickedPos[1], 0.15);  // Z ustawiony na 0.15
                        vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
                        camera->SetFocalPoint(pickedPos[0], pickedPos[1], 0.25);
						isWhiteBallOnTable = true;
                        this->trackedActor->GetProperty()->SetColor(1.0, 1.0, 1.0);   // Kolor poœwiaty (¿ó³ty)
                        this->trackedActor->GetProperty()->SetOpacity(1.0);          // Przezroczystoœæ
                        this->trackedActor->SetScale(1.00);
                        this->IsSelected = false;
                        this->trackedActor = nullptr;
                    }
                    
                }
            }
        }
        //vtkInteractorStyleTrackballCamera::OnRightButtonDown();
        //std::cout << "x " << worldPos[0] << "y " <<  worldPos[1] << "z " << worldPos[2] << "idk "<<  worldPos[3] << std::endl; 
    }
    void OnMouseMove() override
    {
        if (IsDrag)
        {
            //vtkInteractorStyleTrackballCamera::OnMouseMove(); // Obrót kamery tylko wtedy, gdy przycisk jest wciœniêty
        }
    }

private:
    vtkSmartPointer<vtkActor> trackedActor;
    bool IsSelected;
    bool IsDrag; // Flaga mówi¹ca, czy myszka jest wciœniêta
};

// Definicja metody New, by poprawnie tworzyæ obiekt
vtkStandardNewMacro(CameraInteractorStyle);

// Funkcja do tworzenia kuli bilarda
vtkSmartPointer<vtkActor> createBall(double x, double y, double z, double r, double g, double b, bool hasStripe) {
    vtkNew<vtkTexturedSphereSource> sphereSource;
    //sphereSource->SetCenter(x, y, z);  // Pozycja kuli
    sphereSource->SetRadius(ballRadius);    // Promieñ kuli
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
    
    if (textureFile->GetOutput()->GetDimensions()[0] == 0) {
        std::cerr << "Error: Failed to load texture from " << texturePath << std::endl;
    }
    else {
        std::cout << "Texture loaded successfully: " << texturePath << std::endl;
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

class Ball {

public:
    vtkSmartPointer<vtkActor> ballActor;
    double ballSpeedX = 0.0;
    double ballSpeedY = 0.0;
    int id;
    bool isHoled = false;

    Ball(vtkSmartPointer<vtkActor> actor, int id, double  ballSpeedX, double  ballSpeedY, double  pushPower)
    {
        this->id = id;
        this->ballActor = actor;
        this->ballSpeedX = ballSpeedX;
        this->ballSpeedY = ballSpeedY;
    }

    void setIsHoled(bool isHoled)
    {
        this->isHoled = isHoled;
    }
    void setSpeed(double speedX, double speedY)
    {
        this->ballSpeedX = speedX;
        this->ballSpeedY = speedY;
    }

    
    void updatePosition(double deltaTime)
    {

        double position[3];
        double normalX, normalY;
        ballActor->GetPosition(position);


        // TRAFIENIE DO DZIUR
        processBallInHole(position, ballSpeedX, ballSpeedY, isHoled, id);

        getNormal(normalX, normalY, position[0], position[1], isHoled);

        calcRotationSpeed(ballActor, ballSpeedX, ballSpeedY);

        // Zastosowanie oporu do prêdkoœci (spowalnia z czasem)
        ballSpeedX *= resistanceFactor;  // Zmniejsz prêdkoœæ w osi X
        ballSpeedY *= resistanceFactor;  // Zmniejsz prêdkoœæ w osi Y

        if (normalX != 0.0 || normalY != 0.0) {
            reflectBall(ballSpeedX, ballSpeedY, normalX, normalY, isHoled);
        }

        //double speed = std::sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);

        // Zaktualizowanie pozycji na podstawie prêdkoœci
        position[0] = position[0] + ballSpeedX ;
        position[1] = position[1] + ballSpeedY;
        

        // Zatrzymywanie siê kuli po osi¹gniêciu niskiej prêdkoœci (minimalna prêdkoœæ)
        if (fabs(ballSpeedX) < 0.0005 && fabs(ballSpeedY) < 0.0005) {
            ballSpeedX = 0.0;  // Zatrzymanie prêdkoœci w osi X
            ballSpeedY = 0.0;  // Zatrzymanie prêdkoœci w osi Y
            //camera->SetFocalPoint(ballActors[0]->GetPosition());
        }
        ballActor->SetPosition(position);
        
        // Aktualizacja poprzednich pozycji dla interpolacji w nastêpnej klatce
        
    }


    int isInHole(double ballX, double ballY) {

        double holeRadius = 0.1;
        // Sprawdzamy odleg³oœæ od ka¿dego œrodka dziurki
        if (distanceCheck(ballX, ballY, hole1[0], hole1[1]) <= holeRadius) {
            return 1;
        }
        if (distanceCheck(ballX, ballY, hole2[0], hole2[1]) <= holeRadius) {
            return 2;
        }
        if (distanceCheck(ballX, ballY, hole3[0], hole3[1]) <= holeRadius) {
            return 3;
        }
        if (distanceCheck(ballX, ballY, hole4[0], hole4[1]) <= holeRadius) {
            return 4;
        }
        if (distanceCheck(ballX, ballY, hole5[0], hole5[1]) <= holeRadius) {
            return 5;
        }
        if (distanceCheck(ballX, ballY, hole6[0], hole6[1]) <= holeRadius) {
            return 6;
        }

        return 0;
    };
    void processBallInHole(double position[], double& ballSpeedX, double& ballSpeedY, bool& isHoled, int id) {
        if (isInHole(position[0], position[1]) != 0) {
            if (id == 0){                
                isWhiteBallOnTable = false;

            }
            isHoled = true;
            int holeNumber = isInHole(position[0], position[1]);
            double temp_pos[2];
            double addOffsetX = 0;
            double addOffsetY = 0;
            switch (holeNumber) {
            case 1: temp_pos[0] = hole1[0]; temp_pos[1] = hole1[1]; addOffsetX = -0.05; addOffsetY = 0.05; break;
            case 2: temp_pos[0] = hole2[0]; temp_pos[1] = hole2[1]; addOffsetX = 0; addOffsetY = 0.05; break;
            case 3: temp_pos[0] = hole3[0]; temp_pos[1] = hole3[1]; addOffsetX = 0.05; addOffsetY = 0.05; break;
            case 4: temp_pos[0] = hole4[0]; temp_pos[1] = hole4[1]; addOffsetX = -0.05; addOffsetY = -0.05; break;
            case 5: temp_pos[0] = hole5[0]; temp_pos[1] = hole5[1]; addOffsetX = 0; addOffsetY = -0.1; break;
            case 6: temp_pos[0] = hole6[0]; temp_pos[1] = hole6[1]; addOffsetX = 0.05; addOffsetY = -0.05; break;
            }

            ballSpeedY = 0;
            ballSpeedX = 0;
            if (position[2] > -0.35)
            {
                position[0] = temp_pos[0] + addOffsetX;
                position[1] = temp_pos[1] + addOffsetY;
                position[2] -= 0.005;
                //qDebug() << "Hole X:" << position[0] << " Y:" << position[1];
                //qDebug() << "Z:" << position[2];
            }
            else {
                if (id == 0){
                    position[0] = width / 2; position[1] = height / 2; position[2] = 2.8;
                }
                else{

                    if (id == 15)
                    {
                        GameOverActor->SetVisibility(1);
						GameIsOver = true;
                        position[0] = 0.3 + id * 0.35; position[1] = -0.35; position[2] = -0.5;
                        return;
                    }
                    SCORE += 1;
                    std::string scoreText = "SCORE: " + std::to_string(SCORE);
                    textActorScore->SetInput(scoreText.c_str());
                    position[0] = 0.3 + id * 0.35; position[1] = -0.35; position[2] = -0.5;
                }
                
                isHoled = false;
            }
        }
    }

    void calcRotationSpeed(vtkActor* ball, double ballSpeedX, double ballSpeedY)
    {
        // Obliczanie rotacji
        double speedVector = std::sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);
        double angularSpeed = speedVector / 0.15;

        // Oœ obrotu
        double rotationAxis[3] = { -ballSpeedY, ballSpeedX, 0.0 };
        double axisLength = std::sqrt(rotationAxis[0] * rotationAxis[0] + rotationAxis[1] * rotationAxis[1]);
        rotationAxis[0] /= axisLength;
        rotationAxis[1] /= axisLength;

        // K¹t rotacji w tej klatce
        double angle = angularSpeed * 16.0 / 15.0; // Konwersja z ms do sekund

        // Aktualizacja transformacji
        ball->RotateWXYZ(angle * 180.0 / M_PI, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
    }


    // Funkcja do obliczenia normalnej powierzchni (zak³adaj¹c odbicie od œcian)
    void getNormal(double& normalX, double& normalY, double& posX, double& posY, bool& isHoled) {


        // Zresetowanie normalnych
        normalX = 0.0;
        normalY = 0.0;
        // Sprawdzenie odbicia od pionowych œcian
        if (posX <= minX) {
            normalX = 1.0;
            //qDebug() << "\n" << " X: " << posX;
        }
        else if (posX >= maxX) {
            normalX = -1.0;
            //qDebug() << "\n" << " X: " << posX;
        }

        // Sprawdzenie odbicia od poziomych œcian
        if (posY <= minY) {
            normalY = 1.0;
            //qDebug() << "\n" << " minY: " << minY << " Y: " << posY;
        }
        else if (posY >= maxY) {
            normalY = -1.0;
            //qDebug() << "\n" << " maxY: " << maxY << " Y: " << posY;
        }

        // Dodanie tolerancji dla unikniêcia oscylacji
        const double epsilon = 0.01;
        if (!isHoled) {
            if (std::abs(posX - minX) < epsilon || std::abs(posX - maxX) < epsilon) {
                posX += (normalX * epsilon);  // Przesuniêcie poza granicê w odpowiednim kierunku
                //qDebug() << "\n" << " outX: " << posX;
            }

            if (std::abs(posY - minY) < epsilon || std::abs(posY - maxY) < epsilon) {
                posY += (normalY * epsilon);  // Przesuniêcie poza granicê w odpowiednim kierunku
                //qDebug() << "\n" << " outY: " << posY;
            }
        }

    }
    // Funkcja do obliczenia k¹ta odbicia kuli
    void reflectBall(double& ballSpeedX, double& ballSpeedY, double normalX, double normalY, bool& isHoled) {
        // Obliczanie k¹ta odbicia
        double dotProduct = ballSpeedX * normalX + ballSpeedY * normalY;
        double normalMagnitudeSquared = normalX * normalX + normalY * normalY;

        // Obliczenie skalarnej projekcji prêdkoœci na normaln¹ powierzchni
        double projection = 2.0 * dotProduct / normalMagnitudeSquared;

        // Prêdkoœæ odbicia (kierunek zmienia siê o podwójny rzut na normaln¹)
        if (!isHoled)
        {
            ballSpeedX -= projection * normalX;
            ballSpeedY -= projection * normalY;

            ballSpeedX *= restitutionCoefficient;
            ballSpeedY *= restitutionCoefficient;
        }

    }


    // Funkcja do detekcji kolizji z innymi kulami
    void handleCollision(Ball& otherBall) {
        double pos1[3], pos2[3];
        this->ballActor->GetPosition(pos1);
        otherBall.ballActor->GetPosition(pos2);
        // Obliczenie odleg³oœci miêdzy œrodkami kul
        double dx = pos2[0] - pos1[0];
        double dy = pos2[1] - pos1[1];
        double distance = std::sqrt(dx * dx + dy * dy);
		double additionalMargin = 0.0155;
        double radiusSum = ballRadius + ballRadius + additionalMargin;

        // Je¿eli kule siê przecinaj¹
        if (distance <= radiusSum && distance > 1e-5) {
            // Minimum Translation Distance (MTD) - wektor, który przesunie kule
            double mtdX = dx * ((radiusSum - distance) / distance);
            double mtdY = dy * ((radiusSum - distance) / distance);

            //// Obliczanie prêdkoœci wzglêdnej
            double vx = this->ballSpeedX - otherBall.ballSpeedX;
            double vy = this->ballSpeedY - otherBall.ballSpeedY;

            //// Normalizujemy wektor MTD
            double mtdLength = std::sqrt(mtdX * mtdX + mtdY * mtdY);
            if (mtdLength > 0.0) {
                double mtdNormalizedX = mtdX / mtdLength;
                double mtdNormalizedY = mtdY / mtdLength;

                //// Obliczanie prêdkoœci w kierunku normalnym (dot product)
                double vn = vx * mtdNormalizedX + vy * mtdNormalizedY;

                //// Jeœli kule siê oddalaj¹, nie ma kolizji
                if (vn < 0.0) return;

                //// Wspó³czynnik restitucji (1 = idealna kolizja sprê¿ysta)
                const double restitution = 0.99;

                //// Impuls kolizji
                double i = -(1.0 + restitution) * vn / 2;
                double impulseX = mtdNormalizedX * i;
                double impulseY = mtdNormalizedY * i;

                // Zmiana pêdu
                this->ballSpeedX += impulseX;
                this->ballSpeedY += impulseY;
                otherBall.ballSpeedX -= impulseX;
                otherBall.ballSpeedY -= impulseY;

            }
        }
    }
};

std::vector<Ball*> balls;

class Trajectory {
private:
    vtkSmartPointer<vtkPoints> points;
    vtkSmartPointer<vtkCellArray> lines;
    vtkSmartPointer<vtkPolyData> trajectory;
    vtkSmartPointer<vtkFloatArray> scalars;
    int numPoints;
    double length;
public:
    Trajectory() {
        // Inicjalizacja trajektorii
        points = vtkSmartPointer<vtkPoints>::New();
        lines = vtkSmartPointer<vtkCellArray>::New();
        trajectory = vtkSmartPointer<vtkPolyData>::New();
        scalars = vtkSmartPointer<vtkFloatArray>::New();
        length = 0.05; // D³ugoœæ linii
        numPoints = 50; // Liczba punktów
        for (int i = 0; i <= numPoints; ++i) {
            double t = static_cast<double>(i) / numPoints; // Normalizowany wspó³czynnik (0.0 - 1.0)
            double distance = t * length;
            // Skalar oparty na gradientLength
            double scalar = std::max(0.0, 1.0 - distance / gradientLength);
            points->InsertNextPoint(distance, 0.0, 0.0);
            scalars->InsertNextValue(scalar);
            //scalars->InsertNextValue(1.0 - t); // Zanikanie od 1.0 do 0.0
        }
        trajectory->SetPoints(points);
        trajectory->SetLines(lines);
        trajectory->GetPointData()->SetScalars(scalars);
    }
    std::tuple<bool, double, double> getIntersectionPoint(double startX, double startY, double directionX, double directionY,
        double ballX, double ballY) {
        // Wektor trajektorii
        double dx = directionX;
        double dy = directionY;

        // Normalizacja wektora kierunku (zapewnia poprawne obliczenia)
        double magnitude = std::sqrt(dx * dx + dy * dy);
        if (magnitude < 1e-8) {
            return { false, 0.0, 0.0 }; // Niepoprawna trajektoria
        }
        dx /= magnitude;
        dy /= magnitude;

        // Wektor od punktu pocz¹tkowego trajektorii do œrodka kuli
        double fx = startX - ballX;
        double fy = startY - ballY;

        // Suma promieni obu kul
        double combinedRadius = ballRadius + ballRadius;

        // Wspó³czynniki równania kwadratowego
        double a = dx * dx + dy * dy;
        double b = 2 * (fx * dx + fy * dy);
        double c = fx * fx + fy * fy - combinedRadius * combinedRadius;

        // Obliczanie delty
        double delta = b * b - 4 * a * c;

        if (delta < 0) {
            return { false, 0.0, 0.0 };
        }
        
        // Obliczanie pierwiastków równania kwadratowego
        double sqrtDelta = std::sqrt(delta);
        double t1 = (-b - sqrtDelta) / (2 * a);  // Wczeœniejszy punkt przeciêcia
        double t2 = (-b + sqrtDelta) / (2 * a);  // PóŸniejszy punkt przeciêcia

        // Wybieramy najmniejszy dodatni t (najbli¿szy punkt na trajektorii)
        double t = t1 >= 0 ? t1 : t2;  // Wybierz najmniejszy dodatni t
        if (t < 1e-4) {
            return { false, 0.0, 0.0 };
        }
        if (t1 < 0 && t2 < 0) {
            //std::cout << "Brak kolizji: oba t ujemne" << std::endl;
        }
        // Obliczanie wspó³rzêdnych punktu przeciêcia
        double intersectionX = startX + t * dx;
        double intersectionY = startY + t * dy;

        // Sprawdzanie, czy punkt przeciêcia le¿y rzeczywiœcie w obrêbie kuli
        double distSquared = (intersectionX - ballX) * (intersectionX - ballX) +
            (intersectionY - ballY) * (intersectionY - ballY);

        if (distSquared > 2*(ballRadius + 0.0155) * 2 * (ballRadius + 0.0155)) {
            return { false, 0.0, 0.0 }; // Punkt le¿y poza kul¹
        }

        return { true, intersectionX, intersectionY };
    }

    void reflectTrajectoryFromBall(double& velocityX, double& velocityY, double ballX, double ballY, double startX, double startY) {
        // Wektor normalny od œrodka kuli do punktu pocz¹tkowego trajektorii
            double normalX = ballX - startX;
            double normalY = ballY - startY;

            // Normalizowanie wektora normalnego
            double magnitude = std::sqrt(normalX * normalX + normalY * normalY);
            if (magnitude != 0.0) {
                normalX /= magnitude;
                normalY /= magnitude;
            }
            if (magnitude <= 2 * ballRadius) {
                // Obliczanie k¹ta miêdzy wektorem trajektorii a normaln¹
                double dotProduct = velocityX * normalX + velocityY * normalY;
                double normalMagnitude = std::sqrt(normalX * normalX + normalY * normalY);
                double velocityMagnitude = std::sqrt(velocityX * velocityX + velocityY * velocityY);
                double cosTheta = dotProduct / (normalMagnitude * velocityMagnitude);
                double theta = std::acos(cosTheta);

                // Obracamy wektor prêdkoœci o k¹t 2 * theta
                double newVelocityX = velocityX * std::cos(2 * theta) - velocityY * std::sin(2 * theta);
                double newVelocityY = velocityX * std::sin(2 * theta) + velocityY * std::cos(2 * theta);

                // Zaktualizowanie prêdkoœci
                velocityX = newVelocityX;
                velocityY = newVelocityY;

                // Mo¿esz dodaæ t³umienie
                double restitution = restitutionCoefficient;
                velocityX *= restitution;
                velocityY *= restitution;

                // Sprawdzanie poprawnoœci kierunku odbicia
                double incomingAngle = std::atan2(velocityY, velocityX);
                double normalAngle = std::atan2(normalY, normalX);
                double outgoingAngle = std::atan2(-velocityY, -velocityX);

                if (std::abs(incomingAngle - normalAngle) > M_PI / 2) {
                    // Korygowanie kierunku odbicia
                    velocityX = -velocityX;
                    velocityY = -velocityY;
                }
            }

        
        
    }
    // Aktualizacja trajektorii w czasie rzeczywistym
    void updateTrajectory(double startX, double startY, double forceX, double forceY) {
        calculateTrajectory(startX, startY, forceX, forceY, 4, balls);
		processShot();
        this->trajectory->Modified();
    }
    // Obliczanie trajektorii
    void calculateTrajectory(double startX, double startY, double directionX, double directionY,
        int maxBounces, std::vector<Ball*> balls) {
        this->points->Reset();
        this->lines->Reset();

        double currentX = startX;
        double currentY = startY;
        double velocityX = directionX;
        double velocityY = directionY;
		bool Colliding = false;
        this->points->InsertNextPoint(currentX, currentY, 0.15); // Pocz¹tkowy punkt

        for (int i = 0; i < maxBounces; ++i) {
            // Sprawdzanie kolizji z ka¿d¹ kul¹
            for (Ball* ball : balls) {
                int id = ball->id;
                if (id == 0) continue;
                double position[3];
                ball->ballActor->GetPosition(position);
                bool collision;
                double intersectionX, intersectionY;
				std::tie(collision, intersectionX, intersectionY) = getIntersectionPoint(currentX, currentY, velocityX, velocityY, position[0], position[1]);
                if (collision) {
                    currentX = intersectionX;
                    currentY = intersectionY;

                    this->points->InsertNextPoint(currentX, currentY, 0.15);
                    reflectTrajectoryFromBall(velocityX, velocityY, currentX, currentY, position[0], position[1]);

                    currentX += velocityX;
                    currentY += velocityY;
                    this->points->InsertNextPoint(currentX, currentY, 0.15);
					Colliding = true;
                    break;
                }
            }
			
            // Obliczanie czasu do kolizji z ka¿d¹ œcian¹
            double timeToVerticalWall = (velocityX > 0) ? (maxX - currentX) / velocityX
                : (velocityX < 0) ? (minX - currentX) / velocityX
                : std::numeric_limits<double>::max();
            double timeToHorizontalWall = (velocityY > 0) ? (maxY - currentY) / velocityY
                : (velocityY < 0) ? (minY - currentY) / velocityY
                : std::numeric_limits<double>::max();

            // Wybierz najbli¿sz¹ kolizjê
            double timeToCollision = std::min(timeToVerticalWall, timeToHorizontalWall);
            // Aktualizacja pozycji
            currentX += velocityX * timeToCollision;
            currentY += velocityY * timeToCollision;

            if (currentX < minX) {
                currentX = minX;
                velocityX = -velocityX;  // Odbicie od lewej œciany
            }
            else if (currentX > maxX) {
                currentX = maxX;
                velocityX = -velocityX;  // Odbicie od prawej œciany
            }

            if (currentY < minY) {
                currentY = minY;
                velocityY = -velocityY;  // Odbicie od dolnej œciany
            }
            else if (currentY > maxY) {
                currentY = maxY;
                velocityY = -velocityY;  // Odbicie od górnej œciany
            }

            // Dodanie punktu kolizji
            this->points->InsertNextPoint(currentX, currentY, 0.15);

            // Odbicie
            if (timeToCollision == timeToVerticalWall) {
                velocityX = -velocityX; // Odbicie od pionowej œciany
            }
            else {
                velocityY = -velocityY; // Odbicie od poziomej œciany
            }
            if (Colliding)
            {
                break;
            }
        }

        // Tworzenie linii miêdzy punktami
        for (vtkIdType i = 0; i < this->points->GetNumberOfPoints() - 1; ++i) {
            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, i);
            line->GetPointIds()->SetId(1, i + 1);
            this->lines->InsertNextCell(line);
        }
        vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();
        this->lines->GetNextCell(pointIds); // Pobierz liniê (mo¿e byæ u¿yte w pêtli)

        for (vtkIdType i = 0; i < pointIds->GetNumberOfIds(); ++i) {
            vtkIdType pointId = pointIds->GetId(i);
            double point[3];
            this->points->GetPoint(pointId, point);
            qDebug() << "Linia zawiera punkt: (" << point[0] << ", " << point[1] << ", " << point[2] << ")";
        }
    }


    void updateGradient(double newGradientLength) {
        gradientLength = newGradientLength;

        this->scalars->Reset(); // Wyczyœæ istniej¹ce wartoœci
        for (int i = 0; i <= numPoints; ++i) {
            double t = static_cast<double>(i) / numPoints;
            double distance = t * length;
            double scalar = std::max(0.0, 1.0 - distance / gradientLength);
            scalars->InsertNextValue(scalar);
        }

        trajectory->GetPointData()->SetScalars(scalars);
        trajectory->Modified(); // Zg³oœ modyfikacjê danych
    }

    vtkSmartPointer<vtkPolyData> getTrajectory() const {
        return trajectory;
    }
};

// Funkcja do ustawiania pozycji i orientacji kija wzglêdem kuli
void UpdateCueSetPosition(vtkActor* cueStickActor, double ballX, double ballY, double ballZ, double directionX, double directionY, double directionZ) {
    // Ustawienie odleg³oœci kija od kuli
    double stickLength = 4.5; // D³ugoœæ kija
    double offset = 1.00;      // Odleg³oœæ od œrodka kuli do koñca kija

    // Pozycja startowa kija (na koñcu skierowanym do kuli)
    double startX = ballX - directionX ;
    double startY = ballY - directionY ;
    double startZ = ballZ - directionZ ;

    // Pozycja koñca kija
    double endX = ballX - directionX ;
    double endY = ballY - directionY ;
    double endZ = ballZ - directionZ ;

    // Obliczenie œrodka kija
    double midX = (startX + endX) / 2.0 ;
    double midY = (startY + endY) / 2.0 ;
    double midZ = (startZ + endZ) / 2.0 ;

    // Obliczenie orientacji kija
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(midX, midY, midZ );

    // Obliczanie k¹ta obrotu kija
    double velocityAngle = std::atan2(ShotSpeedY, ShotSpeedX) * 180.0 / M_PI;  // K¹t w stopniach
    // Ustawienie orientacji kija wzglêdem prêdkoœci kuli
    cueStickActor->SetOrientation(15.0, 0.0, velocityAngle + 90);  // K¹t obraca siê wokó³ osi Z
    // Przypisz transformacjê do aktora kija
    cueStickActor->SetUserTransform(transform);
}

int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
  QApplication app(argc, argv);

  QMainWindow mainWindow;
  mainWindow.setWindowTitle("Bilard Game");
  mainWindow.resize(1150, 800);

  qDebug("Wyswietlenie okna...");
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
  // Utwórz tekst
  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  textActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Normalizowane wspó³rzêdne ekranu
  textActor->SetTextScaleModeToNone();
  textActor->SetInput("BILARD GAME");
  textActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  renderer->AddActor(textActor);
  // Utwórz tekst

  vtkSmartPointer<vtkTextActor> InstructionActor = vtkSmartPointer<vtkTextActor>::New();
  InstructionActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Normalizowane wspó³rzêdne ekranu
  InstructionActor->SetTextScaleModeToNone();
  InstructionActor->GetTextProperty()->SetFontSize(12);
  InstructionActor->SetPosition(0.01, 0.8);
  // Tekst z instrukcjami do wyœwietlenia
  std::string instructions =
      "Instruction:\n\n"
      "- Left Arrow: Rotate camera left\n"
      "- Right Arrow: Rotate camera right\n"
      "- W: Increase power\n"
      "- S: Decrease power\n"
      "- A: Change shot angle left\n"
      "- D: Change shot angle right\n"
      "- F: Shoot \n\n"
      "- Left Mouse Button: Select white ball on fall\n"
      "- Right Mouse Button: Select white ball position on fall\n"
      "- Mouse Scroll: Zoom camera";

  // Ustawienie tekstu
  InstructionActor->SetInput(instructions.c_str());
  // Ustawienie koloru tekstu na jasny (np. bia³y)
  InstructionActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0); // Bia³y kolor

  // Efekt œwiecenia poprzez ustawienie koloru emisji
  InstructionActor->GetTextProperty()->SetBold(1);
  InstructionActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  renderer->AddActor(InstructionActor);



  textActorScore->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Normalizowane wspó³rzêdne ekranu
  textActorScore->SetTextScaleModeToNone();
  textActorScore->SetInput("SCORE: 0");
  textActorScore->SetPosition(0.8,0.01);
  textActorScore->GetTextProperty()->SetFontSize(24);
  // Ustawienie pozycji w prawym górnym rogu
  //textActorScore->SetDisplayPosition(0.8 * renderer->GetSize()[0], 0.9 * renderer->GetSize()[1]); // Pozycja na ekranie, gdzie 90% szerokoœci i wysokoœci ekranu
  textActorScore->SetWidth(1.0);
  textActorScore->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Ustawienie na normalizowane wspó³rzêdne

  // Dodaj tekst do renderer
  renderer->AddActor2D(textActorScore);
  renderer->AddActor2D(GameOverActor);
  GameOverActor->SetInput("GAME OVER");
  GameOverActor->SetPosition(0.3, 0.5);
  GameOverActor->GetTextProperty()->SetFontSize(76);
  GameOverActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Ustawienie na normalizowane wspó³rzêdne
  GameOverActor->SetVisibility(0);
  GameOverActor->GetTextProperty()->SetBold(1);
  renderer->AddActor2D(ShotPowerActor);
  int powerTextValue = shotSpeedMagnitude * 100;
  powerTextValue = static_cast<int>(((powerTextValue - 1) / static_cast<double>(30 - 1)) * (100 - 1) + 1);
  std::string PowerText = "Power: " + std::to_string(powerTextValue) +"%";
  ShotPowerActor->SetInput(PowerText.c_str());
  ShotPowerActor->SetPosition(0.8, 0.95);
  ShotPowerActor->GetTextProperty()->SetFontSize(26);
  ShotPowerActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Ustawienie na normalizowane wspó³rzêdne

  ////////////////////////////
  
  // Tworzenie PolyData
  vtkNew<vtkPolyData> polyData;
  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> quads;
  // Parametry siatki
  int pointsX = 400; // Liczba punktów w osi X
  int pointsY = 200;  // Liczba punktów w osi Y

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
          if (distanceCheck(i, j, distFromUpTab, distFromUpTab) < radius) {
              z = -0.5; // Obni¿enie punktu w okolicy dziurki
          }
          if (distanceCheck(i, j, distFromUpTab, pointsY - distFromUpTab) < radius) {
              z = -0.5;
          }
          if (distanceCheck(i, j, pointsX - distFromUpTab, distFromUpTab) < radius) {
              z = -0.5;
          }
          if (distanceCheck(i, j, pointsX - distFromUpTab, pointsY - distFromUpTab) < radius) {
              z = -0.5; 
          }
          if (distanceCheck(i, j, pointsX / 2, distFromUpTab - 2) < radius) {
              z = -0.5;
          }
          if (distanceCheck(i, j, pointsX / 2, pointsY - distFromUpTab + 2) < radius) {
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

  mapper->SetInputData(polyData);
  TableActor->GetProperty()->SetRepresentationToPoints(); // Ustawienie na punkty
  TableActor->GetProperty()->SetPointSize(2); // Rozmiar punktów
  //TableActor->GetProperty()->SetColor(0.2, 0.8, 0.4); // Kolor siatki
  TableActor->SetMapper(mapper);
  TableActor->SetTexture(texture);
  TableActor->GetProperty()->SetRepresentationToSurface();
  //TableActor->GetProperty()->SetRepresentationToWireframe();
  TableActor->GetProperty()->SetEdgeVisibility(false);

  
  renderer->AddActor(TableActor);
  renderer->AddLight(light);          // Dodanie œwiat³a do renderera
  renderer->AddLight(light2);
  renderer->AddLight(light3);
  renderer->AddLight(light4);
  qDebug("Ustawianie kamery...");
  // Ustawienie kamery
  //SetUpCamera(renderer, width, height, 0.5);

 

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
  std::vector<std::tuple<double, double, double, double, double, double, bool>> ballsTemp = {
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

  SetUpCamera(renderer, std::get<0>(ballsTemp[0]), std::get<1>(ballsTemp[0]), std::get<2>(ballsTemp[0]));
  vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
  camera->SetFocalPoint(std::get<0>(ballsTemp[0]), std::get<1>(ballsTemp[0]), 0.25);
  //SetUpCamera(renderer, 2, 2, 0);
  double position[3];
  renderer->GetActiveCamera()->GetPosition(position);

  // Create five points.
  double origin[3] = { std::get<0>(ballsTemp[0]), std::get<1>(ballsTemp[0]), std::get<2>(ballsTemp[0]) };
  double p0[3] = { 1.0, 0.0, 0.20 };
  double p1[3] = { 0.0, 1.0, 0.20 };
  double p2[3] = { 0.0, 1.0, 0.20 };
  double p3[3] = { 1.0, 2.0, 0.20 };


  
  // Tworzenie danych trajektorii
  Trajectory trajectory = Trajectory();

  vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfTableValues(256);
  lut->SetRange(0.0, 1.0); // Zakres wartoœci skalarnych
  for (int i = 0; i < 256; ++i) {
      double t = static_cast<double>(i) / 255.0;
      lut->SetTableValue(i, t, 1.0 - t, 1.0 - t, t); // Kolor czerwony z zanikanie przezroczystoœci
  }
  lut->Build();
  // Mapper i aktor trajektorii
  vtkSmartPointer<vtkPolyDataMapper> trajectoryMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  trajectoryMapper->SetInputData(trajectory.getTrajectory());
  trajectoryMapper->SetLookupTable(lut);
  trajectoryMapper->SetScalarRange(0.0, 1.0); // Zakres wartoœci skalarnych
  trajectoryMapper->UseLookupTableScalarRangeOn();


  vtkSmartPointer<vtkActor> trajectoryActor = vtkSmartPointer<vtkActor>::New();
  trajectoryActor->SetMapper(trajectoryMapper);
  trajectoryActor->GetProperty()->SetColor(0.65, 0.65, 0.65); // linia
  trajectoryActor->GetProperty()->SetLineWidth(3.5);
  // Ustawienia wygl¹du
  trajectoryActor->GetProperty()->SetOpacity(0.5);              // Przezroczystoœæ
  trajectoryActor->GetProperty()->SetAmbient(0.8);              // Œwiat³o otoczenia
  trajectoryActor->GetProperty()->SetSpecular(1.0);             // Lœnienie
  trajectoryActor->GetProperty()->SetSpecularPower(50);         // Intensywnoœæ lœnienia

  // Dodanie aktora do sceny
  renderer->AddActor(trajectoryActor);

  // Tworzenie kija bilardowego
  vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
  cylinder->SetRadius(0.025);  // Promieñ kija
  cylinder->SetHeight(4.5);  // D³ugoœæ kija
  cylinder->SetResolution(50);
  cylinder->Update();
  cylinder->SetCenter(0.0, 3.0, 0.0);
  vtkSmartPointer<vtkPolyDataMapper> cueStickMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  cueStickMapper->SetInputConnection(cylinder->GetOutputPort());

  vtkSmartPointer<vtkActor> cueStickActor = vtkSmartPointer<vtkActor>::New();
  cueStickActor->SetMapper(cueStickMapper);
  vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
  cueStickActor->GetProperty()->SetColor(colors->GetColor3d("SaddleBrown").GetData());
  // Dodanie aktorów do sceny
  renderer->AddActor(cueStickActor);

  // Wyœwietlenie pozycji kamery za pomoc¹ qDebug
      // Wyœwietlenie pozycji kamery za pomoc¹ qDebug
  QString message = QString("Pozycja kamery: x = %1, y = %2, z = %3")
      .arg(position[0]) // Podstawienie wartoœci x
      .arg(position[1]) // Podstawienie wartoœci y
      .arg(position[2]); // Podstawienie wartoœci z
  // Wyœwietlenie komunikatu za pomoc¹ qDebug
  qDebug() << message;

  // Ustawienie nowego stylu interakcji dla renderera
  vtkSmartPointer<CameraInteractorStyle> style = vtkSmartPointer<CameraInteractorStyle>::New();
  vtkSmartPointer<vtkRenderWindowInteractor> interactor = window->GetInteractor();
  interactor->SetInteractorStyle(style);

  qDebug("Startowanie interaktora...");
  interactor->EnableRenderOn();
  interactor->Start();

  qDebug("Fizyka bili...");
  // Tworzenie obiektów Ball

  int id_iter = 0;

  for (const auto& ball_iter : ballsTemp) {
      
      double x, y, z, r, g, b;
      bool hasStripe;
      std::tie(x, y, z, r, g, b, hasStripe) = ball_iter;
      vtkSmartPointer<vtkActor> ballActor = createBall(x, y, z, r, g, b, hasStripe);
      ballActors.push_back(ballActor);
      renderer->AddActor(ballActor);
      Ball* ball = new Ball(ballActor, id_iter, 0.0,0.0,0.0);
      balls.push_back(ball);
	  id_iter++;
  }
  qDebug("Dodano bile...");
  qDebug("Renderowanie...");
  // Start renderowania
  window->Render();
  mainWindow.show();
  QTimer timer; 
  QElapsedTimer elapsedTimer;
  double delta = 0;

  double ballSpeedX = 0.000;
  double ballSpeedY = 0.000;
  bool isHoled = false;

  double white_pos[3];
  balls[0]->ballActor->GetPosition(white_pos);
  

  double limitedSpeedX = std::max(0.0, std::min(ShotSpeedX, maxSpeedX));
  double limitedSpeedY = std::max(0.0, std::min(ShotSpeedY, maxSpeedY));
  double cylinderCenterY = 3.2;
  double changeCenterY = cylinderCenterY;
  int frameEndCounter = 0; // Licznik klatek
  size_t liftCounter = 0; // Indeks aktualnie unoszonej kuli
  qDebug("Game start...");
  QObject::connect(&timer, &QTimer::timeout, [&]() {
      // Pobierz bie¿¹c¹ pozycjê kuli
      delta = elapsedTimer.elapsed() / 1000.f;
      static bool increasing = true;
      static double opacity = 0.5;
      if (!isWhiteBallOnTable && !GameIsOver)
      {
		  camera->SetFocalPoint(balls[0]->ballActor->GetPosition()[0], balls[0]->ballActor->GetPosition()[1], 0.25);
          trajectoryActor->SetVisibility(0);
      }
      if (increasing) {
          opacity += 0.02;
          if (opacity >= 1.0) increasing = false;
      }
      else {
          opacity -= 0.02;
          if (opacity <= 0.5) increasing = true;
      }

      trajectoryActor->GetProperty()->SetOpacity(opacity);
      allBallsStopped = true;
      for (Ball* ball : balls) {
          ball->updatePosition(delta); // Aktualizacja ka¿dej kuli z kamer¹
      }
      for (size_t i = 0; i < balls.size(); i++) {
          if (!(balls[i]->ballSpeedX == 0.00 && balls[i]->ballSpeedY == 0.00))
          {
              allBallsStopped = false;
          }
          for (size_t j = i + 1; j < balls.size(); j++) {

              if (i != j)
              {
                  balls[i]->handleCollision(*balls[j]);
              }
          }
          if (GameIsOver)
          {
              camera->SetFocalPoint(width/2, height/2, 0.25);
              frameEndCounter++;
              if (frameEndCounter % 60 == 0) // Co pi¹ta klatka
              {
                  interactor->SetKeySym("Right");
                  interactor->InvokeEvent(vtkCommand::KeyPressEvent);
              }

              if (frameEndCounter >= 10000) // Opcjonalny reset
              {
                  frameEndCounter = 0;
              }
              if (i == liftCounter) { // Tylko aktualnie unoszona kula
                  double pos_heaven[3];
                  ballActors[liftCounter]->GetPosition(pos_heaven);

                  if (pos_heaven[2] < 5) { // Jeœli kula nie osi¹gnê³a koñcowej pozycji
                      pos_heaven[2] += 0.025; // Stopniowe unoszenie
                      ballActors[liftCounter]->SetPosition(pos_heaven);
					  ballActors[liftCounter]->RotateY(2); 
                      ballActors[liftCounter]->RotateZ(2); 
                  }
                  else {
                      // Przechodzimy do kolejnej kuli
                      liftCounter++;
                      if (liftCounter >= balls.size()) {
                          liftCounter = 0; // Reset po osi¹gniêciu ostatniej kuli
                      }
                  }
              }
			  
          }
      }
      if (allBallsStopped && isWhiteBallOnTable && !GameIsOver) {
          
          trajectoryActor->SetVisibility(1);
          cueStickActor->SetVisibility(1);
          double powerTextValue = shotSpeedMagnitude;
          powerTextValue = ((powerTextValue - 0.02f) / (0.3f - 0.02f)) * (100 - 1) + 1;
          // Tworzymy strumieñ, aby sformatowaæ float do 2 miejsc po przecinku
          std::ostringstream oss;
          oss << std::fixed << std::setprecision(2) << powerTextValue;
          // Pobieramy wynik jako string
          std::string PowerText = "Power: " + oss.str() + "%";
          ShotPowerActor->SetInput(PowerText.c_str());
          balls[0]->ballActor->GetPosition(white_pos);
          UpdateCueSetPosition(cueStickActor, white_pos[0], white_pos[1], white_pos[2], 0, 0, -0.01);
          camera->SetFocalPoint(balls[0]->ballActor->GetPosition()[0], balls[0]->ballActor->GetPosition()[1], 0.25);
          if (ShotSpeedX > maxSpeedX) {
              ShotSpeedX = limitedSpeedX;
          }
          if (ShotSpeedY > maxSpeedY) {
              ShotSpeedY = limitedSpeedY;
          }
          trajectory.updateTrajectory(balls[0]->ballActor->GetPosition()[0], balls[0]->ballActor->GetPosition()[1], ShotSpeedX, ShotSpeedY);
          trajectory.updateGradient(shotSpeedMagnitude * shotSpeedMagnitude);
          if (StickShot)
          { 
              if (changeCenterY > 2.1) {
                  changeCenterY -= shotSpeedMagnitude;
                  cylinder->SetCenter(0.0, changeCenterY, 0.0);
              }
              else
              {
                  cylinder->SetCenter(0.0, cylinderCenterY, 0.0);
				  changeCenterY = cylinderCenterY;
                  cueStickActor->SetVisibility(0);
                  trajectoryActor->SetVisibility(0);
                  balls[0]->setSpeed(ShotSpeedX, ShotSpeedY);
                  StickShot = false;
                  velocityAngle = 0;
                  shotSpeedMagnitude = 0.16;
              }
			  
          }            
      }

      vtkRenderWidget->renderWindow()->Render();
      

      });

  timer.start(16); // Uruchom timer z interwa³em ~60 FPS (16 ms)

  return app.exec();
}

