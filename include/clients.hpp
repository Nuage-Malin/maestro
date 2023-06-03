/**
 * @file clients.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 15/03/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_CLIENTS_HPP
#define MAESTRO_CLIENTS_HPP

#include "Clients/Santaclaus/SantaclausClient.hpp"
#include "Clients/Bugle/BugleClient.hpp"
#include "Clients/Vault/VaultClient.hpp"

struct GrpcClients
{
    const SantaclausClient santaclaus;
    const BugleClient bugle;
    const VaultClient vault;
};

#endif