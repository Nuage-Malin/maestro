/**
 * @file MongoModel.hpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 31/05/2023
 * @copyright Nuage Malin
 * @brief Mongo model to lock interactions with the database
 */

#ifndef MAESTRO_MONGO_MODEL_HPP
#define MAESTRO_MONGO_MODEL_HPP

#include <mutex>

#include <mongocxx/database.hpp>

#include "mongocxx.hpp"

class MongoModel : public mongocxx::collection {
  public:
    MongoModel(const mongocxx::collection &model);
    ~MongoModel();

    core::v1::optional<bsoncxx::v_noabi::document::value>
    find_one(const bsoncxx::document::view_or_value &filter, const mongocxx::options::find &options = mongocxx::options::find());
    mongocxx::cursor
    find(const bsoncxx::document::view_or_value &filter, const mongocxx::options::find &options = mongocxx::options::find());
    mongocxx::cursor
    aggregate(const mongocxx::pipeline &pipeline, const mongocxx::options::aggregate &options = mongocxx::options::aggregate());
    core::v1::optional<mongocxx::v_noabi::result::insert_one> insert_one(
        const bsoncxx::document::view_or_value &document, const mongocxx::options::insert &options = mongocxx::options::insert()
    );
    core::v1::optional<mongocxx::v_noabi::result::insert_many> insert_many(
        const std::vector<MongoCXX::Document> &document, const mongocxx::options::insert &options = mongocxx::options::insert()
    );
    core::v1::optional<mongocxx::v_noabi::result::delete_result> delete_one(
        const bsoncxx::document::view_or_value &document,
        const mongocxx::options::delete_options &options = mongocxx::options::delete_options()
    );
    core::v1::optional<mongocxx::v_noabi::result::delete_result> delete_many(
        const bsoncxx::document::view_or_value &document,
        const mongocxx::options::delete_options &options = mongocxx::options::delete_options()
    );
    core::v1::optional<mongocxx::v_noabi::result::update> update_one(
        const bsoncxx::document::view_or_value &filter, const bsoncxx::document::view_or_value &update,
        const mongocxx::options::update &options = mongocxx::options::update()
    );
    core::v1::optional<mongocxx::v_noabi::result::update> update_many(
        const bsoncxx::document::view_or_value &filter, const bsoncxx::document::view_or_value &update,
        const mongocxx::options::update &options = mongocxx::options::update()
    );

  private:
    mongocxx::collection _model;
    std::mutex _mutex;
};

#endif