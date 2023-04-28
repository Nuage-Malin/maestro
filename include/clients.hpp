/**
 * @file clients.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_CLIENTS_HPP
#define MAESTRO_CLIENTS_HPP

#include "Clients/Santaclaus/SantaclausClient.hpp"
#include "Clients/HardwareMalin/HardwareMalinClient.hpp"
#include "Clients/Vault/VaultClient.hpp"

struct GrpcClients
{
    const SantaclausClient santaclaus;
    const HardwareMalinClient hardwareMalin;
    const VaultClient vault;
};

#endif