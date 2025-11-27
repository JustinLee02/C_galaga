#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bcm2835.h>
#include "st7789.h"
#include "input.h"

#define PLAYER_Y        200
#define PLAYER_SPEED    6
#define MISSILE_SPEED   12
#define MISSILE_COLOR   COLOR_YELLOW

#define ENEMY_COUNT     15
#define ENEMY_MAX_SPEED 3

int PLAYER_WIDTH, PLAYER_HEIGHT;
int ENEMY_WIDTH, ENEMY_HEIGHT;

uint16_t* player_img = NULL;
uint16_t* enemy_img = NULL;

typedef struct {
    int x, y;
} Player;

typedef struct {
    int x, y, active;
} Missile;

typedef struct {
    int x, y;
    int vx, vy;
    int active;
} Enemy;

int CheckCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    if (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2) return 1;
    return 0;
}

enum { STATE_PLAYING, STATE_GAMEOVER, STATE_WIN };

int main(void) {
    if (!bcm2835_init()) return 1;

    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    srand(time(NULL));

    ST7789_Init();
    Input_Init();

    player_img = ST7789_LoadImage("player.png", &PLAYER_WIDTH, &PLAYER_HEIGHT);
    enemy_img  = ST7789_LoadImage("enemy.png", &ENEMY_WIDTH, &ENEMY_HEIGHT);

    if (!player_img || !enemy_img) {
        printf("Error: Image files not found!\n");
        return 1;
    }

    Player player;
    player.x = (ST7789_WIDTH - PLAYER_WIDTH) / 2;
    player.y = PLAYER_Y;

    Missile missiles[10];
    for(int i=0; i<10; i++) missiles[i].active = 0;

    int activeEnemies = ENEMY_COUNT;
    Enemy enemies[ENEMY_COUNT];
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        enemies[i].x = rand() % (ST7789_WIDTH - ENEMY_WIDTH);
        enemies[i].y = rand() % (100); 
        
        enemies[i].vx = (rand() % (ENEMY_MAX_SPEED * 2 + 1)) - ENEMY_MAX_SPEED;
        enemies[i].vy = (rand() % (ENEMY_MAX_SPEED * 2 + 1)) - ENEMY_MAX_SPEED;
        
        if (enemies[i].vx == 0) enemies[i].vx = 1;
        if (enemies[i].vy == 0) enemies[i].vy = 1;
        
        enemies[i].active = 1;
    }

    int fireCooldown = 0;
    int gameState = STATE_PLAYING;

    while (1) {
        if (Input_IsPressed(KEY_PRESS)) break;

        if (gameState == STATE_PLAYING) {
            if (Input_IsPressed(KEY_LEFT)) player.x -= PLAYER_SPEED;
            if (Input_IsPressed(KEY_RIGHT)) player.x += PLAYER_SPEED;
            if (player.x < 0) player.x = 0;
            if (player.x > ST7789_WIDTH - PLAYER_WIDTH) player.x = ST7789_WIDTH - PLAYER_WIDTH;

            if (fireCooldown > 0) fireCooldown--;
            if (Input_IsPressed(KEY_A) && fireCooldown == 0) {
                for (int i = 0; i < 10; i++) {
                    if (!missiles[i].active) {
                        missiles[i].active = 1;
                        missiles[i].x = player.x + (PLAYER_WIDTH/2) - 2;
                        missiles[i].y = player.y;
                        fireCooldown = 6;
                        break;
                    }
                }
            }

            for (int i = 0; i < ENEMY_COUNT; i++) {
                if (!enemies[i].active) continue;

                enemies[i].x += enemies[i].vx;
                enemies[i].y += enemies[i].vy;

                if (enemies[i].x <= 0 || enemies[i].x >= ST7789_WIDTH - ENEMY_WIDTH) {
                    enemies[i].vx *= -1;
                }
                if (enemies[i].y <= 0) {
                    enemies[i].vy *= -1;
                }
                
                if (enemies[i].y >= ST7789_HEIGHT - ENEMY_HEIGHT) {
                    enemies[i].vy *= -1; 
                }

                if (rand() % 50 == 0) {
                    enemies[i].vx = (rand() % (ENEMY_MAX_SPEED * 2 + 1)) - ENEMY_MAX_SPEED;
                    enemies[i].vy = (rand() % (ENEMY_MAX_SPEED * 2 + 1)) - ENEMY_MAX_SPEED;
                }
            }

            for (int i = 0; i < 10; i++) {
                if (missiles[i].active) {
                    missiles[i].y -= MISSILE_SPEED;
                    if (missiles[i].y < 0) missiles[i].active = 0;

                    for (int j = 0; j < ENEMY_COUNT; j++) {
                        if (enemies[j].active) {
                            if (CheckCollision(missiles[i].x, missiles[i].y, 4, 10,
                                            enemies[j].x, enemies[j].y, ENEMY_WIDTH, ENEMY_HEIGHT)) {
                                missiles[i].active = 0;
                                enemies[j].active = 0;
                                activeEnemies--;
                                break;
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < ENEMY_COUNT; i++) {
                if (enemies[i].active) {
                    if (CheckCollision(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT,
                                       enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT)) {
                        gameState = STATE_GAMEOVER;
                    }
                }
            }

            if (activeEnemies == 0) gameState = STATE_WIN;
        }

        if (gameState == STATE_PLAYING) {
            ST7789_ClearBuffer(COLOR_BLACK);
            ST7789_DrawImage(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, player_img);

            for (int i = 0; i < ENEMY_COUNT; i++) {
                if (enemies[i].active) {
                    ST7789_DrawImage(enemies[i].x, enemies[i].y, ENEMY_WIDTH, ENEMY_HEIGHT, enemy_img);
                }
            }

            for (int i = 0; i < 10; i++) {
                if (missiles[i].active) {
                    ST7789_DrawRect(missiles[i].x, missiles[i].y, 4, 10, MISSILE_COLOR);
                }
            }
        } 
        else if (gameState == STATE_GAMEOVER) {
            ST7789_ClearBuffer(COLOR_RED);
        }
        else if (gameState == STATE_WIN) {
            ST7789_ClearBuffer(COLOR_BLUE);
        }

        ST7789_UpdateScreen();
    }

    if (player_img) free(player_img);
    if (enemy_img) free(enemy_img);

    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}