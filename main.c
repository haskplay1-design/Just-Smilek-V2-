#include <gb/gb.h>
#include <stdio.h>
#include <rand.h>

// Графика Игрока (Смайлик с глазами и контуром, 4 оттенка для палитры Lemuroid)
const unsigned char tile_player[] = {
    0x3C,0x3C, 0x42,0x7E, 0x85,0xBD, 0x81,0xFF,
    0xA5,0xC3, 0x99,0xE7, 0x42,0x7E, 0x3C,0x3C
};

// Графика Монетки (Круглая блестящая монетка)
const unsigned char tile_coin[] = {
    0x3C,0x3C, 0x42,0x42, 0x99,0x99, 0xBD,0xBD,
    0xBD,0xBD, 0x99,0x99, 0x42,0x42, 0x3C,0x3C
};

// Функция для генерации случайного числа в диапазоне
uint8_t get_random(uint8_t min, uint8_t max) {
    return (rand() % (max - min + 1)) + min;
}

void main(void) {
    // Координаты игрока (начало в центре экрана)
    uint8_t player_x = 80;
    uint8_t player_y = 72;

    // Координаты монетки
    uint8_t coin_x = 40;
    uint8_t coin_y = 40;

    uint8_t keys; // Для считывания кнопок

    // Инициализация звукового движка Game Boy
    NR52_REG = 0x80; // Включить звук
    NR50_REG = 0x77; // Максимальная громкость левого и правого каналов
    NR51_REG = 0xFF; // Включить все звуковые каналы

    // Загрузка графики в VRAM
    set_sprite_data(0, 1, tile_player); // Игрок под индексом 0
    set_sprite_data(1, 1, tile_coin);   // Монетка под индексом 1

    // Настройка спрайтов
    set_sprite_tile(0, 0); // Спрайт 0 использует графику игрока
    set_sprite_tile(1, 1); // Спрайт 1 использует графику монетки

    // Правильная настройка палитры для раскраски в эмуляторе Lemuroid
    set_sprite_prop(0, 0x00);
    set_sprite_prop(1, 0x00);
    OBP0_REG = 0xE4; // Стандартная схема: прозрачный, светлый, серый, черный

    // Отображаем объекты на стартовых позициях
    move_sprite(0, player_x, player_y);
    move_sprite(1, coin_x, coin_y);

    SHOW_SPRITES; // Включаем рендеринг спрайтов

    // Главный игровой цикл
    while(1) {
        keys = joypad(); // Считываем нажатия на экранные кнопки Lemuroid

        // Движение влево с проверкой границы экрана
        if ((keys & J_LEFT) && (player_x > 8)) { 
            player_x--; 
        }
        // Движение вправо с проверкой границы экрана
        if ((keys & J_RIGHT) && (player_x < 160)) { 
            player_x++; 
        }
        // Движение вверх с проверкой границы экрана
        if ((keys & J_UP) && (player_y > 16)) { 
            player_y--; 
        }
        // Движение вниз с проверкой границы экрана
        if ((keys & J_DOWN) && (player_y < 152)) { 
            player_y++; 
        }

        // Обновляем позицию игрока на экране
        move_sprite(0, player_x, player_y);

        // Используем движение игрока как источник случайности для генератора чисел
        initrand(player_x + player_y + coin_x);

        // Проверка столкновения (Хитбокс) между игроком и монеткой
        // Если расстояние между центрами объектов по X и Y меньше 6 пикселей
        if ((player_x >= coin_x - 6 && player_x <= coin_x + 6) && 
            (player_y >= coin_y - 6 && player_y <= coin_y + 6)) {
            
            // 1. Проигрываем классический ретро-звук "Пип!" (Канал 1)
            NR10_REG = 0x00;
            NR11_REG = 0x80;
            NR12_REG = 0xF1;
            NR13_REG = 0x00;
            NR14_REG = 0xC7; // Триггер звука

            // 2. Перемещаем монетку в новую случайную точку внутри экрана
            coin_x = get_random(16, 144);
            coin_y = get_random(24, 136);
            move_sprite(1, coin_x, coin_y);
        }

        wait_vbl_done(); // Ограничение цикла до 60 кадров/сек
    }
}