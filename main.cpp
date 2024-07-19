#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <cmath>

int height = 700, width = 700;
float T = 1;
float t = T*0.016f;
float restituition = 1;
bool solid_corner = false;
float factor = 0.1;
void update(sf::RenderWindow &window);   

struct Particle{
    sf::Color color;
    float x;
    float y;
    float x_v = 0, y_v = 0;
    float x_a = 0, y_a = 0;
    float mass;
    int id;
    int size;
    sf::CircleShape shape;
    Particle(float x_, float y_, int size_,int id_,float mass_,sf::Color color_ )
        :x(x_), y(y_), color(color_),size(size_),id(id_),mass(mass_),shape(size_){
            shape.setFillColor(color);
            shape.setPosition(x,y);
    }
    void setPosition(float x_, float y_) {
        x = x_;
        y = y_;
        shape.setPosition(x, y);
    }
};

std::vector<Particle> create_group(int numberParticles,int size,float mass,sf::Color);
Particle create_particle(int size,float mass,int id,sf::Color color);
void rule(std::vector<Particle> &g1, std::vector<Particle> &g2, float G, float min_distance = 0,float  max_distance = 80);
float distance(Particle p1, Particle p2);
void check_corner(Particle &p);
void switch_corner(Particle &p);
int size_cell = 1;
//int numberParticles,int size, float mass,sf::Color color
std::vector<Particle> yellow = create_group(200,size_cell,1, sf::Color::Yellow);
std::vector<Particle> red = create_group(200,size_cell,1, sf::Color::Red);
std::vector<Particle> blue = create_group(100,size_cell,1, sf::Color::Blue);
std::vector<Particle> green = create_group(200,size_cell,1, sf::Color::Green);

int main(){

    sf::RenderWindow window(sf::VideoMode(width, height), "LIFE");
//      window.setFramerateLimit(60);

    while(window.isOpen()){

        sf::Event event;

        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }

            if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::S){
                    solid_corner = true;
                    std::cout<<"SOLID"<<std::endl;
                }
                if(event.key.code == sf::Keyboard::D){
                    solid_corner = false;
                    std::cout<<"NOT SOLID"<<std::endl;
                }
                if(event.key.code == sf::Keyboard::W){
                    factor+=0.05;
                    std::cout<<factor<<std::endl;
                }else if(event.key.code == sf::Keyboard::E){
                    factor-=0.05;
                    std::cout<<factor<<std::endl;
                }
            }

        }

        window.clear(sf::Color::Black);

        update(window);

        window.display();

    }

    return 0;
}


void update(sf::RenderWindow &window){
    
    //rule(yellow, red, 1);
    //rule(red,yellow, 1);
    //rule(yellow, yellow, 0);
    ////rule(blue, red , -1);
    //rule(blue, yellow, -2);
    //rule(yellow, blue, -2 );
    //rule(blue, red , 1);
    //rule(red, blue, 1);
    //rule(green, blue, 1.5);
    //rule(blue, green, 1.5);
    //rule(green, red, -0.7);
    //rule(red,green, -0.7);
    //rule(green,  yellow, 1);
    //rule(red, red, 1);
    //rule(red, red, 1);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> g(-100,100);
    std::uniform_int_distribution<> dis(0, 1);

/*
    rule(green, green, -0.32,100, 120);
    rule(green, red, -0.17);
    rule(green, yellow, 0.34);
    rule(red, red, -0.10, 50, 80);
    rule(red, green, -0.34);
    rule(yellow, yellow, 0.15, 30);
    rule(yellow, green, -0.20);
*/
/*
    rule(yellow, yellow, -0.3, 50, 80);
    rule(yellow, yellow, 0.1, 20);
    rule(red, red, -0.3, 40, 70);
    rule(red, red, 0.1);
    rule(yellow, red, -0.6, 30);
    rule(red, yellow, 0.3, 30);
*/

    rule(yellow, yellow, -0.9, 50, 80);
    rule(yellow, yellow, 0.2);
    rule(red, red, -0.3, 30, 70);
    rule(red, red, 0.1);
    rule(yellow, red, -0.6, 30);
    rule(red, yellow, 0.3, 30);

    rule(green, red, 0.7);
    rule(red, green, -0.2);
    rule(green, green, -0.2, 20, 80);
    rule(green, green, 0.1, 10, 30);
    rule(green, yellow, 0.1);
    rule(yellow, green, -0.2);

    rule(blue, yellow, -0.35, 10, 100);
    rule(yellow, blue, 0.2, 10);
    rule(blue, blue, 0.1);
    //rule(blue, green,0.1,10);
    //rule(green, blue, -0.2);
//    rule(green, green, 0.4);
//    rule(blue, blue, -0.3, 40, 200);
//    rule(blue, blue, 0.1);
//    rule(green, blue, -0.6, 10);
//    rule(blue, green, 0.3, 10);

    //rule(green,yellow, 0.2);
    //rule(green,red, 0.1);
    //rule(red, green, -0.2, 10, 100);
    //rule(red, green, -0.2, 10);
    //rule(green, green,-0.1, 20, 80);
    int randomValue;

/*
    randomValue = dis(gen) == 0 ? -1 : 1;
    rule(green,green, g(gen)*0.01 );

    randomValue = dis(gen) == 0 ? -1 : 1;
    rule(green, red, g(gen)*0.01 );

    randomValue = dis(gen) == 0 ? -1 : 1;
    rule(green, yellow,g(gen)*0.01 );

    randomValue = dis(gen) == 0 ? -1 : 1;
    rule(red, red, g(gen)*0.01 );

    randomValue = dis(gen) == 0 ? -1 : 1;
    rule(red, green, g(gen)*0.01 );

    randomValue = dis(gen) == 0 ? -1 : 1;
    rule(yellow, yellow, g(gen)*0.01 );

    randomValue = dis(gen) == 0 ? -1 : 1;
    rule(yellow, green, g(gen)*0.01 * randomValue );

*/

    for (auto &p : yellow){
        //check_corner(p);
        window.draw(p.shape);
    }
    
    for (auto &p : red){
        //check_corner(p);
        window.draw(p.shape);
    }
    for(auto &p : green){
        window.draw(p.shape);
    }
    for(auto &p : blue){
        window.draw(p.shape);
    }
    /*
    for (auto &p : blue){
        check_corner(p);
        window.draw(p.shape);
    }
    for (auto &p : green){
        check_corner(p);
        window.draw(p.shape);
    }
    */
}

void switch_corner(Particle &p){
    if(p.x < 0){
        p.x = width - 2*p.size;
        p.x_v = restituition*p.x_v;
        }else if(p.x > width - 2*p.size){
            p.x = 0;
            p.x_v = restituition*p.x_v;
        }
    if(p.y < 0){
        p.y = height - 2*p.size;
        p.y_v = restituition*p.y_v;
        }else if(p.y > height - 2*p.size){
            p.y = 0;
            p.y_v = restituition*p.y_v;
        }
}

void check_corner(Particle &p){
    if(p.x < 0){
        p.x = 0;
        p.x_v = -restituition*p.x_v;
        }else if(p.x > width - 2*p.size){
            p.x = width - 2*p.size;
            p.x_v = -restituition*p.x_v;
        }
    if(p.y < 0){
        p.y = 0;
        p.y_v = -restituition*p.y_v;
        }else if(p.y > height - 2*p.size){
            p.y = height - 2*p.size;
            p.y_v = -restituition*p.y_v;
        }
    
}

void rule(std::vector<Particle> &g1, std::vector<Particle> &g2, float G, float min_distance, float max_distance){
    for(auto &p1 : g1){
        for(auto &p2 : g2){
            if(p1.color == p2.color && p1.id == p2.id){
                continue;}
           
            float fx = 0, fy = 0;
            float dx = p1.x - p2.x;
            float dy = p1.y - p2.y;
            float D = sqrt((dx * dx + dy*dy));

            if( D > min_distance && D < max_distance){
                float F = G/D;
                fx += F*dx;
                fy += F*dy;     
            }
            
            p1.x_v = (p1.x_v+fx)*factor ;
            p1.y_v = (p1.y_v+fy)*factor ;
            p1.x += p1.x_v;
            p1.y += p1.y_v;

            if(solid_corner){check_corner(p1);}else{switch_corner(p1);}
           
            p1.setPosition(p1.x, p1.y);

        }
    }
}

std::vector<Particle> create_group(int numberParticles,int size, float mass,sf::Color color){
    //Each group must contain an number of particles, and an color and 
    //Each particle must contain x,y components and an color, and an shape, and an mass [[[[[CHECKED]]]]]
    std::vector<Particle> group;

    for(int i = 0; i < numberParticles; i++){
        Particle p = create_particle(size,mass,i,color);
        group.push_back(p); 
    }
    return group;
} 
float distance(Particle p1, Particle p2){
    return (sqrt( (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) ));
}

Particle create_particle(int size,float mass, int id,sf::Color color){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> x_p(1,width-2*size);
    std::uniform_int_distribution<> y_p(1, height-2*size);
    Particle p(x_p(gen),y_p(gen),size,id,mass,color);
    return p;
}


