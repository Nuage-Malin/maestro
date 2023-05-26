/**
 * @file UsersBackTests.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 26/05/2023
 * @copyright Nuage Malin
 */

#ifndef TEST_USERSBACK_TESTS_HPP
#define TEST_USERSBACK_TESTS_HPP

#include <functional>

#include "utils.hpp"

#include "UsersBack/client/UsersBackClient.hpp"

class UsersBackTests {
  public:
    UsersBackTests(UsersBackClient &client);
    ~UsersBackTests() = default;

    NODISCARD const size_t &getTotalTests() const;
    NODISCARD const size_t &getPassedTests() const;
    void run();

  private:
    bool _getEmptyRootFilesIndex();
    bool _uploadFileInRootFolder();
    bool _getRootFilesIndex();
    bool _askRootFileDownload(); // Only check if the response is OK and it has a waiting time

    void _manageResult(const string &name, const bool &result);

    UsersBackClient &_client;
    size_t _totalTests = 0;
    size_t _passedTests = 0;

    string _testRootFileId;
};

#endif