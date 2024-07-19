#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>


const int width = 1280;
const int height = 720; 

const int number_of_colors = 5;  //Here we change the number of species 
const int group_size = 200; // Here we have the total of particles per species


const int number_of_particles = number_of_colors * group_size;
float rMax = 0.50;
const float particleSize = 2;
float scaling = 0.800001;
float minimumSize = 1;
float boxDistance = 0;

float dt = 0.001;
const float friction_half_life = 0.040;
const float frictionFactor = pow(0.5, (dt/friction_half_life));
float beta = 0.3;
float forceFactor = 10;

std::vector<std::vector<float>> forces;
std::vector<sf::Color> allColors;
std::vector<float> positionsX;
std::vector<float> positionsY;
std::vector<float> positionsZ;
std::vector<float> velocitiesZ;
std::vector<float> velocitiesX;
std::vector<float> velocitiesY;
std::vector<sf::CircleShape> allCircles;


void start_containers();
void start_containers(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> c(0,255);
    std::uniform_real_distribution<> x(-2, 2);
    std::uniform_real_distribution<> y(-2, 2);
    std::uniform_real_distribution<> z(boxDistance, 2);
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
            velocitiesY.push_back(0);
            velocitiesX.push_back(0);
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

        if(xPosition < -2) {velocitiesX[i] *= -1; positionsX[i] = -2;}else if(xPosition > 2){velocitiesX[i] *= -1; positionsX[i] = 2;}
        if(yPosition < -2) {velocitiesY[i] *= -1; positionsY[i] = -2;}else if(yPosition > 2){velocitiesY[i] *= -1; positionsY[i] = 2;}
        if(zPosition < boxDistance) {velocitiesZ[i] *= -1; positionsZ[i] = boxDistance;}else if(zPosition > 2){velocitiesZ[i] *= -1; positionsZ[i] = 2;}
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
        velocitiesY[i] += totalForceY * dt;
        velocitiesZ[i] += totalForceZ * dt;
        //updating positions:

        positionsX[i] += velocitiesX[i] * dt;
        positionsY[i] += velocitiesY[i] * dt;
        positionsZ[i] += velocitiesZ[i] * dt;
        
        //updating the circle 2d position:

        float k = 1/(positionsZ[i] + 2) ;
        float screenX = (positionsX[i] * k + 1) * 0.5 * width;
        float screenY = (positionsY[i] * k + 1) * 0.5 * height;

        //std::cout<<positionsZ[i]<<std::endl;
        sf::CircleShape circle(scaling - scaling/2*(positionsZ[i] - boxDistance) + minimumSize);
        circle.setPosition(screenX, screenY);
        circle.setFillColor(allColors[i % number_of_colors]);
        allCircles.push_back(circle);
        //allCircles[i].setPosition(screenX, screenY);
        //allCircles[i](1);

    } 
}



void update(sf::RenderWindow &window);
void update(sf::RenderWindow &window){
    
    updateParticles();

    //drawing the particles:
    for(auto particle : allCircles){
        window.draw(particle);
    }
}

int main(){

    start_containers();

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "LIFE 2.0", sf::Style::Fullscreen);
    
    while(window.isOpen()){

        sf::Event event;

        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }

            if(event.type == sf::Event::KeyPressed){
                
                
                if(event.key.code == sf::Keyboard::P){
                    forceFactor += 0.5;
                    std::cout<<"Force Facto : "<<forceFactor<<std::endl;
                }else if(event.key.code == sf::Keyboard::O){
                    forceFactor -= 0.5;
                    std::cout<<"Force Facto : "<<forceFactor<<std::endl;                    
                }
                if(event.key.code == sf::Keyboard::L){
                    dt += 0.001;
                    std::cout<<"dt : "<<dt<<std::endl;
                }else if(event.key.code == sf::Keyboard::K){
                    if(dt - 0.001 > 0) dt -= 0.001;
                    std::cout<<"dt : "<<dt<<std::endl;
                }
                 if(event.key.code == sf::Keyboard::M){
                    rMax += 0.01;
                    std::cout<<"R MAX : "<<rMax<<std::endl;
                }else if(event.key.code == sf::Keyboard::N){
                    if(rMax - 0.01 > 0) rMax -= 0.01;
                    std::cout<<"R MAX : "<<rMax<<std::endl;
                }
                 if(event.key.code == sf::Keyboard::H){
                    beta += 0.05;
                    std::cout<<" BETA : "<<beta<<std::endl;
                }else if(event.key.code == sf::Keyboard::G){
                    if(beta - 0.05 > 0) beta -= 0.05;
                    std::cout<<" BETA : "<<beta<<std::endl;
                }
                 if(event.key.code == sf::Keyboard::S){
                    scaling += 0.2;
                    std::cout<<"SCALING : "<<scaling<<std::endl;
                }else if(event.key.code == sf::Keyboard::A){
                    if(scaling - 0.2 > 0) scaling -= 0.2;
                    std::cout<<"SCALING : "<<scaling<<std::endl;
                }
            
            }

        }

        window.clear(sf::Color::Black);

        update(window);

        window.display();
    }

    return 0;

}



