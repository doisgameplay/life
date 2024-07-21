#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>


const int width = 1280;
const int height = 720; 
const float mouseVelocitie = 30;

const int number_of_colors = 10;  //Here we change the number of species 
const int group_size = 100; // Here we have the total of particles per species


const int number_of_particles = number_of_colors * group_size;
float rMax = 0.50;
const float particleSize = 100;
float scaling = 0.8001;
float minimumSize = 1;
float boxDistance = 0;
float boxZSize = 2;
float boxXSize = 2;
float boxYSize = 2;
float zoom = 2;
float cameraX = 0;
float cameraY = 0;
bool toggleLines = true;

float dt = 0.006;
const float friction_half_life = 0.040;
const float frictionFactor = pow(0.5, (dt/friction_half_life));
float beta = 0.3;
float forceFactor = 10;
float gravity = 9.5;


std::vector<std::vector<float>> forces;
std::vector<sf::Color> allColors;
std::vector<float> positionsX;
std::vector<float> positionsY;
std::vector<float> positionsZ;
std::vector<float> velocitiesZ;
std::vector<float> velocitiesX;
std::vector<float> velocitiesY;
std::vector<sf::CircleShape> allCircles;
sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

void start_containers();
void start_containers(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> c(0,255);
    std::uniform_real_distribution<> x(-boxXSize, boxXSize);
    std::uniform_real_distribution<> y(-boxYSize, boxYSize);
    std::uniform_real_distribution<> z(boxDistance, boxZSize);
    std::uniform_real_distribution<> value(-1,1);

        
    for(int i = 0; i < number_of_colors; i++){
        //colors:
        sf::Color uniqColor(c(gen), c(gen), c(gen), 255);
        allColors.push_back(uniqColor);    
    
        //forces:
        std::vector<float> Ci;
        for(int j = 0; j < number_of_colors; j++){
            Ci.push_back(value(gen));
        }
        forces.push_back(Ci);
    }
    
    for(int i = 1; i <= number_of_colors; i++ ){
        for(int j = 0; j < (number_of_particles/number_of_colors); j++){
            //particles positions and velocities:
            float xPosition = x(gen);
            float yPosition = y(gen);
            float zPosition = z(gen);
            positionsX.push_back(xPosition);
            positionsY.push_back(yPosition);
            positionsZ.push_back(zPosition);
            //positionsX.push_back(-boxXSize);
            //positionsY.push_back(boxYSize);
            //positionsZ.push_back(boxZSize);
            
            velocitiesX.push_back(0);
            velocitiesY.push_back(0);
            velocitiesZ.push_back(0);

            //circles positions and color:
            sf::CircleShape circle(particleSize);
            circle.setPosition(xPosition, yPosition);
            circle.setFillColor(allColors[i - 1]);
            allCircles.push_back(circle);
       }
    }    
}



void makeRandomForces();
void makeRandomForces(){
    std::vector<std::vector<float>> matrix;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> value(0,1);
    
    forces.clear();

    for(int i = 0; i < number_of_colors; i++){
        std::vector<float> Ci;
        for(int j = 0; j < number_of_colors; j++){
            Ci.push_back(value(gen));
        }
        forces.push_back(Ci);
    }    
}


float force(float r, float a);
float force(float r, float a){
    if( r < beta){
        return r/beta - 1;
    }
    if( beta < r && r < 1 ){
        return a * (1 - abs(2 * r - 1 - beta)/(1- beta));
    }
    return 0;
}

void checkCorner();
void checkCorner(){
    for(int i = 0; i < number_of_particles; i++){
        float xPosition = positionsX[i];
        float yPosition = positionsY[i];
        float zPosition = positionsZ[i];

        if(xPosition < -boxXSize) {velocitiesX[i] *= -1; positionsX[i] = -boxXSize;}else if(xPosition > boxXSize){velocitiesX[i] *= -1; positionsX[i] = boxXSize;}
        if(yPosition < -boxYSize) {velocitiesY[i] *= -1; positionsY[i] = -boxYSize;}else if(yPosition > boxYSize){velocitiesY[i] *= -1; positionsY[i] = boxYSize;}
        if(zPosition < boxDistance) {velocitiesZ[i] *= -1; positionsZ[i] = boxDistance;}else if(zPosition > boxDistance + boxZSize){velocitiesZ[i] *= -1; positionsZ[i] = boxDistance + boxZSize;}
    }
}





void updateParticles();
void updateParticles(){
    
    //updating velocities:
    checkCorner();

    allCircles.clear();

    for(int i = 0; i < number_of_particles; i++){
        float totalForceX = 0;
        float totalForceY = 0;
        float totalForceZ = 0;
        for(int j = 0; j < number_of_particles; j++){
            if(i == j) continue;
            float rx = positionsX[j] - positionsX[i];   
            float ry = positionsY[j] - positionsY[i];
            float rz = positionsZ[j] - positionsZ[i];
            float r = sqrt(rx*rx + ry*ry + rz*rz);
            if(r > 0 && r < rMax){
                float f = force(r/rMax,forces[i % number_of_colors][j % number_of_colors]);
                totalForceX += rx/r * f;
                totalForceY += ry/r * f;
                totalForceZ += rz/r * f;
            }
        }

        totalForceX *= rMax * forceFactor;
        totalForceY *= rMax * forceFactor;
        totalForceZ *= rMax * forceFactor;

        velocitiesX[i] *= frictionFactor;
        velocitiesY[i] *= frictionFactor;
        velocitiesZ[i] *= frictionFactor;

        velocitiesX[i] += totalForceX * dt;
        velocitiesY[i] += (totalForceY + gravity) * dt ;
        velocitiesZ[i] += totalForceZ * dt;
        //updating positions:

        positionsX[i] += velocitiesX[i] * dt;
        positionsY[i] += velocitiesY[i] * dt;
        positionsZ[i] += velocitiesZ[i] * dt;
        
        //updating the circle 2d position:

        float k = 1/(positionsZ[i] + zoom) ;
        float screenX = (positionsX[i] * k + 1) * 0.5 * width  + cameraX;
        float screenY = (positionsY[i] * k + 1) * 0.5 * height + cameraY;

        // {(-boxXSize, 0, 0 );  k = 1/2 ; (  (-boxXSize*k+1)*0.5 , 0 )}

        sf::CircleShape circle(scaling - scaling/2*(positionsZ[i] - boxDistance) + minimumSize);
        circle.setPosition(screenX, screenY);
        circle.setFillColor(allColors[i % number_of_colors]);
        allCircles.push_back(circle);

    } 
}

void drawLine(sf::RenderWindow &window,float x0, float y0, float z0, float x1, float y1, float z1);
void drawLine(sf::RenderWindow &window,float x0, float y0, float z0, float x1, float y1, float z1){
    float n;
    n = 1/(z0 + zoom);
    sf::Vector2f p8((x0 * n + 1) * 0.5 * width + cameraX, (y0 * n + 1) * 0.5 * height + cameraY);
    n = 1/(z1 + zoom);
    sf::Vector2f p9((x1 * n + 1) * 0.5 * width + cameraX, (y1 * n + 1) * 0.5 * height + cameraY);
    sf::VertexArray line5(sf::Lines, 2);
    line5[0].position = p8;
    line5[0].color = sf::Color::Red;
    line5[1]. position = p9;
    line5[1].color = sf::Color::Red;
    window.draw(line5);
}


void update(sf::RenderWindow &window);
void update(sf::RenderWindow &window){
    
    updateParticles();

    if(toggleLines){
        drawLine(window,boxXSize,-boxYSize,0,boxXSize,-boxYSize,boxZSize);
        drawLine(window,boxXSize, boxYSize,0,boxXSize, boxYSize, boxZSize);
        drawLine(window,-boxXSize, boxYSize,0,-boxXSize, boxYSize, boxZSize);
        drawLine(window,-boxXSize, -boxYSize,0,-boxXSize, -boxYSize, boxZSize);
        drawLine(window,boxXSize, boxYSize,0,boxXSize, boxYSize, boxZSize);
        drawLine(window,-boxXSize, -boxYSize,boxZSize,boxXSize, -boxYSize, boxZSize);
        drawLine(window,-boxXSize, boxYSize,boxZSize,boxXSize, boxYSize, boxZSize);
        drawLine(window,-boxXSize, -boxYSize,boxZSize,-boxXSize, boxYSize, boxZSize);
        drawLine(window,boxXSize, -boxYSize,boxZSize,boxXSize, boxYSize, boxZSize);
        drawLine(window,-boxXSize, -boxYSize,0,-boxXSize, boxYSize, 0);
        drawLine(window,boxXSize, boxYSize,0,boxXSize, -boxYSize, 0);
        drawLine(window,-boxXSize, -boxYSize,0,boxXSize,-boxYSize, 0);
        drawLine(window,-boxXSize, boxYSize,0,boxXSize, boxYSize, 0);
    }

    //drawing the particles:
    for(auto particle : allCircles){
        window.draw(particle);
    }
}

void keyEvents(sf::Event event){



    bool Equal = sf::Keyboard::isKeyPressed(sf::Keyboard::Equal);
    bool Hyphen = sf::Keyboard::isKeyPressed(sf::Keyboard::Hyphen);
    bool PKey = sf::Keyboard::isKeyPressed(sf::Keyboard::P);
    bool OKey = sf::Keyboard::isKeyPressed(sf::Keyboard::O);
    bool LKey = sf::Keyboard::isKeyPressed(sf::Keyboard::L);
    bool KKey = sf::Keyboard::isKeyPressed(sf::Keyboard::K);
    bool MKey = sf::Keyboard::isKeyPressed(sf::Keyboard::M);
    bool NKey = sf::Keyboard::isKeyPressed(sf::Keyboard::N);
    bool HKey = sf::Keyboard::isKeyPressed(sf::Keyboard::H);
    bool GKey = sf::Keyboard::isKeyPressed(sf::Keyboard::G);
    bool UpKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool DownKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    bool Num8Key = sf::Keyboard::isKeyPressed(sf::Keyboard::Num8);
    bool Num7Key = sf::Keyboard::isKeyPressed(sf::Keyboard::Num7);
    bool Num4Key = sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);
    bool Num3Key = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);
    bool Num6Key = sf::Keyboard::isKeyPressed(sf::Keyboard::Num6);
    bool Num5Key = sf::Keyboard::isKeyPressed(sf::Keyboard::Num5);
    bool Num2Key = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
    bool Num1Key = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
    bool IKey = sf::Keyboard::isKeyPressed(sf::Keyboard::I);
    bool AKey = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool DKey = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    bool WKey = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool SKey = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    bool LShift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);

    if(Equal){
        gravity+=0.5;
        std::cout<<"gravity : "<<gravity<<std::endl;
    }else if(Hyphen){
        if(gravity - 0.5 >= 0){gravity-=0.5;}else{gravity = 0;} 
        std::cout<<"gravity : "<<gravity<<std::endl;
        }

    if (PKey) {
        forceFactor += 0.5;
        std::cout << "Force Factor : " << forceFactor << std::endl;
    } else if (OKey) {
        forceFactor -= 0.5;
        std::cout << "Force Factor : " << forceFactor << std::endl;
    }

    if (LKey) {
        dt += 0.001;
        std::cout << "dt : " << dt << std::endl;
    } else if (KKey) {
        if (dt - 0.001 > 0) dt -= 0.001;
        std::cout << "dt : " << dt << std::endl;
    }

    if (MKey) {
        rMax += 0.01;
        std::cout << "R MAX : " << rMax << std::endl;
    } else if (NKey) {
        if (rMax - 0.01 > 0) rMax -= 0.01;
        std::cout << "R MAX : " << rMax << std::endl;
    }

    if (HKey) {
        beta += 0.05;
        std::cout << "BETA : " << beta << std::endl;
    } else if (GKey) {
        if (beta - 0.05 > 0) beta -= 0.05;
        std::cout << "BETA : " << beta << std::endl;
    }

    if (UpKey) {
        scaling += 0.2;
        std::cout << "SCALING : " << scaling << std::endl;
    } else if (DownKey) {
        if (scaling - 0.2 > 0) scaling -= 0.2;
        std::cout << "SCALING : " << scaling << std::endl;
    }

    if (Num8Key) {
        boxZSize += 0.2;
        std::cout << "boxZSize : " << boxZSize << std::endl;
    } else if (Num7Key) {
        if (boxZSize - 0.2 > -1) boxZSize -= 0.2;
        std::cout << "boxZSize : " << boxZSize << std::endl;
    }

    if (Num4Key) {
        boxYSize += 0.2;
        std::cout << "boxYSize : " << boxYSize << std::endl;
    } else if (Num3Key) {
        if (boxYSize - 0.2 > -1) boxYSize -= 0.2;
        std::cout << "boxYSize : " << boxYSize << std::endl;
    }

    if (Num6Key) {
        boxXSize += 0.2;
        std::cout << "boxXSize : " << boxXSize << std::endl;
    } else if (Num5Key) {
        if (boxXSize - 0.2 > -1) boxXSize -= 0.2;
        std::cout << "boxXSize : " << boxXSize << std::endl;
    }

    if (Num2Key) {
        boxXSize += 0.2;
        std::cout << "boxXSize : " << boxXSize << std::endl;
        boxZSize += 0.2;
        std::cout << "boxZSize : " << boxZSize << std::endl;
        boxYSize += 0.2;
        std::cout << "boxYSize : " << boxYSize << std::endl;
    } else if (Num1Key) {
        if (boxXSize - 0.2 > 0) boxXSize -= 0.2;
        std::cout << "boxXSize : " << boxXSize << std::endl;
        if (boxZSize - 0.2 > 0) boxZSize -= 0.2;
        std::cout << "boxZSize : " << boxZSize << std::endl;
        if (boxYSize - 0.2 > 0) boxYSize -= 0.2;
        std::cout << "boxYSize : " << boxYSize << std::endl;
    }

    if (IKey) {
        toggleLines = !toggleLines;
    }

    if (AKey) {
        if (LShift) {
            cameraX += 20;
        } else {
            cameraX += 5;
        }
        std::cout << "cameraX : " << cameraX << std::endl;
    } else if (DKey) {
        if (LShift) {
            cameraX -= 20;
        } else {
            cameraX -= 5;
        }
        std::cout << "cameraX : " << cameraX << std::endl;
    }

    if (WKey) {
        if (LShift) {
            cameraY += 20;
        } else {
            cameraY += 5;
        }
        std::cout << "cameraY : " << cameraY << std::endl;
    } else if (SKey) {
        if (LShift) {
            cameraY -= 20;
        } else {
            cameraY -= 5;
        }
        std::cout << "cameraY : " << cameraY << std::endl;
    }

              

}
void mouseUsed(sf::Event event, sf::RenderWindow &window);
void mouseUsed(sf::Event event, sf::RenderWindow &window){
    
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    float k = 1/(zoom) ;
    float screenX = cameraX; 
    float screenY = cameraY;
    float temp = mouse.x;
    float mouseX = temp/desktop.width*2 -1;
    temp = mouse.y;
    float mouseY = temp/desktop.height*2 -1;

    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        if (event.mouseWheelScroll.delta > 0) {
                zoom -= 0.1;
                if(mouseX < 0){
                    cameraX -= mouseX*mouseVelocitie;
                }else{cameraX -= mouseX*mouseVelocitie;}
                if(mouseY < 0){
                    cameraY -= mouseY * mouseVelocitie;
                }else{cameraY -= mouseY * mouseVelocitie;}
            }else {
                zoom += 0.1;
                if(mouseX < 0){
                    cameraX += mouseX * mouseVelocitie * 0.1;
                }else{cameraX += mouseX * mouseVelocitie * 0.1;}
                if(mouseY < 0){
                    cameraY += mouseY * mouseVelocitie * 0.1;
                }else{cameraY += mouseY * mouseVelocitie * 0.1;}
        }
    }
    

}

int main(){

    start_containers();

    sf::RenderWindow window(desktop, "LIFE 3.0", sf::Style::Fullscreen);
    sf::Music music;
    music.openFromFile("Sounds/theme.mp3");
    music.play();
    
    while(window.isOpen()){

        sf::Event event;

        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
            if(event.type == sf::Event::KeyPressed){     
                keyEvents(event);
            }
            if(event.type == sf::Event::MouseWheelScrolled){
            mouseUsed(event, window);
            }
        }

        window.clear(sf::Color::Black);

        update(window);

        window.display();
    }

    return 0;

}



