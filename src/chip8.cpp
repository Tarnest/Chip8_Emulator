#include "chip8.h"

void Chip8::load_font()
{
    int memory_location = 0x50; // arbitrary starting location, based on tradition

    for (auto font : fonts)
    {
        for (int i = 0; i < font.size(); i++)
        {
            memory[memory_location] = font[i];
            memory_location++;                  
        }

        memory_location++;
    }
}

void Chip8::load_pixels()
{
    SDL_RenderClear(_renderer);

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
    
    for (int i = 0; i < SCREEN_WIDTH_PIXELS; i++)
    {
        for (int j = 0; j < SCREEN_HEIGHT_PIXELS; j++)
        {
            pixels[i][j] = pixel(i * PIXEL_SIZE, j * PIXEL_SIZE);
            SDL_RenderFillRect(_renderer, &pixels[i][j].rect);
        }
    }

    SDL_RenderPresent(_renderer);
}

void Chip8::load_file()
{
    if (!file->is_open()) return;

    int current_place = 0x200;
    std::array<uint8_t, 2> current_instructions = {0};
    while(!file->eof())
    {
        std::array<uint8_t, 2> current_instructions;
        file->read(reinterpret_cast<char*>(&current_instructions), sizeof uint16_t);

        memory[current_place] = current_instructions[0];
        memory[++current_place] = current_instructions[1];
        current_place++;
    }
}

bool Chip8::find_bit(uint8_t &byte, int bit)
{
    int increment_size = 7 - bit; // bit in this case 0 thru 7
    
    if (bit == 0)
    {
        return byte >> increment_size;
    }

    int narrowed_bit = (byte >> increment_size) << increment_size;
    int subtraction_bit = (byte >> increment_size + 1) << increment_size + 1;
    
    int result = (narrowed_bit - subtraction_bit) >> increment_size;

    return result;
}

void Chip8::clear_screen()
{
    SDL_RenderClear(_renderer);

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
    
    for (int i = 0; i < SCREEN_WIDTH_PIXELS; i++)
    {
        for (int j = 0; j < SCREEN_HEIGHT_PIXELS; j++)
        {
            pixels[i][j].off();
            SDL_RenderFillRect(_renderer, &pixels[i][j].rect);
        }
    }

    SDL_RenderPresent(_renderer);
}

void Chip8::jump(int NNN)
{
    program_counter = NNN;
}

void Chip8::set_x(int x, int NN)
{
    registers[x] = NN;
}

void Chip8::add_x(int x, int NN)
{
    registers[x] += NN;
}

void Chip8::set_index(int NNN)
{
    index_register = NNN;
}

void Chip8::display(int x, int y, int N)
{
    int x_coord = registers[x] % SCREEN_WIDTH_PIXELS;
    int y_coord = registers[y] % SCREEN_HEIGHT_PIXELS;

    registers[0xF] = 0;

    for (int i = 0; i < N; i++)
    {
        if (y_coord > SCREEN_HEIGHT_PIXELS - 1)
        {
            break;
        }

        x_coord = registers[x] % SCREEN_WIDTH_PIXELS;   
        uint8_t current_byte = memory[index_register + i];

        for (int j = 0; j < 8; j++)
        {            
            if (x_coord > SCREEN_WIDTH_PIXELS - 1)
            {
                break;
            }
            
            pixel *current_pixel = &pixels[x_coord][y_coord];
            bool current_bit = find_bit(current_byte, j);

            if (current_pixel->is_on() && current_bit)
            {
                current_pixel->off();
                //SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
                //SDL_RenderFillRect(_renderer, &current_pixel.rect);

                registers[0xF] = 1;
            } 
            else if (!current_pixel->is_on() && current_bit)
            {
                current_pixel->on();
                //SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE); // white
                //SDL_RenderFillRect(_renderer, &current_pixel.rect);
            }

            x_coord++;
        }

        y_coord++;
    }

    SDL_RenderClear(_renderer);

    for (int i = 0; i < SCREEN_WIDTH_PIXELS; i++)
    {
        for (int j = 0; j < SCREEN_HEIGHT_PIXELS; j++)
        {
            pixel *current_pixel = &pixels[i][j];
            if (current_pixel->is_on())
            {
                SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE); // white
                SDL_RenderFillRect(_renderer, &current_pixel->rect);
            }
            else
            {
                SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
                SDL_RenderFillRect(_renderer, &current_pixel->rect);
            }
        }
    }

    SDL_RenderPresent(_renderer);
}

void Chip8::call_subroutine(int NNN)
{
    instructions.push(program_counter);
    program_counter = NNN;
}

void Chip8::ret_subroutine()
{
    program_counter = instructions.top();
    instructions.pop();
}

// 0x3XNN and 0x4XNN
void Chip8::skip_instruction_addr(int num, int x, int NN)
{
    switch (num)
    {
        case 3:
            if (registers[x] == NN)
            {
                program_counter += 2;
            }
            break;
        case 4:
            if (registers[x] != NN)
            {
                program_counter += 2;
            }
            break;
    }
}

// 0x5XY0 and 0x9XY0
void Chip8::skip_instuction_reg(int num, int x, int y)
{
    switch (num)
    {
        case 5:
            if (registers[x] == registers[y])
            {
                program_counter += 2;
            }
            break;
        case 9:
            if (registers[x] != registers[y])
            {
                program_counter += 2;
            }
            break;
    }
}

void Chip8::set_x_to_y(int x, int y)
{
    registers[x] = registers[y];
}

void Chip8::binary_or(int x, int y)
{
    registers[x] = registers[x] | registers[y];
}

void Chip8::binary_and(int x, int y)
{
    registers[x] = registers[x] & registers[y];
}

void Chip8::logical_xor(int x, int y)
{
    registers[x] = registers[x] ^ registers[y];
}

void Chip8::add_x_to_y(int x, int y)
{
    registers[x] += registers[y];
}

void Chip8::subtract_y_from_x(int x, int y)
{
    registers[x] -= registers[y];
}

void Chip8::subtract_x_from_y(int x, int y)
{
    registers[x] = registers[y] - registers[x];
}

void Chip8::shift(int x, int y, int identifier)
{
    // ! CONSIDER MAKING THIS CONFIGURABLE FOR OLDER SYSTEMS
    
    if (identifier == 0x6)
    {
        registers[x] >>= 1;
    }

    if (identifier == 0xE)
    {
        registers[x] <<= 1;
    }
}

void Chip8::bin_dec_conversion(int x)
{
    
}

void Chip8::store_reg(int x)
{

}

void Chip8::load_reg(int x)
{

}

void Chip8::fetch()
{
    current_instruction.at(0) = memory.at(program_counter);
    current_instruction.at(1) = memory.at(program_counter + 1);

    program_counter += 2;
}

void Chip8::decode_and_execute()
{
    int nibble_1 = current_instruction[0] >> 4;
    int nibble_2 = current_instruction[0] - ((current_instruction[0] >> 4) << 4);
    int nibble_3 = current_instruction[1] >> 4;
    int nibble_4 = current_instruction[1] - ((current_instruction[1] >> 4) << 4);
    
    int full_num = (current_instruction[0] << 8) + current_instruction[1];    
    // third and fourth nibbles
    int NN = (nibble_3 << 4) + nibble_4;
    // second, third, and fourth nibbles AKA mem addr
    int NNN = (nibble_2 << 8) + (nibble_3 << 4) + nibble_4;

    switch(nibble_1)
    {
        case 0x0:
            switch(NNN)
            {
                case 0x0E0:
                    clear_screen();
                    break;
                case 0x0EE:
                    ret_subroutine();
                    break;
            }
            break;
        case 0x1:
            jump(NNN);
            break;
        case 0x2:
            call_subroutine(NNN);
            break;
        case 0x3:
            skip_instruction_addr(3, nibble_2, NN); 
            break;
        case 0x4:
            skip_instruction_addr(4, nibble_2, NN); 
            break;
        case 0x5:
            skip_instuction_reg(5, nibble_2, nibble_3);
            break;
        case 0x6:
            set_x(nibble_2, NN);
            break;
        case 0x7:
            add_x(nibble_2, NN);
            break;
        case 0x8:
            switch (nibble_4)
            {
                case 0x0:
                    set_x_to_y(nibble_2, nibble_3);
                    break;
                case 0x1:
                    binary_or(nibble_2, nibble_3);
                    break;
                case 0x2:
                    binary_and(nibble_2, nibble_3);
                    break;
                case 0x3:
                    logical_xor(nibble_2, nibble_3);
                    break;
                case 0x4:
                    add_x_to_y(nibble_2, nibble_3);
                    break;
                case 0x5:
                    subtract_y_from_x(nibble_2, nibble_3);
                    break;
                case 0x6:
                    shift(nibble_2, nibble_3, nibble_4);
                    break;
                case 0x7:
                    subtract_y_from_x(nibble_3, nibble_2);
                    break;
                case 0xE:
                    shift(nibble_2, nibble_3, nibble_4);
                    break;
            }
            break;
        case 0x9:
            skip_instuction_reg(9, nibble_2, nibble_3);
            break;
        case 0xA:
            set_index(NNN);
            break;
        case 0xB:
            break;
        case 0xC:
            break;
        case 0xD:
            display(nibble_2, nibble_3, nibble_4);
            break;
        case 0xE:
            break;
        case 0xF:
            break;
    }
}

Chip8::Chip8(const std::string &path, SDL_Renderer *_new_renderer)
{
    registers = {0}; 
    program_counter = 0x200;
    memory = {0};
    file = std::make_shared<std::fstream>();
    file->open(path, std::ios::in | std::ios::out | std::ios::binary);
    _renderer = _new_renderer;

    load_font();
    load_pixels();
    load_file();
}

Chip8::~Chip8()
{
    file->close();
}

Chip8::pixel::pixel() {}

Chip8::pixel::pixel(int x, int y)
{
    on_bit = 0;

    rect.h = PIXEL_SIZE;
    rect.w = PIXEL_SIZE;
    rect.x = x;
    rect.y = y;
}

void Chip8::pixel::on()
{
    on_bit = 1;
}

void Chip8::pixel::off()
{
    on_bit = 0;
}

bool Chip8::pixel::is_on()
{
    return on_bit;
}
