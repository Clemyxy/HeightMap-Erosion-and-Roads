#pragma once;

#include "mat.h"
#include "mesh.h"
#include "texture.h"
#include "orbiter.h"
#include "draw.h"
#include "image_io.h"
#include "app_camera.h"        
#include "uniforms.h"
#include "widgets.h"
#include <algorithm>
#include <random>
#include "HF2.hpp"
#include "util.hpp"
#include "roads.hpp"


class HFApp : public App {
private:
    void GenWidget() {
        begin(widgets);
        begin_line(widgets);
        label(widgets, "Times to power stream erode: %d", psTimes);
        begin_line(widgets);
        label(widgets, "x to increase, w to decrease");
        begin_line(widgets);
        label(widgets, "Times to hillslope erode: %d", hsTimes);
        begin_line(widgets);
        label(widgets, "v to increase, c to decrease");
        begin_line(widgets);
        label(widgets, "Times to debris erode: %d", dTimes);
        begin_line(widgets);
        label(widgets, "n to increase, b to decrease");
        begin_line(widgets);
        label(widgets, "Press a for Steepest erosion, z for Normalized erosion");
        begin_line(widgets);
        label(widgets, "Press y to find the road between two random points, press h to input the points yourself in command line");
        end(widgets);
    }
    Mesh getRoadMesh(const std::list<vec2>& l) {
        Mesh tmp(GL_LINES);
        glLineWidth(5);
        tmp.default_color(Red());
        for (auto v : l) {
            tmp.vertex(v.x * hf.GetDivx(), hf.at(v) + 5, v.y* hf.GetDivy());
        }
        for (int i = 0; i < l.size() - 1; ++i) {
            tmp.index(i);
            tmp.index(i + 1);
        }
        return tmp;
    }
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    HFApp(const std::vector<float>& data, const vec2& min, const vec2& max, const int& nx, const int& ny, const float& coe, const float& ms) : App(1024, 640),
                                                                                                               hf(data, min, max, nx, ny),
                                                                                                               nbx(nx), nby(ny), coeff(coe), maxSlope(ms){
        // projection par defaut, adaptee a la fenetre
        m_camera.projection(window_width(), window_height(), 45);
        coeff = std::min(std::max(coeff,0.f), 1.f);
    }

    int init() override {
        widgets = create_widgets();
        Point mesh_pmin;
        Point mesh_pmax;
        m_mesh = hf.GetMesh();
        m_mesh.bounds(mesh_pmin, mesh_pmax);
        m_camera.lookat(mesh_pmin, mesh_pmax);
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClearDepthf(1);
        return 0;
    }
        // dessiner une nouvelle image
    int render() override {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        GenWidget();

        draw(m_mesh, m_camera);
        if(yPressed)
            draw(m_road, m_camera);
        draw(widgets, window_width(), window_height());
        return 1;
    }

    // destruction des objets de l'application
    int quit() override {
        release_widgets(widgets);
        m_mesh.release();
        return 0;
    }

    int prerender() override {
        // recupere les mouvements de la souris
        int mx, my;
        unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);

        // deplace la camera
        if (mb & SDL_BUTTON(1))
            m_camera.rotation(float(mx), float(my));      // tourne autour de l'objet
        else if (mb & SDL_BUTTON(3))
            m_camera.translation((float) mx / (float) window_width(),
                                 (float) my / (float) window_height()); // deplace le point de rotation
        else if (mb & SDL_BUTTON(2))
            m_camera.move(float(mx));           // approche / eloigne l'objet

        SDL_MouseWheelEvent wheel = wheel_event();
        if (wheel.y != 0) {
            clear_wheel_event();
            m_camera.move(8.f * float(wheel.y));  // approche / eloigne l'objet
        }
        if (key_state('z') || key_state('a')) {
            yPressed = false;
        }
        if (key_state('z')) {
            clear_key_state('z');

            for (int i = 0; i < psTimes; ++i) {
                hf.StreamPowerErode(NeighbouringType::Steepest, 1);
                std::cout << "Stream Power Erosion " << i << " done." << std::endl;
            }
            for (int i = 0; i < hsTimes; ++i) {
                hf.HillslopeErode(1);
                std::cout << "HillSlope Erosion " << i << " done." << std::endl;
            }
            for (int i = 0; i < dTimes; ++i) {
                hf.DebrisErode(1);
                std::cout << "Debris Erosion " << i << " done." << std::endl;
            }
            m_mesh.release();
            m_mesh = hf.GetMesh();
        }
        if (key_state('a')) {
            clear_key_state('a');
            for (int i = 0; i < psTimes; ++i) {
                hf.StreamPowerErode(NeighbouringType::Normalized, 1);
                std::cout << "Steam Power Erosion " << i << " done." << std::endl;
            }
            for (int i = 0; i < hsTimes; ++i) {
                hf.HillslopeErode(1);
                std::cout << "HillSlope Erosion " << i << " done." << std::endl;
            }
            for (int i = 0; i < dTimes; ++i) {
                hf.DebrisErode(1);
                std::cout << "Debris Erosion " << i << " done." << std::endl;
            }
            m_mesh.release();
            m_mesh = hf.GetMesh();
        }
        if (key_state('e')) {
            clear_key_state('e');
            SF2 s = hf.S();
            hf.ApplySF(m_mesh,s,Color(0,0,1));
        }
        if (key_state('r')) {
            clear_key_state('r');
            SF2 A = hf.A(NeighbouringType::Steepest);
            hf.ApplySF(m_mesh,A,Color(0,1,0));
        }
        if (key_state('t')) {
            clear_key_state('t');
            SF2 A = hf.A(NeighbouringType::Normalized);
            hf.ApplySF(m_mesh,A,Color(0,1,0));
        }
        if (key_state('x')) {
            clear_key_state('x');
            psTimes++;
        }
        if (key_state('w')) {
            clear_key_state('w');
            psTimes = std::max(0,psTimes - 1);
        }
        if (key_state('v')) {
            clear_key_state('v');
            hsTimes++;
        }
        if (key_state('c')) {
            clear_key_state('c');
            hsTimes = std::max(0,hsTimes - 1);
        }
        if (key_state('n')) {
            clear_key_state('n');
            dTimes++;
        }
        if (key_state('b')) {
            clear_key_state('b');
            dTimes = std::max(0,dTimes - 1);
        }
        if (key_state('d')) {
            clear_key_state('d');
            Util::WriteSF(hf.S(), "Slope.png", nbx, nby);
            std::cout << "Slope.png Created" << std::endl;
        }
        if (key_state('f')) {
            clear_key_state('f');
            Util::WriteSF(hf.A(NeighbouringType::Steepest), "SteepestDrain.png", nbx, nby);
            std::cout << "SteepestDrain.png Created" << std::endl;
        }
        if (key_state('g')) {
            clear_key_state('g');
            Util::WriteSF(hf.A(NeighbouringType::Normalized), "NormalizedDrain.png", nbx, nby);
            std::cout << "NormalizedDrain.png Created" << std::endl;
        }
        if (key_state('y')) {
            clear_key_state('y');
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> distx(0,nbx-1);
            std::uniform_int_distribution<std::mt19937::result_type> disty(0,nby-1);
            float sx, sy, ex, ey;
            do {
                sx = distx(rng);
                sy = disty(rng);
                ex = distx(rng);
                ey = disty(rng);
            } while (sx == ex && sy == ey);
            
            Graph road((SF2)hf,coeff,maxSlope);
            std::cout << std::endl << "Going from Point (" << sx << ", " << sy << ") to Point (" << ex << ", " << ey << ")." << std::endl << std::endl;
            std::list<vec2> path = road.Dijkstra(vec2(sx, sy), vec2(ex, ey));
            m_road = getRoadMesh(path);
            yPressed = true;   
        }
        if (key_state('h')) {
            clear_key_state('h');
            float sx, sy, ex, ey;
            do {
                std::cout << "Enter source x coordinate (Must be in terrain bounds)" << std::endl;
                std::cin >> sx;
                std::cout << "Enter source y coordinate (Must be in terrain bounds)" << std::endl;
                std::cin >> sy;
                std::cout << "Enter target x coordinate (Must be in terrain bounds)" << std::endl;
                std::cin >> ex;
                std::cout << "Enter target y coordinate (Must be in terrain bounds)" << std::endl;
                std::cin >> ey;
                if (sx == ex && sy == ey)
                    std::cout << "Error, source point is the same as the target point, please re enter positions" << std::endl;
            }while (sx == ex && sy == ey);
            
            Graph road((SF2)hf,coeff,maxSlope);
            std::cout << std::endl << "Going from Point (" << sx << ", " << sy << ") to Point (" << ex << ", " << ey << ")." << std::endl << std::endl;
            std::list<vec2> path = road.Dijkstra(vec2(sx, sy), vec2(ex, ey));
            m_road = getRoadMesh(path);
            yPressed = true;   
        }
        // appelle la fonction update() de la classe derivee
        return update(global_time(), delta_time());
    }

protected:
    Mesh m_mesh, m_road;
    Orbiter m_camera;
    GLuint vao = 0;
    GLuint m_program = 0;
    Widgets widgets;
    int nbx, nby;
    int psTimes = 1;
    int dTimes = 1;
    int hsTimes = 1;
    bool yPressed = false;
    float coeff,maxSlope;
    HF2 hf;
};

int main(int argc, char **argv) {
    std::cout << argc << std::endl;
    if (argc <= 1 || argc > 4) {
        std::cout << argc << std::endl;
        std::cout << "Bad arguments, input the path to your heightmap as the first argument"
            << ", how much you want of the slope to count instead of the distance (range 0-1) as the second argument (optional)"
            << "and the max slope permitted (1 = 45 degrees) as the last argument(optional), choosing a max slope too low can make the algorithm fail and/or overload your ram" << std::endl;
        return -1;
    }
    float maxS, coeff;
    if (argc == 2) {
        maxS = 100.0;
        coeff = 0.5;
    }
    else if (argc == 3) {
        maxS = 1.0;
        coeff = atof(argv[2]);
    }
    else {
        maxS = atof(argv[3]);
        coeff = atof(argv[2]);
    }
    Image img = read_image(argv[1]);
    std::vector<float> imgGray = Util::RGBtoHeight(img);
    
    HFApp hfApp(imgGray,vec2(0,0),vec2(2000,2000),img.width(),img.height(), coeff,maxS);
    hfApp.run();

    return 0;
}