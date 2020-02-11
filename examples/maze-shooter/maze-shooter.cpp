#include "maze-shooter.hpp"

using namespace blit;

constexpr auto SCREEN_WIDTH = 320;
constexpr auto SCREEN_HEIGHT = 240;
constexpr auto TILE_SIZE = 10;

constexpr auto TILES_X = SCREEN_WIDTH / TILE_SIZE;
constexpr auto TILES_Y = SCREEN_HEIGHT / TILE_SIZE;

enum directionMask {
    NORTH = 1,
    SOUTH = 2,
    WEST = 4,
    EAST = 8,
    NORTH_WEST = 16,
    NORTH_EAST = 32,
    SOUTH_WEST = 64,
    SOUTH_EAST = 128
};

struct Player {
    Vec2 position = Vec2(100, 100);
    directionMask facing = directionMask::SOUTH_WEST;
};

Player player;

uint32_t maze[24] = {
    0b11111111111111111111111111111111,
    0b10000000000000001000000000000001,
    0b10000000000000001011110000000001,
    0b10000000000000001010000011001101,
    0b10000000000000001010000000000001,
    0b10000000000000001000000000000001,
    0b10000000000000001000101100000001,
    0b10000000000000001000000000000101,
    0b10000000000000001000000000000101,
    0b10000000000000001011111011111101,
    0b10000000000000001000000000000001,
    0b10000000000000001111111111111111,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b10000000000000000000000000000001,
    0b11111111111111111111111111111111
};

void init() {
    set_screen_mode(ScreenMode::hires);
}

bool standing(Vec2 position) {
    auto rowAt = maze[uint16_t(position.y / TILE_SIZE)];
    auto colMask = 0x80000000 >> uint16_t(position.x / TILE_SIZE);

    return rowAt & colMask;
}

uint8_t adjacent_tiles(Vec2 position) { 
    uint8_t nearbyTiles;

    nearbyTiles |= directionMask::EAST;

    auto rowAbove = maze[uint16_t(position.y / TILE_SIZE) - 1];
    auto rowAt = maze[uint16_t(position.y / TILE_SIZE)];
    auto rowBelow = maze[uint16_t(position.y / TILE_SIZE) + 1];

    auto colMask = 0x80000000 >> uint16_t(position.x / TILE_SIZE);

    if (rowAbove & colMask) {
        nearbyTiles |= directionMask::NORTH;
    }
    if (rowBelow & colMask) {
        nearbyTiles |= directionMask::NORTH;
    }
    if (rowAt & ((colMask << 1) & 0xffffffff)) {
        nearbyTiles |= directionMask::WEST;
    }
    if (rowAt & (colMask >> 1)) {
        nearbyTiles |= directionMask::EAST;
    }
    if (rowAbove & ((colMask << 1) & 0xffffffff)) {
        nearbyTiles |= directionMask::NORTH_WEST;
    }
    if (rowAbove & (colMask >> 1)) {
        nearbyTiles |= directionMask::NORTH_EAST;
    }
    if (rowBelow & ((colMask << 1) & 0xffffffff)) {
        nearbyTiles |= directionMask::SOUTH_WEST;
    }
    if (rowBelow & (colMask >> 1)) {
        nearbyTiles |= directionMask::SOUTH_EAST;
    }

    return nearbyTiles;
}

void update(uint32_t time) {
    static uint32_t lastButtons = 0;
    static uint32_t changed = buttons ^ lastButtons;

    Vec2 move;

    if(buttons & Button::DPAD_LEFT) {
        move.x = -1;
        player.facing = directionMask::WEST;
    }
    if(buttons & Button::DPAD_RIGHT) {
        move.x = 1;
        player.facing = directionMask::EAST;
    }
    if (standing(player.position + move)) {
        move.x = 0;
    }

    if(buttons & Button::DPAD_UP) {
        move.y = -1;
        if(player.facing == directionMask::WEST) {
            player.facing = directionMask::NORTH_WEST;
        }
        else if(player.facing == directionMask::EAST) {
            player.facing = directionMask::NORTH_EAST;
        }
        else
        {
            player.facing = directionMask::NORTH;
        }
    }
    if(buttons & Button::DPAD_DOWN) {
        move.y = 1;
        if(player.facing == directionMask::WEST) {
            player.facing = directionMask::SOUTH_WEST;
        }
        else if(player.facing == directionMask::EAST) {
            player.facing = directionMask::SOUTH_EAST;
        }
        else
        {
            player.facing = directionMask::SOUTH;
        }
    }
    if (standing(player.position + move)) {
        move.y = 0;
    }

    player.position += move;

    lastButtons = buttons;
}

void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    for (auto y = 0; y < TILES_Y; y++) {
        auto maze_row = maze[y];
        auto py = y * TILE_SIZE;
        for (auto x = 0; x < TILES_X; x++) {
            auto px = x * TILE_SIZE;
            auto maze_col = (maze_row & (0x80000000 >> x)) > 0;
            if (maze_col) {
                screen.pen = Pen(200, 200, 255);
                screen.rectangle(Rect(px, py, TILE_SIZE, TILE_SIZE));
            }
        }
    }

    Point draw_player_at = Point(player.position.x, player.position.y);

    screen.pen = Pen(128, 128, 0);
    switch(player.facing){
        case directionMask::NORTH:
        case directionMask::SOUTH:
            screen.rectangle(Rect(draw_player_at.x - 3, draw_player_at.y - 2, 6, 3));
            break;
        case directionMask::WEST:
        case directionMask::EAST:
            screen.rectangle(Rect(draw_player_at.x - 2, draw_player_at.y - 3, 3, 6));
            break;
        case directionMask::NORTH_EAST:
        case directionMask::SOUTH_WEST:
            screen.line(
                Point(draw_player_at.x - 2, draw_player_at.y - 2),
                Point(draw_player_at.x + 2, draw_player_at.y + 2));
            screen.line(
                Point(draw_player_at.x - 2, draw_player_at.y - 1),
                Point(draw_player_at.x + 1, draw_player_at.y + 2));
            screen.line(
                Point(draw_player_at.x - 1, draw_player_at.y - 2),
                Point(draw_player_at.x + 2, draw_player_at.y + 1));
            break;
        break;
        case directionMask::NORTH_WEST:
        case directionMask::SOUTH_EAST:
            screen.line(
                Point(draw_player_at.x - 2, draw_player_at.y + 2),
                Point(draw_player_at.x + 2, draw_player_at.y - 2));
            screen.line(
                Point(draw_player_at.x - 2, draw_player_at.y + 1),
                Point(draw_player_at.x + 1, draw_player_at.y - 2));
            screen.line(
                Point(draw_player_at.x - 1, draw_player_at.y + 2),
                Point(draw_player_at.x + 2, draw_player_at.y - 1));
            break;
    }

    screen.pen = Pen(255, 255, 0);
    screen.rectangle(Rect(draw_player_at.x - 1, draw_player_at.y - 1, 2, 2));

    screen.pen = Pen(255, 255, 0);
    switch(player.facing) {
        case directionMask::NORTH:
            screen.rectangle(Rect(draw_player_at.x, draw_player_at.y - 4, 1, 2));
            break;
        case directionMask::SOUTH:
            screen.rectangle(Rect(draw_player_at.x - 1, draw_player_at.y + 1, 1, 2));
            break;
    }
}