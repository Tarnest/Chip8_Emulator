#include <array>
#include <stack>
#include <iomanip>
#include <SDL3/SDL.h>
#include <fstream>
#include <string>
#include <iostream>

class Chip8
{
    private:
        std::array<uint8_t, 5> font_0 = {0xF0, 0x90, 0x90, 0x90, 0xF0};
        std::array<uint8_t, 5> font_1 = {0x20, 0x60, 0x20, 0x20, 0x70};
        std::array<uint8_t, 5> font_2 = {0xF0, 0x10, 0xF0, 0x80, 0xF0};
        std::array<uint8_t, 5> font_3 = {0xF0, 0x10, 0xF0, 0x10, 0xF0};
        std::array<uint8_t, 5> font_4 = {0x90, 0x90, 0xF0, 0x10, 0x10};
        std::array<uint8_t, 5> font_5 = {0xF0, 0x80, 0xF0, 0x10, 0xF0};
        std::array<uint8_t, 5> font_6 = {0xF0, 0x80, 0xF0, 0x90, 0xF0};
        std::array<uint8_t, 5> font_7 = {0xF0, 0x10, 0x20, 0x40, 0x40};
        std::array<uint8_t, 5> font_8 = {0xF0, 0x90, 0xF0, 0x90, 0xF0};
        std::array<uint8_t, 5> font_9 = {0xF0, 0x90, 0xF0, 0x10, 0xF0};
        std::array<uint8_t, 5> font_A = {0xF0, 0x90, 0xF0, 0x90, 0x90};
        std::array<uint8_t, 5> font_B = {0xE0, 0x90, 0xE0, 0x90, 0xE0};
        std::array<uint8_t, 5> font_C = {0xF0, 0x80, 0x80, 0x80, 0xF0};
        std::array<uint8_t, 5> font_D = {0xE0, 0x90, 0x90, 0x90, 0xE0};
        std::array<uint8_t, 5> font_E = {0xF0, 0x80, 0xF0, 0x80, 0xF0};
        std::array<uint8_t, 5> font_F = {0xF0, 0x80, 0xF0, 0x80, 0x80};
        std::array<std::array<uint8_t, 5>, 16> fonts = {font_0, font_1, font_2, font_3, font_4, font_5, font_6, font_7, font_8, font_9, font_A, font_B, font_C, font_D, font_E, font_F};

        std::array<uint8_t, 4096> memory;
        int program_counter;
        uint16_t index_register;
        std::stack<uint16_t> instructions;
        uint8_t delay_timer;
        uint8_t sound_timer;
        std::array<uint8_t, 16> x_registers;
        std::array<uint8_t, 16> y_registers;
        std::shared_ptr<std::fstream> file;
        std::array<uint8_t, 2> current_instructions;

        void load_font();
        void clear_screen(); // 0x00E0
        void jump(int NNN); // 0x1NNN
        void set_x(int x, int NN); // 0x6XNN
        void add_x(int x, int NN); // 0x7XNN
        void set_index(int NNN); // 0xANNN
        void display(int x, int y, int N); // 0xDXYN

    public:
        void fetch();
        void decode_and_execute();
        Chip8(const std::string &path);
        ~Chip8();
};