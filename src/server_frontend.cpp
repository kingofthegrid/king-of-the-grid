#include "server_frontend.h"

ServerFrontend::ServerFrontend(World& world)
    : Frontend(world)
{
}

void* ServerFrontend::on_new_bot(Bot& bot)
{
    return nullptr;
}

void ServerFrontend::on_bot_removed(Bot& bot, void* frontend)
{

}

void ServerFrontend::update_cell(int x, int y, const Cell& cell)
{

}
void ServerFrontend::step()
{

}