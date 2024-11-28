// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#define _USE_MATH_DEFINES
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
#include <qthread.h>
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
#include <QDebug>
#include <QSurfaceFormat>
#include <QTimer>
#include <vtkCellPicker.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkline.h>

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








// Sta�e

// Zmienna globalna dla vtkActor
vtkSmartPointer<vtkActor> TableActor = vtkSmartPointer<vtkActor>::New();
std::vector<vtkSmartPointer<vtkActor>> ballActors;
double width = 10.0;  // Szeroko�� p�aszczyzny
double height = 5.0;   // Wysoko�� p�aszczyzny
const double minX = width / 15.75;  // Minimalna dopuszczalna pozycja X
const double maxX = width - width / 15.75;  // Maksymalna dopuszczalna pozycja X
const double minY = height / 7.75;  // Minimalna dopuszczalna pozycja Y
const double maxY = height - height / 7.75;  // Maksymalna dopuszczalna pozycja Y
const double resistanceFactor = 0.99;  // Wsp�czynnik oporu (tarcia)
const double restitutionCoefficient = 0.97; // Wsp�czynnik odbicia (0.0 - brak odbicia, 1.0 - idealne odbicie)
// Przyk�adowe wsp�rz�dne �rodk�w dziurek
  //const double holeRadius = 10.0; // Promie� dziurki
const double hole1[2] = { width / 15.75, height - height / 7.75 }; // Wsp�rz�dne pierwszej dziurki
const double hole2[2] = { width / 2, height - height / 7.75 }; // Wsp�rz�dne drugiej dziurki
const double hole3[2] = { width - width / 15.75, height - height / 7.75 }; // Trzecia dziurka
const double hole4[2] = { width / 15.75, height / 7.75 }; // Czw�rka
const double hole5[2] = { width / 2, height / 7.75 }; // Pi�tka
const double hole6[2] = { width - width / 15.75, height / 7.75 }; // Sz�stka
const double ballRadius = 0.15;

/////// RUCH 
// Funkcja sprawdzaj�ca, czy kula wpad�a do kt�rejkolwiek z dziurek

auto distanceCheck = [](double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    };



void SetUpCamera(vtkSmartPointer<vtkRenderer> renderer, double width, double height, double z)
{
    // �rodek sto�u
    double tableCenter[3] = { width , height - 5, z };

    // Tworzymy kamer�
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

    // Pozycja kamery - ustawiamy j� wy�ej i dalej od sto�u, aby uzyska� lepszy widok
    double cameraPosition[3] = { width , height, z + 3.0 }; // Kamera znajduje si� nad sto�em
    camera->SetPosition(cameraPosition);
    camera->SetFocalPoint(tableCenter);  // Kamera patrzy na �rodek sto�u
    // Ustawienie k�ta widzenia kamery
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
        //// Wy�wietlenie pozycji kamery za pomoc� qDebug
        //    // Wy�wietlenie pozycji kamery za pomoc� qDebug
        //QString message = QString("Pozycja kamery: x = %1, y = %2, z = %3")
        //    .arg(position[0]) // Podstawienie warto�ci x
        //    .arg(position[1]) // Podstawienie warto�ci y
        //    .arg(position[2]); // Podstawienie warto�ci z
        //// Wy�wietlenie komunikatu za pomoc� qDebug
        //qDebug()<< message;
        //camera->GetFocalPoint(position);
        //message = QString("Pozycja punktu patrzenia kamery: x = %1, y = %2, z = %3")
        //    .arg(position[0]) // Podstawienie warto�ci x
        //    .arg(position[1]) // Podstawienie warto�ci y
        //    .arg(position[2]); // Podstawienie warto�ci z
        //// Wy�wietlenie komunikatu za pomoc� qDebug
        //qDebug() << message;
        AngleIncrement = angle / TotalSteps; // 45 stopni w 90 krokach
    }

    // Funkcja startuj�ca animacj�
    void Start()
    {
        vtkNew<vtkCallbackCommand> timerCallback;
        timerCallback->SetCallback(SmoothCameraRotation::RotateCamera);
        timerCallback->SetClientData(this); // Przekazanie wska�nika do obiektu
        Interactor->AddObserver(vtkCommand::TimerEvent, timerCallback);
        TimerId = Interactor->CreateRepeatingTimer(10); // Co 10 ms
    }

    // Funkcja callback do obs�ugi rotacji kamery
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
            camera->Azimuth(self->AngleIncrement); // Obr�t kamery
            self->Interactor->GetRenderWindow()->Render(); // Od�wie� widok
            ++self->CurrentStep;
        }
        else
        {
            // Zako�czenie animacji
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


    void OnKeyDown() override
    {
        // Pobierz wci�ni�ty klawisz
        std::string key = this->GetInteractor()->GetKeySym();
        vtkRenderWindowInteractor* interactor = this->GetInteractor();
        vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        // Obs�uga prawej strza�ki
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
        // Wywo�anie oryginalnej implementacji (opcjonalne)
        //vtkInteractorStyleTrackballCamera::OnKeyDown();
    }
    void OnLeftButtonDown() override
    {
        // Pobierz wsp�rz�dne klikni�cia myszk�
        
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
                    if (pos[1] < 0)
                    {
                        this->trackedActor = pickedActor;
                        this->IsSelected = true;
                        std::cout << "Wybor miejsca dla kuli" << std::endl;
                        this->trackedActor->GetProperty()->SetColor(0.5, 1.0, 0.5);   // Kolor po�wiaty (��ty)
                        this->trackedActor->GetProperty()->SetOpacity(0.5);          // Przezroczysto��
                        this->trackedActor->SetScale(1.25);
                    }
                }
            }
        }
        // Wywo�anie oryginalnej implementacji
        //vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
    // Wykonywana podczas przesuwania myszk�, ale zablokowana, gdy IsDrag jest false
    void OnMiddleButtonDown() override
    {
        // Zablokowanie przesuwania kamery �rodkowym przyciskiem myszy
        // Nie wywo�uj funkcji bazowej, aby nie przesuwa� kamery
        // vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
    }
    void OnRightButtonDown() override {
 
        if (IsSelected)
        {
            vtkRenderWindowInteractor* interactor = this->GetInteractor();
            vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();


            // Pobierz bie��c� pozycj� kursora
            int* mousePos = interactor->GetEventPosition();
            // Stw�rz picker do wykrywania klikni�� na aktorach
            vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
            picker->SetTolerance(0.001);  // Ustaw tolerancj� pickingu
            if (picker->Pick(mousePos[0], mousePos[1], 0, renderer)) {
                vtkSmartPointer<vtkActor> pickedActor = picker->GetActor();

                if (pickedActor == TableActor) {
                    // Pobierz wsp�rz�dne punktu na powierzchni obiektu
                    double pickedPos[3];
                    picker->GetPickPosition(pickedPos);  // Pobierz punkt na powierzchni obiektu

                    // Wydrukuj wynik
                    /*std::cout << "Klikni�to aktor o wsp�rz�dnych X: " << pickedPos[0]
                        << ", Y: " << pickedPos[1]
                        << ", Z: " << pickedPos[2] << std::endl;*/

                    // Ustaw pozycj� trackedActor na wybranej lokalizacji (na powierzchni obiektu)
                    if (pickedPos[0] > minX && pickedPos[0] < maxX && pickedPos[1] > minY && pickedPos[1] < maxY)
                    {
                        this->trackedActor->SetPosition(pickedPos[0], pickedPos[1], 0.15);  // Z ustawiony na 0.15
                        vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
                        camera->SetFocalPoint(pickedPos[0], pickedPos[1], 0.15);
                        this->trackedActor->GetProperty()->SetColor(1.0, 1.0, 1.0);   // Kolor po�wiaty (��ty)
                        this->trackedActor->GetProperty()->SetOpacity(1.0);          // Przezroczysto��
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
            //vtkInteractorStyleTrackballCamera::OnMouseMove(); // Obr�t kamery tylko wtedy, gdy przycisk jest wci�ni�ty
        }
    }

private:
    vtkSmartPointer<vtkActor> trackedActor;
    bool IsSelected;
    bool IsDrag; // Flaga m�wi�ca, czy myszka jest wci�ni�ta
};

// Definicja metody New, by poprawnie tworzy� obiekt
vtkStandardNewMacro(CameraInteractorStyle);


// Funkcja do generowania �cie�ki do tekstury
char* getTexturePath(double r, double g, double b, bool hasStripe) {
    // Kolory kul (w formacie RGB)
    if (!hasStripe) {
        if (r == 1.0 && g == 1.0 && b == 1.0) return (char*)"textures/white_ball_texture.jpg";  // Bia�a kula
        if (r == 1.0 && g == 0.0 && b == 0.0) return (char*)"textures/red_ball_texture.jpg";    // Czerwona kula
        if (r == 0.0 && g == 1.0 && b == 0.0) return (char*)"textures/green_ball_texture.jpg";  // Zielona kula
        if (r == 0.0 && g == 0.0 && b == 1.0) return (char*)"textures/blue_ball_texture.jpg";   // Niebieska kula
        if (r == 1.0 && g == 1.0 && b == 0.0) return (char*)"textures/yellow_ball_texture.jpg"; // ��ta kula
        if (r == 1.0 && g == 0.0 && b == 1.0) return (char*)"textures/purple_ball_texture.jpg"; // Fioletowa kula
        if (r == 0.0 && g == 1.0 && b == 1.0) return (char*)"textures/orange_ball_texture.jpg"; // Pomara�czowa kula
        if (r == 0.5 && g == 0.5 && b == 0.5) return (char*)"textures/brown_ball_texture.jpg";  // Br�zowa kula
    }
    if (hasStripe) {
        // Dla kul z paskiem dodaj prefiks 'st_' przed kolorem
        if (r == 0.5 && g == 0.5 && b == 1.0) return (char*)"textures/st_blue_ball_texture.jpg";   // Niebieska z paskiem
        if (r == 1.0 && g == 0.0 && b == 1.0) return (char*)"textures/st_yellow_ball_texture.jpg"; // ��ta z paskiem
        if (r == 0.0 && g == 1.0 && b == 0.0) return (char*)"textures/st_red_ball_texture.jpg";    // Czerwona z paskiem
        if (r == 1.0 && g == 0.0 && b == 0.0) return (char*)"textures/st_green_ball_texture.jpg";  // Zielona z paskiem
        if (r == 0.0 && g == 0.0 && b == 1.0) return (char*)"textures/st_blue_ball_texture.jpg";   // Niebieska z paskiem
        if (r == 0.0 && g == 0.5 && b == 0.5) return (char*)"textures/st_orange_ball_texture.jpg"; // Pomara�czowa z paskiem
        if (r == 0.5 && g == 0.0 && b == 0.0) return (char*)"textures/st_purple_ball_texture.jpg"; // Fioletowa z paskiem
        if (r == 0.5 && g == 0.5 && b == 0.5) return (char*)"textures/st_brown_ball_texture.jpg";     // Br�zowa z paskiem
    }
    return (char*)"textures/black_ball_texture.jpg";  // Kula 8 (czarna)
}

// Funkcja do tworzenia kuli bilarda
vtkSmartPointer<vtkActor> createBall(double x, double y, double z, double r, double g, double b, bool hasStripe) {
    vtkNew<vtkTexturedSphereSource> sphereSource;
    //sphereSource->SetCenter(x, y, z);  // Pozycja kuli
    sphereSource->SetRadius(ballRadius);    // Promie� kuli
    sphereSource->SetPhiResolution(50);  // Rozdzielczo�� kuli
    sphereSource->SetThetaResolution(50);  // Rozdzielczo�� kuli

    // �adowanie tekstury z pliku
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> textureFile;
    // Pobranie �cie�ki do tekstury
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

    // Mapowanie tekstury na kul�
    vtkNew<vtkTextureMapToSphere> textureMapToSphere;
    textureMapToSphere->SetInputConnection(sphereSource->GetOutputPort());

    // Tworzymy transformacj� wsp�rz�dnych tekstury
    vtkNew<vtkTransformTextureCoords> transformTexture;
    transformTexture->SetInputConnection(textureMapToSphere->GetOutputPort());
    transformTexture->AddPosition(-0.75, 0.0, 0.0);
    // Skalowanie wsp�rz�dnych tekstury w poziomie
    transformTexture->SetScale(-0.5, -1.0, 1.0);  // Rozci�ganie tekstury w osi X
    
    vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    sphereMapper->SetInputConnection(transformTexture->GetOutputPort());

    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->SetTexture(texture); 
    //texture->SetInputData(textureImage);
    
    sphereActor->SetPosition(x, y, z);
    sphereActor->GetProperty()->SetSpecular(0.4);  // Dodaj po�ysk
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
    double pushPower = 1.0;
    int id;
    bool isHoled = false;
    Ball(vtkSmartPointer<vtkActor> actor, int id, double  ballSpeedX, double  ballSpeedY, double  pushPower)
    {
        this->id = id;
        this->ballActor = actor;
        this->ballSpeedX = ballSpeedX;
        this->ballSpeedY = ballSpeedY;
        this->pushPower = pushPower;
    }

    void setIsHoled(bool isHoled)
    {
        this->isHoled = isHoled;
    }
    void setPower(double power)
    {
        this->pushPower = power;
    }
    void setSpeed(double speedX, double speedY)
    {
        this->ballSpeedX = speedX;
        this->ballSpeedY = speedY;
    }

    void updatePosition()
    {
        double position[3];
        double normalX, normalY;
        ballActor->GetPosition(position);


        // TRAFIENIE DO DZIUR
        processBallInHole(position, ballSpeedX, ballSpeedY, isHoled, id);

        getNormal(normalX, normalY, position[0], position[1], isHoled);

        calcRotationSpeed(ballActor, ballSpeedX, ballSpeedY);

        // Zastosowanie oporu do pr�dko�ci (spowalnia z czasem)
        ballSpeedX *= resistanceFactor;  // Zmniejsz pr�dko�� w osi X
        ballSpeedY *= resistanceFactor;  // Zmniejsz pr�dko�� w osi Y

        if (normalX != 0.0 || normalY != 0.0) {
            reflectBall(ballSpeedX, ballSpeedY, normalX, normalY, isHoled);
        }
        position[0] += ballSpeedX;
        position[1] += ballSpeedY;

        // Zatrzymywanie si� kuli po osi�gni�ciu niskiej pr�dko�ci (minimalna pr�dko��)
        if (fabs(ballSpeedX) < 0.0005 && fabs(ballSpeedY) < 0.0005) {
            ballSpeedX = 0.0;  // Zatrzymanie pr�dko�ci w osi X
            ballSpeedY = 0.0;  // Zatrzymanie pr�dko�ci w osi Y
            //camera->SetFocalPoint(ballActors[0]->GetPosition());
        }
        ballActor->SetPosition(position);
    }


    int isInHole(double ballX, double ballY) {

        double holeRadius = 0.1;
        // Sprawdzamy odleg�o�� od ka�dego �rodka dziurki
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

                position[0] = 0.3 + id * 0.35; position[1] = -0.35; position[2] = -0.5;
                isHoled = false;
            }
        }
    }

    void calcRotationSpeed(vtkActor* ball, double ballSpeedX, double ballSpeedY)
    {
        // Obliczanie rotacji
        double speedVector = std::sqrt(ballSpeedX * ballSpeedX + ballSpeedY * ballSpeedY);
        double angularSpeed = speedVector / 0.15;

        // O� obrotu
        double rotationAxis[3] = { -ballSpeedY, ballSpeedX, 0.0 };
        double axisLength = std::sqrt(rotationAxis[0] * rotationAxis[0] + rotationAxis[1] * rotationAxis[1]);
        rotationAxis[0] /= axisLength;
        rotationAxis[1] /= axisLength;

        // K�t rotacji w tej klatce
        double angle = angularSpeed * 16.0 / 15.0; // Konwersja z ms do sekund

        // Aktualizacja transformacji
        ball->RotateWXYZ(angle * 180.0 / M_PI, rotationAxis[0], rotationAxis[1], rotationAxis[2]);
    }


    // Funkcja do obliczenia normalnej powierzchni (zak�adaj�c odbicie od �cian)
    void getNormal(double& normalX, double& normalY, double& posX, double& posY, bool& isHoled) {


        // Zresetowanie normalnych
        normalX = 0.0;
        normalY = 0.0;
        // Sprawdzenie odbicia od pionowych �cian
        if (posX <= minX) {
            normalX = 1.0;
            //qDebug() << "\n" << " X: " << posX;
        }
        else if (posX >= maxX) {
            normalX = -1.0;
            //qDebug() << "\n" << " X: " << posX;
        }

        // Sprawdzenie odbicia od poziomych �cian
        if (posY <= minY) {
            normalY = 1.0;
            //qDebug() << "\n" << " minY: " << minY << " Y: " << posY;
        }
        else if (posY >= maxY) {
            normalY = -1.0;
            //qDebug() << "\n" << " maxY: " << maxY << " Y: " << posY;
        }

        // Dodanie tolerancji dla unikni�cia oscylacji
        const double epsilon = 0.01;
        if (!isHoled) {
            if (std::abs(posX - minX) < epsilon || std::abs(posX - maxX) < epsilon) {
                posX += (normalX * epsilon);  // Przesuni�cie poza granic� w odpowiednim kierunku
                //qDebug() << "\n" << " outX: " << posX;
            }

            if (std::abs(posY - minY) < epsilon || std::abs(posY - maxY) < epsilon) {
                posY += (normalY * epsilon);  // Przesuni�cie poza granic� w odpowiednim kierunku
                //qDebug() << "\n" << " outY: " << posY;
            }
        }

    }
    // Funkcja do obliczenia k�ta odbicia kuli
    void reflectBall(double& ballSpeedX, double& ballSpeedY, double normalX, double normalY, bool& isHoled) {
        // Obliczanie k�ta odbicia
        double dotProduct = ballSpeedX * normalX + ballSpeedY * normalY;
        double normalMagnitudeSquared = normalX * normalX + normalY * normalY;

        // Obliczenie skalarnej projekcji pr�dko�ci na normaln� powierzchni
        double projection = 2.0 * dotProduct / normalMagnitudeSquared;

        // Pr�dko�� odbicia (kierunek zmienia si� o podw�jny rzut na normaln�)
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
        // Obliczenie odleg�o�ci mi�dzy �rodkami kul
        double dx = pos2[0] - pos1[0];
        double dy = pos2[1] - pos1[1];
        double distance = std::sqrt(dx * dx + dy * dy);
        double radiusSum = ballRadius + ballRadius;

        // Je�eli kule si� przecinaj�
        if (distance <= radiusSum && distance > 1e-5) {
            // Minimum Translation Distance (MTD) - wektor, kt�ry przesunie kule
            double mtdX = dx * ((radiusSum - distance) / distance);
            double mtdY = dy * ((radiusSum - distance) / distance);

            //// Obliczanie pr�dko�ci wzgl�dnej
            double vx = this->ballSpeedX - otherBall.ballSpeedX;
            double vy = this->ballSpeedY - otherBall.ballSpeedY;

            //// Normalizujemy wektor MTD
            double mtdLength = std::sqrt(mtdX * mtdX + mtdY * mtdY);
            double mtdNormalizedX = mtdX / mtdLength;
            double mtdNormalizedY = mtdY / mtdLength;

            //// Obliczanie pr�dko�ci w kierunku normalnym (dot product)
            double vn = vx * mtdNormalizedX + vy * mtdNormalizedY;

            //// Je�li kule si� oddalaj�, nie ma kolizji
            if (vn < 0.0) return;

            //// Wsp�czynnik restitucji (1 = idealna kolizja spr�ysta)
            const double restitution = 0.93;

            //// Impuls kolizji
            double i = -(1.0 + restitution) * vn / 2;
            double impulseX = mtdNormalizedX * i;
            double impulseY = mtdNormalizedY * i;

            // Zmiana p�du
            this->ballSpeedX += impulseX;
            this->ballSpeedY += impulseY;
            otherBall.ballSpeedX -= impulseX;
            otherBall.ballSpeedY -= impulseY;
            this->updatePosition();
            otherBall.updatePosition();
        }
    }
};



std::vector<Ball*> balls;


int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
  QApplication app(argc, argv);

  
  QMainWindow mainWindow;
  mainWindow.setWindowTitle("Bilard Game");
  mainWindow.resize(800, 800);

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


  // Utw�rz tekst
  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
  textActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Normalizowane wsp�rz�dne ekranu
  textActor->SetTextScaleModeToNone();
  textActor->SetInput("BILARD GAME");
  textActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  renderer->AddActor(textActor);
  // Utw�rz tekst
  vtkSmartPointer<vtkTextActor> textActorScore = vtkSmartPointer<vtkTextActor>::New();
  textActorScore->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Normalizowane wsp�rz�dne ekranu
  textActorScore->SetTextScaleModeToNone();
  textActorScore->SetInput("SCORE: 1000");
  textActorScore->SetPosition(0.8,0.01);
  textActorScore->GetTextProperty()->SetFontSize(24);
  // Ustawienie pozycji w prawym g�rnym rogu
  //textActorScore->SetDisplayPosition(0.8 * renderer->GetSize()[0], 0.9 * renderer->GetSize()[1]); // Pozycja na ekranie, gdzie 90% szeroko�ci i wysoko�ci ekranu
  textActorScore->SetWidth(1.0);
  textActorScore->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay(); // Ustawienie na normalizowane wsp�rz�dne



  // Dodaj tekst do renderer
  //renderer->AddActor2D(textActorScore);

  ////////////////////////////
  
  // Tworzenie PolyData
  vtkNew<vtkPolyData> polyData;
  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> quads;
  // Parametry siatki
  int pointsX = 400; // Liczba punkt�w w osi X
  int pointsY = 200;  // Liczba punkt�w w osi Y

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
  // Dodawanie punkt�w do siatki
  for (int j = 0; j < pointsY; j++) {
      for (int i = 0; i < pointsX; i++) {
          double x = i * (width / (pointsX - 1));
          double y = j * (height / (pointsY - 1));
          double z = 0.0;
          int distFromUpTab = 18;
          // Podnosimy punkty na kraw�dziach, aby stworzy� efekt band
          if (j <= (distFromUpTab - 2) || j >= pointsY - (distFromUpTab - 2) ||
              i <= (distFromUpTab - 2) || i >= pointsX - (distFromUpTab - 2)) {
              z = 0.3; // Podniesienie kraw�dzi 
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
              z = -0.5; // Obni�enie punktu w okolicy dziurki
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
          // Dodawanie wsp�rz�dnych UV
          double u = (i / double(pointsX - 1));  // U wsp�rz�dna
          double v = (j / double(pointsY - 1));  // V wsp�rz�dna
          textureCoordinates->InsertNextTuple2(u, v);

      }
  }
  // Dodawanie kwadrat�w do siatki
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
  // Dodawanie punkt�w i kom�rek (kwadrat�w) do PolyData
  polyData->SetPoints(points);
  polyData->SetPolys(quads);
  qDebug("Wygladzenie stolu...");
  vtkNew<vtkSmoothPolyDataFilter> smoother;
  smoother->SetInputData(polyData);
  smoother->SetNumberOfIterations(100);  // Liczba iteracji wyg�adzania
  smoother->SetRelaxationFactor(0.06);   // Czynnik relaksacji (im wy�szy, tym szybciej wyg�adza)
  smoother->Update();

  ///�WIAT�O
  qDebug("Dodawanie swiatla...");
  vtkNew<vtkLight> light;
  light->SetLightTypeToSceneLight();   // �wiat�o sceniczne
  light->SetPosition(width / 2 - 3, height / 2 - 3, 5);       // Pozycja �wiat�a (z g�ry nad �rodkiem sceny)
  light->SetFocalPoint(width / 2 - 1, height / 2 - 1, 0.5);      // Kierunek �wiat�a (�rodek sceny)
  light->SetColor(1.0, 1.0, 1.0);     // Bia�e �wiat�o
  light->SetIntensity(0.35);

  vtkNew<vtkLight> light2;
  light2->SetLightTypeToSceneLight();   // �wiat�o sceniczne
  light2->SetPosition(width / 2 + 3, height / 2 + 3, 5);       // Pozycja �wiat�a (z g�ry nad �rodkiem sceny)
  light2->SetFocalPoint(width / 2 + 1, height / 1 + 1, 0.5);      // Kierunek �wiat�a (�rodek sceny)
  light2->SetColor(1.0, 1.0, 1.0);     // Bia�e �wiat�o
  light2->SetIntensity(0.35);

  vtkNew<vtkLight> light3;
  light3->SetLightTypeToSceneLight();   // �wiat�o sceniczne
  light3->SetPosition(width / 2 - 3, height / 2 + 3, 5);       // Pozycja �wiat�a (z g�ry nad �rodkiem sceny)
  light3->SetFocalPoint(width / 2 + 1, height / 1 - 1, 0.5);      // Kierunek �wiat�a (�rodek sceny)
  light3->SetColor(1.0, 1.0, 1.0);     // Bia�e �wiat�o
  light3->SetIntensity(0.35);

  vtkNew<vtkLight> light4;
  light4->SetLightTypeToSceneLight();   // �wiat�o sceniczne
  light4->SetPosition(width / 2 + 3, height / 2 - 3, 5);       // Pozycja �wiat�a (z g�ry nad �rodkiem sceny)
  light4->SetFocalPoint(width / 2 - 1, height / 1 + 1, 0.5);      // Kierunek �wiat�a (�rodek sceny)
  light4->SetColor(1.0, 1.0, 1.0);     // Bia�e �wiat�o
  light4->SetIntensity(0.35);
  
  polyData->ShallowCopy(smoother->GetOutput());
  // Mapper i Actor
  vtkNew<vtkPolyDataMapper> mapper;

  mapper->SetInputData(polyData);
  TableActor->GetProperty()->SetRepresentationToPoints(); // Ustawienie na punkty
  TableActor->GetProperty()->SetPointSize(2); // Rozmiar punkt�w
  //TableActor->GetProperty()->SetColor(0.2, 0.8, 0.4); // Kolor siatki
  TableActor->SetMapper(mapper);
  TableActor->SetTexture(texture);
  TableActor->GetProperty()->SetRepresentationToSurface();
  //TableActor->GetProperty()->SetRepresentationToWireframe();
  TableActor->GetProperty()->SetEdgeVisibility(false);

  
  renderer->AddActor(TableActor);
  renderer->AddLight(light);          // Dodanie �wiat�a do renderera
  renderer->AddLight(light2);
  renderer->AddLight(light3);
  renderer->AddLight(light4);
  qDebug("Ustawianie kamery...");
  // Ustawienie kamery
  //SetUpCamera(renderer, width, height, 0.5);



  // Tworzenie obiektu reprezentuj�cego pozycj� �wiat�a
  vtkSmartPointer<vtkSphereSource> lightPositionSphere = vtkSmartPointer<vtkSphereSource>::New();
  lightPositionSphere->SetRadius(0.2); // Ma�a sfera reprezentuj�ca pozycj� �wiat�a
  lightPositionSphere->SetThetaResolution(10);
  lightPositionSphere->SetPhiResolution(10);
  lightPositionSphere->SetCenter(light->GetPosition());

  vtkSmartPointer<vtkPolyDataMapper> lightPositionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  lightPositionMapper->SetInputConnection(lightPositionSphere->GetOutputPort());

  vtkSmartPointer<vtkActor> lightPositionActor = vtkSmartPointer<vtkActor>::New();
  lightPositionActor->SetMapper(lightPositionMapper);
  lightPositionActor->GetProperty()->SetColor(1.0, 0.0, 0.0); // Czerwony kolor dla pozycji �wiat�a

  renderer->SetBackground2(0.002, 0.0022, 0.04);
  renderer->SetBackground(0.0, 0.0, 0.0);
  renderer->GradientBackgroundOn();
  //renderer->AddActor(lightPositionActor);  // Dodanie reprezentacji pozycji �wiat�a

  ///BILE
  qDebug("Dodawanie bil...");
  std::vector<std::tuple<double, double, double, double, double, double, bool>> ballsTemp = {
      {      width / 2 - 2.000, height / 2 + 0.000 , 0.15,  1.0, 1.0, 1.0, false},      // Bia�a kula (cue ball)
      {1.0 + width / 2 + 0.000, height / 2 + 0.000 , 0.15,  1.0, 0.0, 0.0, false},      // Czerwona kula (3)
      {1.0 + width / 2 + 0.275, height / 2 + 0.155 , 0.15,  0.0, 1.0, 0.0, false},      // Zielona kula (6)
      {1.0 + width / 2 + 0.275, height / 2 - 0.155 , 0.15,  1.0, 0.0, 1.0, true},       // ��ta z paskiem (9)
      {1.0 + width / 2 + 0.545, height / 2 + 0.305 , 0.15,  0.0, 0.0, 1.0, true},       // Niebieska z paskiem (10)
      {1.0 + width / 2 + 0.545, height / 2 - 0.305 , 0.15,  0.5, 0.0, 0.0, true},       // Fioletowa z paskiem (12)
      {1.0 + width / 2 + 0.820, height / 2 + 0.155 , 0.15,  0.0, 1.0, 0.0, true},       // Czerwona z paskiem (11)
      {1.0 + width / 2 + 0.820, height / 2 - 0.155 , 0.15,  0.5, 0.5, 0.5, false},      // Br�zowa kula (7)
      {1.0 + width / 2 + 0.820, height / 2 + 0.460 , 0.15,  0.5, 0.5, 0.5, true},       // Br�zowa z paskiem (15)
      {1.0 + width / 2 + 0.820, height / 2 - 0.460 , 0.15,  0.0, 0.0, 1.0, false},      // Niebieska kula (2)
      {1.0 + width / 2 + 1.095, height / 2 - 0.305 , 0.15,  0.0, 1.0, 1.0, false},      // Pomara�czowa kula (5)
      {1.0 + width / 2 + 1.095, height / 2 + 0.305 , 0.15,  1.0, 0.0, 0.0, true},       // Zielona z paskiem (14)
      {1.0 + width / 2 + 1.095, height / 2 + 0.000 , 0.15,  1.0, 1.0, 0.0, false},      // ��ta kula (1)
      {1.0 + width / 2 + 1.095, height / 2 - 0.615 , 0.15,  0.0, 0.5, 0.5, true},       // Pomara�czowa z paskiem (13)
      {1.0 + width / 2 + 1.095, height / 2 + 0.615 , 0.15,  1.0, 0.0, 1.0, false},      // Fioletowa kula (4)
      {1.0 + width / 2 + 0.545, height / 2 + 0.000 , 0.15,  0.0, 0.0, 0.0, false}       // Kula 8 (czarna)
  };

  SetUpCamera(renderer, std::get<0>(ballsTemp[0]), std::get<1>(ballsTemp[0]), std::get<2>(ballsTemp[0]));
  vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
  camera->SetFocalPoint(std::get<0>(ballsTemp[0]), std::get<1>(ballsTemp[0]), 0.15);
  //SetUpCamera(renderer, 2, 2, 0);
  double position[3];
  renderer->GetActiveCamera()->GetPosition(position);

  // Create five points.
  double origin[3] = { std::get<0>(ballsTemp[0]), std::get<1>(ballsTemp[0]), std::get<2>(ballsTemp[0]) };
  double p0[3] = { 1.0, 0.0, 0.20 };
  double p1[3] = { 0.0, 1.0, 0.20 };
  double p2[3] = { 0.0, 1.0, 0.20 };
  double p3[3] = { 1.0, 2.0, 0.20 };

  // Create a vtkPoints object and store the points in it
  vtkNew<vtkPoints> pointsLine;
  pointsLine->InsertNextPoint(origin);
  pointsLine->InsertNextPoint(p0);
  pointsLine->InsertNextPoint(p1);
  pointsLine->InsertNextPoint(p2);
  pointsLine->InsertNextPoint(p3);
  // Create a polydata to store everything in
  vtkNew<vtkPolyData> linesPolyData;
  vtkNew<vtkCellArray> lines;
  vtkNew<vtkActor> LineActor;
  for (unsigned int i = 0; i < 3; i++)
  {
      vtkNew<vtkLine> line;
      line->GetPointIds()->SetId(0, i);
      line->GetPointIds()->SetId(1, i + 1);
      lines->InsertNextCell(line);
  }
  LineActor->GetProperty()->SetLineWidth(4);
  LineActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

  renderer->AddActor(LineActor);


  // Wy�wietlenie pozycji kamery za pomoc� qDebug
      // Wy�wietlenie pozycji kamery za pomoc� qDebug
  QString message = QString("Pozycja kamery: x = %1, y = %2, z = %3")
      .arg(position[0]) // Podstawienie warto�ci x
      .arg(position[1]) // Podstawienie warto�ci y
      .arg(position[2]); // Podstawienie warto�ci z
  // Wy�wietlenie komunikatu za pomoc� qDebug
  qDebug() << message;

  // Ustawienie nowego stylu interakcji dla renderera
  vtkSmartPointer<CameraInteractorStyle> style = vtkSmartPointer<CameraInteractorStyle>::New();
  vtkSmartPointer<vtkRenderWindowInteractor> interactor = window->GetInteractor();
  interactor->SetInteractorStyle(style);

  qDebug("Startowanie interaktora...");
  interactor->EnableRenderOn();
  interactor->Start();

  qDebug("Fizyka bili...");
  // Tworzenie obiekt�w Ball

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
  double pushPower = 1.00;
  double ballSpeedX = 0.800 * pushPower;
  double ballSpeedY = 0.000 * pushPower;
  bool isHoled = false;

  balls[0]->setSpeed(ballSpeedX, ballSpeedY);
  balls[0]->setPower(pushPower);

  qDebug("Game start...");
  QObject::connect(&timer, &QTimer::timeout, [&]() {
      // Pobierz bie��c� pozycj� kuli

      
      // Pauza na 500 ms (pozwala na obserwacj� efekt�w)
      QCoreApplication::processEvents();
      QThread::msleep(1000); // Wstrzymanie na 500 ms

      
      vtkRenderWidget->renderWindow()->Render();

      });

  timer.start(16); // Uruchom timer z interwa�em ~60 FPS (16 ms)


  QTimer PhysicTimer;
  QObject::connect(&PhysicTimer, &QTimer::timeout, [&]() {

      bool allBallsStopped = true;
      for (Ball* ball : balls) {
          ball->updatePosition(); // Aktualizacja ka�dej kuli z kamer�
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
      }
      if (allBallsStopped) {
          camera->SetFocalPoint(balls[0]->ballActor->GetPosition());



      }
      });
  PhysicTimer.start(1);
  return app.exec();
}

