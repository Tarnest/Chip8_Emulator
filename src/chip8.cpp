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
            pixels[i][j] = pixel(i * SCREEN_WIDTH_PIXELS, j * SCREEN_HEIGHT_PIXELS);
            SDL_RenderFillRect(_renderer, &pixels[i][j].rect);
        }
    }

    SDL_RenderPresent(_renderer);
}

bool Chip8::find_bit(uint8_t &byte, int bit)
{
    int increment_size = 7 - bit; // bit in this case 0 thru 7
    
    if (bit == 0)
    {
        return byte << increment_size;
    }

    int narrowed_bit = byte << increment_size;
    int subtraction_bit = byte << increment_size + 1;
    
    return narrowed_bit - subtraction_bit;
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

    SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE); // white

    for (int i = 0; i < N; i++)
    {
        if (y_coord > SCREEN_HEIGHT_PIXELS - 1)
        {
            break;
        }
        
        uint8_t current_byte = memory[index_register + i];

        for (int j = 0; j < 8; j++)
        {            
            if (x_coord > SCREEN_WIDTH_PIXELS - 1)
            {
                break;
            }
            
            pixel current_pixel = pixels[x_coord][y_coord];
            bool current_bit = find_bit(current_byte, j);

            if (current_pixel.is_on() == current_bit && current_pixel.is_on() != false)
            {
                current_pixel.off();
                SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
                SDL_RenderFillRect(_renderer, &current_pixel.rect);
            } 
            else
            {
                current_pixel.on();
                SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE); // white
                SDL_RenderFillRect(_renderer, &current_pixel.rect);
            }

            x_coord++;
        }

        y_coord++;
    }

    SDL_RenderPresent(_renderer);
}

void Chip8::fetch()
{
    if (!file->is_open()) return;

    file->seekg(program_counter);
    file->read(reinterpret_cast<char*>(&current_instructions), sizeof uint16_t);

    program_counter += 2;
}

void Chip8::decode_and_execute()
{
    int nibble_1 = current_instructions[0] >> 4;
    int nibble_2 = current_instructions[0] - ((current_instructions[0] >> 4) << 4);
    int nibble_3 = current_instructions[1] >> 4;
    int nibble_4 = current_instructions[1] - ((current_instructions[1] >> 4) << 4);
    
    int full_num = (current_instructions[0] << 8) + current_instructions[1];    
    // third and fourth nibbles
    int NN = (nibble_3 << 4) + nibble_4;
    // second, third, and fourth nibbles AKA mem addr
    int NNN = (nibble_2 << 8) + (nibble_3 << 4) + nibble_4;

    switch(nibble_1)
    {
        case 0x0:
            clear_screen();
            break;
        case 0x1:
            jump(NNN);
            break;
        case 0x2:
            break;
        case 0x3:
            break;
        case 0x4:
            break;
        case 0x5:
            break;
        case 0x6:
            set_x(nibble_2, NN);
            break;
        case 0x7:
            add_x(nibble_2, NN);
            break;
        case 0x8:
            break;
        case 0x9:
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
    program_counter = 0;
    memory = {0};
    load_font();
    file = std::make_shared<std::fstream>();
    file->open(path, std::ios::in | std::ios::out | std::ios::binary);
    _renderer = _new_renderer;

    load_pixels();
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
