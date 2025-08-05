/*******************************************************************************************
*
*   olcPixelGameEngine example - EnTT Bouncing Balls
*   
********************************************************************************************
*   
*   This is free and unencumbered software released into the public domain.
*   
*   Anyone is free to copy, modify, publish, use, compile, sell, or
*   distribute this software, either in source code form or as a compiled
*   binary, for any purpose, commercial or non-commercial, and by any
*   means.
*   
*   In jurisdictions that recognize copyright laws, the author or authors
*   of this software dedicate any and all copyright interest in the
*   software to the public domain. We make this dedication for the benefit
*   of the public at large and to the detriment of our heirs and
*   successors. We intend this dedication to be an overt act of
*   relinquishment in perpetuity of all present and future rights to this
*   software under copyright law.
*   
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
*   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
*   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*   OTHER DEALINGS IN THE SOFTWARE.
*   
*   For more information, please refer to <https://unlicense.org>
*
********************************************************************************************/
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "entt.hpp"
#include <random>
#include <vector>

// Components for EnTT
struct Position
{
    float x, y;
};

struct Velocity
{
    float dx, dy;
};

struct Ball
{
    float radius;
    float mass; // For collision physics
};

struct Color
{
    olc::Pixel color;
};

// Main game class
class BouncingBalls : public olc::PixelGameEngine
{
public:
    BouncingBalls()
    {
        sAppName = "Bouncing Balls";
    }

private:
    entt::registry registry;
    std::mt19937 rng{std::random_device{}()};
    static constexpr int numBalls = 50;
    static constexpr float minRadius = 5.0f;
    static constexpr float maxRadius = 15.0f;
    static constexpr float maxSpeed = 300.0f;

    // Initialize balls with random properties
    void InitializeBalls()
    {
        std::uniform_real_distribution<float> posX(0.0f, (float)ScreenWidth());
        std::uniform_real_distribution<float> posY(0.0f, (float)ScreenHeight());
        std::uniform_real_distribution<float> vel(-maxSpeed, maxSpeed);
        std::uniform_real_distribution<float> radius(minRadius, maxRadius);
        std::uniform_int_distribution<int> color(0, 255);

        for(int i = 0; i < numBalls; ++i)
        {
            auto entity = registry.create();
            float r = radius(rng);
            registry.emplace<Position>(entity, posX(rng), posY(rng));
            registry.emplace<Velocity>(entity, vel(rng), vel(rng));
            registry.emplace<Ball>(entity, r, r * r); // Mass proportional to area
            registry.emplace<Color>(entity, olc::Pixel(color(rng), color(rng), color(rng)));
        }
    }

    // Physics system: Update positions and handle wall collisions
    void UpdatePhysics(float fElapsedTime)
    {
        registry.view<Position, Velocity, Ball>().each([&](auto entity, Position& pos, Velocity& vel, Ball& ball)
        {
            // Update position
            pos.x += vel.dx * fElapsedTime;
            pos.y += vel.dy * fElapsedTime;

            // Bounce off walls
            if (pos.x - ball.radius < 0) {
                pos.x = ball.radius;
                vel.dx = -vel.dx;
            } else if (pos.x + ball.radius > ScreenWidth()) {
                pos.x = ScreenWidth() - ball.radius;
                vel.dx = -vel.dx;
            }
            if (pos.y - ball.radius < 0) {
                pos.y = ball.radius;
                vel.dy = -vel.dy;
            } else if (pos.y + ball.radius > ScreenHeight()) {
                pos.y = ScreenHeight() - ball.radius;
                vel.dy = -vel.dy;
            }
        });

        // Handle ball-ball collisions
        auto view = registry.view<Position, Velocity, Ball>();
        std::vector<std::pair<entt::entity, entt::entity>> collisions;
        view.each([&](auto e1, Position& p1, Velocity& v1, Ball& b1)
        {
            view.each([&](auto e2, Position& p2, Velocity& v2, Ball& b2)
            {
                if (e1 >= e2) return; // Avoid duplicate pairs
                
                float dx = p2.x - p1.x;
                float dy = p2.y - p1.y;
                float distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance < b1.radius + b2.radius)
                {
                    collisions.emplace_back(e1, e2);
                }
            });
        });

        // Resolve collisions (elastic collision physics)
        for (auto [e1, e2] : collisions)
        {
            auto& p1 = registry.get<Position>(e1);
            auto& v1 = registry.get<Velocity>(e1);
            auto& b1 = registry.get<Ball>(e1);
            auto& p2 = registry.get<Position>(e2);
            auto& v2 = registry.get<Velocity>(e2);
            auto& b2 = registry.get<Ball>(e2);

            // Normal vector
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist == 0) return; // Avoid division by zero
            
            float nx = dx / dist;
            float ny = dy / dist;

            // Relative velocity
            float dvx = v2.dx - v1.dx;
            float dvy = v2.dy - v1.dy;
            float dot = dvx * nx + dvy * ny;

            // Impulse (elastic collision)
            float impulse = 2.0f * dot / (b1.mass + b2.mass);
            v1.dx += impulse * b2.mass * nx;
            v1.dy += impulse * b2.mass * ny;
            v2.dx -= impulse * b1.mass * nx;
            v2.dy -= impulse * b1.mass * ny;

            // Separate overlapping balls
            float overlap = (b1.radius + b2.radius - dist) * 0.5f;
            p1.x -= overlap * nx;
            p1.y -= overlap * ny;
            p2.x += overlap * nx;
            p2.y += overlap * ny;
        }
    }

    // Render system: Draw balls
    void RenderBalls()
    {
        Clear(olc::BLACK);
        registry.view<Position, Ball, Color>().each([&](Position& pos, Ball& ball, Color& color)
        {
            FillCircle((int32_t)pos.x, (int32_t)pos.y, (int32_t)ball.radius, color.color);
        });
    }

public:
    bool OnUserCreate() override
    {
        InitializeBalls();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // clamp time to a maximum of ~30 fps, for when the simulation goes out of focus
        fElapsedTime = std::clamp(fElapsedTime, 0.0f, 0.033f);
        UpdatePhysics(fElapsedTime);
        RenderBalls();
        return true;
    }
};

int main()
{
    BouncingBalls game;
    if (game.Construct(800, 600, 1, 1))
        game.Start();
    return 0;
}
