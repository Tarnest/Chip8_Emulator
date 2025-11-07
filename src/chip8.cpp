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

void Chip8::clear_screen()
{

}

void Chip8::jump(int NNN)
{

}

void Chip8::set_x(int x, int NN)
{

}

void Chip8::add_x(int x, int NN)
{

}

void Chip8::set_index(int NNN)
{

}

void Chip8::display(int x, int y, int N)
{

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

Chip8::Chip8(const std::string &path)
{
    program_counter = 0;
    load_font();
    file = std::make_shared<std::fstream>();
    file->open(path, std::ios::in | std::ios::out | std::ios::binary);
}

Chip8::~Chip8()
{
    file->close();
}