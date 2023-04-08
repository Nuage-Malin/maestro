/**
 * @file FileUploadCron.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 08/04/2023
 * @copyright Nuage Malin
 */

#include "FileUploadCron.hpp"
#include "Cron/Manager/ManagerCron.hpp"

FileUploadCron::FileUploadCron() : TemplateCron("FileUpload")
{
}

void FileUploadCron::run()
{
    std::cout << "Hello world !" << std::endl;
}