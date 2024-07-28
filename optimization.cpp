#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>

const int width = 5000;
const int height = 5000; 
const int gridSize = 40; //will divide the grid on an 10 x 10 ;
float xCam = 0;
float yCam = 0;
float zoom = 1;
float mouseVelocitie = 1;

const int number_of_colors = 20;
const int group_size = 250;
const int number_of_particles = number_of_colors * group_size;
const int number_of_dimensions = 2;
float rMax = 80.0f;
const float particleSize = 2.0f;

float dt = 0.006f;
const float friction_half_life = 0.040;
const float frictionFactor = pow(0.5, (dt/friction_half_life));
float beta = 0.3;
float forceFactor = 10;


void start_containers(std::vector<std::vector<float>>& forces,std::vector<sf::Color>& allColors,std::vector<float>& positionsX,std::vector<float>& positionsY,std::vector<float>& velocitiesX,std::vector<float>& velocitiesY,std::vector<sf::CircleShape>& allCircles, std::vector<std::vector<int>>& hashGrid, std::unordered_map<int, std::vector<int>>& hashNeighbors);
void start_containers(std::vector<std::vector<float>>& forces,std::vector<sf::Color>& allColors,std::vector<float>& positionsX,std::vector<float>& positionsY,std::vector<float>& velocitiesX,std::vector<float>& velocitiesY,std::vector<sf::CircleShape>& allCircles, std::vector<std::vector<int>>& hashGrid, std::unordered_map<int, std::vector<int>>& hashNeighbors){
    
    //HashGrid:

    for(int y = 0; y < gridSize; y ++){
        std::vector<int> line;
        for(int x = 0; x < gridSize; x++){
            int key = y * gridSize + x;
            line.push_back(key);
            std::cout<<std::endl<<key<<std::endl<<std::endl;
            for(int dx = -1; dx <= 1; dx++){
                for(int dy = -1; dy <= 1; dy++){
                    if(dx + x < 0 || dx + x >= gridSize || dy + y < 0 || dy + y >= gridSize)continue;
                    hashNeighbors[key].push_back((dy + y)*gridSize + (x + dx));
                  //  std::cout<<(dy + y)*gridSize + (x + dx)<<std::endl;
                }
            }    
        }
    }
    
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> c(0,255);
    std::uniform_real_distribution<> x(0, width);
    std::uniform_real_distribution<> y(0,height);
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
            positionsX.push_back(xPosition);
            positionsY.push_back(yPosition);
            velocitiesY.push_back(0);
            velocitiesX.push_back(0);

            //circles positions and color:
            sf::CircleShape circle(particleSize);
            circle.setPosition(xPosition, yPosition);
            circle.setFillColor(allColors[i - 1]);
            allCircles.push_back(circle);
       }
    }    
}



void makeRandomForces(std::vector<std::vector<float>>& forces);
void makeRandomForces(std::vector<std::vector<float>>& forces){
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

void checkCorner(std::vector<float>& positionsX,std::vector<float>& positionsY,std::vector<float>& velocitiesX,std::vector<float>& velocitiesY);
void checkCorner(std::vector<float>& positionsX,std::vector<float>& positionsY,std::vector<float>& velocitiesX,std::vector<float>& velocitiesY){
    for(int i = 0; i < number_of_particles; i++){
        float xPosition = positionsX[i];
        float yPosition = positionsY[i];
        
        //if(xPosition < 0){velocitiesX[i]*=-1; positionsX[i] = 0;}else if(xPosition > width-10){velocitiesX[i] *= -1; positionsX[i] = width ;}
        //if(yPosition < 0){velocitiesY[i] *= -1; positionsY[i] = 0;}else if(yPosition > height-10){velocitiesY[i] *= -1; positionsY[i] = height/2 ;}
        if(xPosition < 0 ){positionsX[i] = width;}else if(xPosition > width){positionsX[i] = 0;}
        if(yPosition <= 0 ){positionsY[i] = height;}else if(positionsY[i] > height){positionsY[i] = 0;}

    }
}

void updateParticles(std::vector<float>& positionsX,std::vector<float>& positionsY,std::vector<float>& velocitiesX,std::vector<float>& velocitiesY, std::vector<sf::CircleShape>& allCircles,std::vector<std::vector<float>>& forces, std::unordered_map<int,std::vector<int>> map, std::unordered_set<int> hashValues, std::unordered_map<int, std::vector<int>> hashNeighbors, int initial, int last);
void updateParticles(std::vector<float>& positionsX,std::vector<float>& positionsY,std::vector<float>& velocitiesX,std::vector<float>& velocitiesY, std::vector<sf::CircleShape>& allCircles,std::vector<std::vector<float>>& forces, std::unordered_map<int,std::vector<int>> map, std::unordered_set<int> hashValues, std::unordered_map<int, std::vector<int>> hashNeighbors, int initial, int last){
    
    //updating velocities:
    auto start = hashValues.begin();
    std::advance(start, initial);
    auto end = hashValues.begin();
    std::advance(end, last);
    for(auto H = start; H != end; ++H){
        //int H = hashValues[i]; 
        std::vector<int> IDS;
        for(auto HV : hashNeighbors[*H]){
            if(hashValues.count(HV) == 0)continue;
            for(auto id : map[HV]){
                IDS.push_back(id);
            }
        }

        for(auto id1 : IDS){
            float totalForceX = 0;
            float totalForceY = 0;
            for(auto id2 : IDS){
                if(id1 == id2) continue;
                float rx = positionsX[id2] - positionsX[id1];   
                float ry = positionsY[id2] - positionsY[id1];
                float r = sqrt(rx*rx + ry*ry);
                if(r > 0 && r < rMax){
                    float f = force(r/rMax,forces[id1 % number_of_colors][id2 % number_of_colors]);
                    totalForceX += rx/r * f;
                    totalForceY += ry/r * f;
                }
            }

            totalForceX *= rMax * forceFactor;
            totalForceY *= rMax * forceFactor;

            velocitiesX[id1] *= frictionFactor;
            velocitiesY[id1] *= frictionFactor;

            velocitiesX[id1] += totalForceX * dt;
            velocitiesY[id1] += totalForceY * dt;

            //updating positions:

            positionsX[id1] += velocitiesX[id1] * dt;
            positionsY[id1] += velocitiesY[id1] * dt;
            allCircles[id1].setRadius(particleSize*zoom*2);
            allCircles[id1].setPosition((positionsX[id1] - xCam)*zoom, (positionsY[id1]-yCam)*zoom);
        } 
    }
}


void updateMap(const std::vector<float> positionsX,const std::vector<float> positionsY, std::unordered_map<int,std::vector<int>>& map, std::unordered_set<int>& hashValues, int initial, int last);
void updateMap(const std::vector<float> positionsX,const std::vector<float> positionsY, std::unordered_map<int,std::vector<int>>& map, std::unordered_set<int>& hashValues, int initial, int last){
    float xSize = width/gridSize;
    //std::cout<<"-------------------------------------"<<std::endl;
    float ySize = height/gridSize;
    for(int i = initial; i < last; i++){
        if(positionsX[i] < 0 || positionsY[i] < 0)continue;
        int gridX = positionsX[i]/xSize;
        int gridY = positionsY[i]/ySize;
        int key = gridY * gridSize + gridX;
        map[key].push_back(i);
      //  std::cout<<positionsX[i]<<" : "<<positionsY[i]<<" - "<<key<<std::endl;
        hashValues.insert(key);
    }
}


void update(sf::RenderWindow &window,std::vector<std::vector<float>>& forces,std::vector<sf::Color>& allColors,std::vector<float>& positionsX,std::vector<float>& positionsY,std::vector<float>& velocitiesX,std::vector<float>& velocitiesY,std::vector<sf::CircleShape>& allCircles, const std::unordered_map<int, std::vector<int>> hashNeighbors);
void update(sf::RenderWindow &window,std::vector<std::vector<float>>& forces,std::vector<sf::Color>& allColors,std::vector<float>& positionsX,std::vector<float>& positionsY,std::vector<float>& velocitiesX,std::vector<float>& velocitiesY,std::vector<sf::CircleShape>& allCircles, const std::unordered_map<int, std::vector<int>> hashNeighbors){
    //this function will update all the informations 
    std::unordered_map<int, std::vector<int>> map; //this map contains all the particles inside an cell 
    std::unordered_set<int> hashValues; //this set will hold all the cells that have at least one particle on it; 
    updateMap(positionsX, positionsY, map, hashValues,0, number_of_particles); //this funtion will update all the positions of the particles on the grid 
    //std::thread u1(updateMap,positionsX, positionsY, std::ref(map), std::ref(hashValues),0, number_of_particles/4);
    //u1.join();
    //std::thread u2(updateMap,positionsX, positionsY, std::ref(map), std::ref(hashValues),number_of_particles/4, number_of_particles/4*2);
    //u2.join();
    //std::thread u3(updateMap,positionsX, positionsY, std::ref(map), std::ref(hashValues),number_of_particles/4*2, number_of_particles/4*3);
    //u3.join();
    //std::thread u4(updateMap,positionsX, positionsY, std::ref(map), std::ref(hashValues),number_of_particles/4*3, number_of_particles/4*4);
    //u4.join();


    checkCorner(positionsX, positionsY, velocitiesX, velocitiesY);
    int size = hashValues.size();
    std::thread t1(updateParticles,std::ref(positionsX), std::ref(positionsY), std::ref(velocitiesX), std::ref(velocitiesY), std::ref(allCircles), std::ref(forces), map, hashValues, hashNeighbors,0,size/4);
    std::thread t2(updateParticles,std::ref(positionsX), std::ref(positionsY), std::ref(velocitiesX), std::ref(velocitiesY), std::ref(allCircles), std::ref(forces), map, hashValues, hashNeighbors,size/4 + 1,size/4*2);
    std::thread t3(updateParticles,std::ref(positionsX), std::ref(positionsY), std::ref(velocitiesX), std::ref(velocitiesY), std::ref(allCircles), std::ref(forces), map, hashValues, hashNeighbors,size/4 * 2 + 1,size/4*3);
    std::thread t4(updateParticles,std::ref(positionsX), std::ref(positionsY), std::ref(velocitiesX), std::ref(velocitiesY), std::ref(allCircles), std::ref(forces), map, hashValues, hashNeighbors,size/4 * 3 + 1,size - 1);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    
    //std::thread t5(updateParticles,std::ref(positionsX), std::ref(positionsY), std::ref(velocitiesX), std::ref(velocitiesY), std::ref(allCircles), std::ref(forces), map, hashValues, hashNeighbors,0,size -1);
    //t5.join();
    //updateParticles(positionsX, positionsY, velocitiesX, velocitiesY, allCircles, forces, map, hashValues, hashNeighbors); //this function updates all the informations of the cells in hashValues
    //std::cout<<0<<"-"<<size/4 * 1<<"-"<<size/4 * 2<<"-"<<size/4 * 3<<"-"<<size/4 * 4<<"-"<<std::endl;

    //drawing the particles:
    for(auto particle : allCircles){
        window.draw(particle);
    }
}

float calculateFPS(sf::Time elapsedTime);
float calculateFPS(sf::Time elapsedTime){
    return 1.0f/elapsedTime.asSeconds();
}


int main(){

    std::vector<std::vector<float>> forces;
    std::vector<sf::Color> allColors;
    std::vector<float> positionsX;
    std::vector<float> positionsY;
    std::vector<float> velocitiesX;
    std::vector<float> velocitiesY;
    std::vector<sf::CircleShape> allCircles;
    std::vector<std::vector<int>> hashGrid ;
    std::unordered_map<int, std::vector<int>> hashNeighbors;
    start_containers(forces,allColors,positionsX,positionsY,velocitiesX,velocitiesY,allCircles, hashGrid, hashNeighbors);

    sf::Clock clock;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "LIFE 2.0", sf::Style::Fullscreen);

    
    while(window.isOpen()){

        sf::Event event;

        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }

            if(event.type == sf::Event::KeyPressed){
                
                if(event.key.code == sf::Keyboard::W){
                    yCam -= 50;
                }else if(event.key.code == sf::Keyboard::S){
                    yCam +=50;
                }
                if(event.key.code == sf::Keyboard::Z){
                    zoom -= 0.1;
                    std::cout<<zoom<<std::endl;
                }else if(event.key.code == sf::Keyboard::X){
                    zoom += 0.1;
                    //xCam += desktop.width*0.9/2;
                    //yCam += desktop.height*0.9/2;
                    std::cout<<zoom<<std::endl;
                }


                if(event.key.code == sf::Keyboard::A){
                    xCam -= 50;
                }else if(event.key.code == sf::Keyboard::D){
                    xCam +=50;
                }
                
                if(event.key.code == sf::Keyboard::P){
                    forceFactor += 0.5;
                    std::cout<<forceFactor<<std::endl;
                }else if(event.key.code == sf::Keyboard::O){
                    forceFactor -= 0.5;
                    std::cout<<forceFactor<<std::endl;                    
                }
                if(event.key.code == sf::Keyboard::L){
                    dt += 0.001;
                    std::cout<<dt<<std::endl;
                }else if(event.key.code == sf::Keyboard::K){
                    if(dt - 0.001 > 0) dt -= 0.001;
                    std::cout<<dt<<std::endl;
                }
                 if(event.key.code == sf::Keyboard::M){
                    rMax += 0.1;
                    std::cout<<rMax<<std::endl;
                }else if(event.key.code == sf::Keyboard::N){
                    if(rMax - 0.1 > 0) rMax -= 0.1;
                    std::cout<<rMax<<std::endl;
                }
                if(event.key.code == sf::Keyboard::H){
                    beta += 0.05;
                    std::cout<<beta<<std::endl;
                }else if(event.key.code == sf::Keyboard::G){
                    if(beta - 0.05 > 0) beta -= 0.05;
                    std::cout<<beta<<std::endl;
                }
            }
            
            sf::Vector2i mouse = sf::Mouse::getPosition(window);

            //std::cout<<mouse.x<<" ::: "<<mouse.y<<std::endl;
            //std::cout<<xCam<<"  ::::  "<<yCam<<std::endl;
            if(mouse.x < 10 && xCam > -20){xCam -= 10*0.5f;}else if(mouse.x > desktop.width - 10 && xCam + desktop.width < width  ){xCam += 10 * 0.5f;}
            if(mouse.y < 10 && yCam > -20){yCam -= 10*0.5f;}else if(mouse.y > desktop.height -10 && yCam + desktop.height < height){yCam += 10 * 0.5f;}
            
            //if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            //    if (event.mouseWheelScroll.delta > 0) {
            //      zoom -= 0.1;
            //    }else{zoom +=0.1;}
            //}
        }
        sf::Time elapsedTime = clock.restart();
        float fps = calculateFPS(elapsedTime);
        std::cout<<"FPS : "<<fps<<std::endl;
        window.clear(sf::Color::Black);

        update(window, forces, allColors,positionsX, positionsY, velocitiesX, velocitiesY, allCircles,hashNeighbors);

        window.display();
    }

    return 0;

}



