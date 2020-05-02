#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <iterator> 
#include <ctime>
#include <cstdlib>


struct Vec2D {
    int x, y;
    Vec2D() {
        x = 0;
        y = 0;
    }
    Vec2D(int nx, int ny) {
        x = nx;
        y = ny;
    }
    std::string to_string() {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
    Vec2D& operator+= (const Vec2D& v) {
        this->x += v.x;
        this->y += v.y;
        return (*this);
    }
    Vec2D& operator+ (const Vec2D& v) {
        this->x = this->x + v.x;
        this->y = this->y + v.y;
        return (*this);
    }
    Vec2D& operator- (const Vec2D& v) {
        this->x = this->x - v.x;
        this->y = this->y - v.y;
        return (*this);
    }
    Vec2D& operator* (const int& n) {
        this->x = this->x * n;
        this->y = this->y * n;
        return (*this);
    }

};

Vec2D operator* (int n, Vec2D v) {
    return v * n;
}

bool operator== (const Vec2D& v1, const Vec2D& v2) {
    return (v1.x == v2.x && v1.y == v2.y);
}

class Particle {
public:
    Particle(Vec2D pos, Vec2D vel = Vec2D(0,1), int m = 1) {
        position = pos;
        velocity = vel;
        mass = m;
    }
    Vec2D getPosition() {
        return position;
    }
    void setPosition(Vec2D newPos) {
        position = newPos;
    }
    Vec2D getVelocity() {
        return velocity;
    }
    void setVelocity(Vec2D newVel) {
        velocity = newVel;
    }
    void update(int deltaTime) {
        position += deltaTime * velocity;
    }
private:
    Vec2D position;
    Vec2D velocity;
    int mass;

};

class Game {
public:
    std::map<int, std::map<int, char>> board;
    std::vector<Particle*> particles;
    std::vector<std::vector<Particle*>*> collisions;
    Game(int xSize, int ySize, char emptyBoard) {
        // store board sizes
        width = xSize;
        height = ySize;
        // populates board with empty chars
        for (int i = 0; i < height; i++) {
            std::map<int, char> tempBoard = std::map<int, char>();
            for (int j = 0; j < width; j++) {
                tempBoard[j] = emptyBoard;
            }
            board[i] = tempBoard;
        }

    }
    bool hasCollided(Particle* p) {
        for (int i = 0; i < collisions.size(); i++) {
            for (int j = 0; j < (*collisions[i]).size(); j++) {
                if (p == (*collisions[i])[j]) {
                    return true;
                }
            }
        }
        return false;
    }
    void addParticle(Particle* p) {
        particles.push_back(p);
    }
    void detectCollisions() {
        std::vector<Particle*> particlesCopy = particles;
        for (int i = 0; i < particlesCopy.size(); i++) {
            Particle* p = particlesCopy[i];
            std::vector<Particle*>* collision = new std::vector<Particle*>();
            for (int j = 0; j < particlesCopy.size(); j++) {
                Particle* o = particlesCopy[j];
                if (p != o) {
                    if (p->getPosition() == o->getPosition()) {
                        collision->push_back(o);
                        std::remove(particlesCopy.begin(), particlesCopy.end(), o);
                    }
                }
            }
            if (collision->size() > 0) {
                collision->push_back(p);
                std::remove(particlesCopy.begin(), particlesCopy.end(), p);
            }
            collisions.push_back(collision);
        }
    }
    void moveParticles(int deltaTime) {
        for (int i = 0; i < particles.size(); i++) {
            Particle* p = particles[i];
            if (p->getPosition().x + deltaTime * p->getVelocity().x < width &&
                p->getPosition().x + deltaTime * p->getVelocity().x > 0 &&
                p->getPosition().y + deltaTime * p->getVelocity().y < height &&
                p->getPosition().y + deltaTime * p->getVelocity().y > 0 &&
                !hasCollided(p)) {
                p->update(deltaTime);
            }
        }
    }
    std::vector<Particle*>* findCollider(Particle* p) {
        for (int i = 0; i < collisions.size(); i++) {
            for (int j = 0; j < (*collisions[i]).size(); j++) {
                if (p == (*collisions[i])[j]) {
                    return collisions[i];
                }
            }
        }
        return nullptr;
    }
    void removeColliders(std::vector<Particle*>* v) {
        std::remove(collisions.begin(), collisions.end(), v);
    }
    void resolveCollisions(int deltaTime) {
        for (int i = 0; i < particles.size(); i++) {
            Particle* p = particles[i];
            if (hasCollided(p)) {
                std::srand(std::time(nullptr));
                Vec2D oldPos = p->getPosition() + (-deltaTime) * p->getVelocity();
                p->setPosition(oldPos);
                Vec2D oldVel = Vec2D(0, 0) - p->getVelocity();
                p->setVelocity(oldVel);
                removeColliders(findCollider(p));
            }
        }
    }
    
    char getTile(Vec2D v) {
        return board[v.y][v.x];
    }
    void setTile(Vec2D v, char c) {
        board[v.y][v.x] = c;
    }
    void printBoard() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                std::cout << board[i][j];
            }
            std::cout << std::endl;
        }
    }
    void update() {
        updateParticles();
        updateBoard();
        collisions.clear();
    }
    void updateParticles() {
        int deltaTime = 1;
        detectCollisions();
        moveParticles(deltaTime);
        resolveCollisions(deltaTime);
    }
    void updateBoard() {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                setTile(Vec2D(i, j), ' ');
            }
        }
        for (int i = 0; i < particles.size(); i++) {
            setTile(particles[i]->getPosition(), '#');
        }
    }
private:
    int width, height;
};

int main() {
    std::system("CLS");
    Game* game = new Game(25, 25, ' ');
    game->addParticle(new Particle(Vec2D(2, 4)));
    game->addParticle(new Particle(Vec2D(10, 20)));
    game->addParticle(new Particle(Vec2D(1, 7)));
    game->addParticle(new Particle(Vec2D(10, 18)));
    game->addParticle(new Particle(Vec2D(1, 6)));
    game->addParticle(new Particle(Vec2D(1, 22)));

    while (true) {
        game->update();
        game->printBoard();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::system("CLS");
    }
}