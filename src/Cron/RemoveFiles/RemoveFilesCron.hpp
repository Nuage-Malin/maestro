/**
 * @file DownloadFilesCron.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 13/04/2023
 * @copyright Nuage Malin
 */

#ifndef MAESTRO_REMOVEFILES_CRON_HPP
#define MAESTRO_REMOVEFILES_CRON_HPP

#include "schemas.hpp"
#include "clients.hpp"
#include "Cron/Template/TemplateCron.hpp"

class RemoveFilesCron : public TemplateCron {
  public:
    RemoveFilesCron(GrpcClients &grpcClient, EventsManager &events);

    /**
     * @brief Triggered by the cron timer,
     *  turn on all disks in the removeQueue, which will trigger the corresponding event, therefore call  _removeDiskFiles
     */
    void run() override;

  private:
    /**
     * @brief Triggered by the event of disk turned one, will remove files pending in the RemoveQueue
     * @param diskId
     */
    void _removeDiskFiles(const string &diskId);

    /**
     * @brief Triggered by the event of disk turned on, it will try to remove the file even if it doesn't exist on the disk
     * @param diskId UNUSED
    */
    void _removeFilesWithoutDisk(const string &diskId);

  private:
    EventsManager &_events;
    GrpcClients &_clients;
};

#endif