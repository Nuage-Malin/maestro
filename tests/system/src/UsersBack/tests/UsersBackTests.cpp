/**
 * @file UsersBackTests.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 26/05/2023
 * @copyright Nuage Malin
 */

#include "common/File.grpc.pb.h"

#include "system_tests.hpp"
#include "UsersBackTests.hpp"

const string USER_ID = "6472315b91dcdcb3d3e49d1a";

UsersBackTests::UsersBackTests(UsersBackClient &client) : _client(client)
{
}

NODISCARD const size_t &UsersBackTests::getTotalTests() const
{
    return this->_totalTests;
}

NODISCARD const size_t &UsersBackTests::getPassedTests() const
{
    return this->_passedTests;
}

void UsersBackTests::_manageResult(const string &name, const bool &result)
{
    this->_totalTests++;

    if (result) {
        this->_passedTests++;
        std::cout << name << "\033[32m"
                  << " PASSED"
                  << "\033[0m" << std::endl;
    } else
        std::cout << name << "\033[31m"
                  << " FAILED"
                  << "\033[0m" << std::endl;
}

void UsersBackTests::run()
{
    this->_manageResult("Get empty root files index", this->_getEmptyRootFilesIndex());
    this->_manageResult("Upload file in root folder", this->_uploadFileInRootFolder());
    this->_manageResult("Get root files index", this->_getRootFilesIndex());
    this->_manageResult("Ask root file download", this->_askRootFileDownload());
}

bool UsersBackTests::_getEmptyRootFilesIndex()
{
    UsersBack_Maestro::GetFilesIndexRequest request;

    request.set_userid(USER_ID);
    request.set_isrecursive(true);

    const auto &response = this->_client.getFilesIndex(request);

    ASSERT_TRUE(response.has_value())
    ASSERT_EQ(response->subfiles().fileindex_size(), 0)
    ASSERT_EQ(response->subfiles().dirindex_size(), 0)
    return true;
}

bool UsersBackTests::_uploadFileInRootFolder()
{
    UsersBack_Maestro::FileUploadRequest request;
    File::NewFile *file = new File::NewFile();
    File::FileApproxMetadata *metadata = new File::FileApproxMetadata();

    metadata->set_name("root_test.txt");
    metadata->set_dirpath("/");
    metadata->set_userid(USER_ID);
    file->set_allocated_metadata(metadata);
    file->set_content("Hello World");
    request.set_allocated_file(file);

    const auto &response = this->_client.fileUpload(request);

    ASSERT_TRUE(response.has_value())
    ASSERT_FALSE(response->fileid().empty())
    this->_testRootFileId = response->fileid();
    return true;
}

bool UsersBackTests::_getRootFilesIndex()
{
    UsersBack_Maestro::GetFilesIndexRequest request;

    request.set_userid(USER_ID);
    request.set_isrecursive(false);

    const auto &response = this->_client.getFilesIndex(request);

    ASSERT_TRUE(response.has_value())
    ASSERT_EQ(response->subfiles().fileindex_size(), 1)
    ASSERT_EQ(response->subfiles().fileindex(0).approxmetadata().name(), "root_test.txt")
    ASSERT_EQ(response->subfiles().fileindex(0).approxmetadata().dirpath(), "/")
    ASSERT_EQ(response->subfiles().fileindex(0).approxmetadata().userid(), USER_ID)
    ASSERT_EQ(response->subfiles().fileindex(0).fileid(), this->_testRootFileId)
    ASSERT_EQ(response->subfiles().fileindex(0).lasteditorid(), USER_ID)
    ASSERT_EQ(response->subfiles().dirindex_size(), 0)
    return true;
}

bool UsersBackTests::_askRootFileDownload()
{
    UsersBack_Maestro::AskFileDownloadRequest request;

    request.set_fileid(this->_testRootFileId);

    const auto &response = this->_client.askFileDownload(request);

    ASSERT_TRUE(response.has_value())
    ASSERT_TRUE(response->has_waitingtime())
    return true;
}