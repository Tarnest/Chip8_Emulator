#include "chip8.h"
#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>

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

void Chip8::fetch()
{
    if (!file->is_open()) return;

    file->seekg(program_counter);
    file->read(reinterpret_cast<char*>(&current_instructions), sizeof uint16_t);

    program_counter += 2;
}

void Chip8::decode_and_execute()
{
    // ! FIND OTHER NIBBLES
    int nibbles[4] = {current_instructions[0] >> 4, 0, 0, 0};
    
    switch(nibbles[0])
    {
        case 0x0:
            SDL_Log("Zero!");
            break;
        case 0x1:
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
            break;
        case 0x7:
            break;
        case 0x8:
            break;
        case 0x9:
            break;
        case 0xA:
            break;
        case 0xB:
            break;
        case 0xC:
            break;
        case 0xD:
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