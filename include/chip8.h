#include <array>
#include <stack>
#include <iomanip>
#include <SDL3/SDL.h>
#include <fstream>
#include <string>
#include <iostream>

#define SCREEN_WIDTH_PIXELS 64
#define SCREEN_HEIGHT_PIXELS 32
#define PIXEL_SIZE 16

class Chip8
{
    private:
        struct pixel
        {
            bool on_bit;
            SDL_FRect rect;

            pixel();
            pixel(int x, int y);
            void on();
            void off();
            bool is_on();
        };
    
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
        std::array<uint8_t, 16> registers;
        std::shared_ptr<std::fstream> file;
        std::array<uint8_t, 2> current_instruction;
        SDL_Renderer *_renderer;
        pixel pixels[SCREEN_WIDTH_PIXELS][SCREEN_HEIGHT_PIXELS];

        void load_font();
        void load_pixels();
        void load_file();
        bool find_bit(uint8_t &byte, int bit);
        void clear_screen(); // 0x00E0
        void jump(int NNN); // 0x1NNN
        void set_x(int x, int NN); // 0x6XNN
        void add_x(int x, int NN); // 0x7XNN
        void set_index(int NNN); // 0xANNN
        void display(int x, int y, int N); // 0xDXYN
        void call_subroutine(int NNN); // 0x2NNN
        void ret_subroutine(); // 0x00EE
        void skip_instruction_addr(int num, int x, int NN); // 0x3XNN, 0x4XNN,
        void skip_instuction_reg(int num, int x, int y); // 0x5XY0 and 0x9XY0
        void set_x_to_y(int x, int y); // 0x8XY0
        void binary_or(int x, int y); // 0x8XY1
        void binary_and(int x, int y); // 0x8XY2
        void logical_xor(int x, int y); // 0x8XY3
        void add_x_to_y(int x, int y); // 0x8XY4
        void subtract_y_from_x(int x, int y); // 0x8XY5  
        void subtract_x_from_y(int x, int y); // 0x8XY7
        void shift(int x, int y, int identifier); // 0x8XY6 and 0x8XYE
        void bin_dec_conversion(int x); // 0xFX33
        void store_reg(int x); // 0xFX55
        void load_reg(int x); // 0xFX65
        void add_to_index(int x); // 0xFX1E
        

    public:
        void fetch();
        void decode_and_execute();
        Chip8(const std::string &path, SDL_Renderer *_new_renderer);
        ~Chip8();
};