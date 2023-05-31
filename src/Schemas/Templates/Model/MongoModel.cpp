/**
 * @file MongoModel.cpp
 * @author Vincent Andrieu (vincent.andrieu@epitech.eu)
 * @date 31/05/2023
 * @copyright Nuage Malin
 */

#include "MongoModel.hpp"

MongoModel::MongoModel(const mongocxx::collection &model) : _model(model)
{
}

MongoModel::~MongoModel()
{
    if (this->_mutex.try_lock())
        this->_mutex.unlock();
    else
        std::cerr << "[WARNING] MongoModel destroyed while locked" << std::endl;
}

core::v1::optional<bsoncxx::v_noabi::document::value>
MongoModel::find_one(const bsoncxx::document::view_or_value &filter, const mongocxx::options::find &options)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.find_one(filter, options);
}

mongocxx::cursor MongoModel::find(const bsoncxx::document::view_or_value &filter, const mongocxx::options::find &options)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.find(filter, options);
}

mongocxx::cursor MongoModel::aggregate(const mongocxx::pipeline &pipeline, const mongocxx::v_noabi::options::aggregate &options)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.aggregate(pipeline, options);
}

core::v1::optional<mongocxx::v_noabi::result::insert_one>
MongoModel::insert_one(const bsoncxx::document::view_or_value &document, const mongocxx::options::insert &options)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.insert_one(document, options);
}

core::v1::optional<mongocxx::v_noabi::result::insert_many>
MongoModel::insert_many(const std::vector<MongoCXX::Document> &document, const mongocxx::options::insert &options)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.insert_many(document, options);
}

core::v1::optional<mongocxx::v_noabi::result::delete_result>
MongoModel::delete_one(const bsoncxx::document::view_or_value &document, const mongocxx::options::delete_options &options)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.delete_one(document, options);
}

core::v1::optional<mongocxx::v_noabi::result::delete_result>
MongoModel::delete_many(const bsoncxx::document::view_or_value &document, const mongocxx::options::delete_options &options)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.delete_many(document, options);
}

core::v1::optional<mongocxx::v_noabi::result::update> MongoModel::update_one(
    const bsoncxx::document::view_or_value &filter, const bsoncxx::document::view_or_value &update,
    const mongocxx::options::update &options
)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.update_one(filter, update, options);
}

core::v1::optional<mongocxx::v_noabi::result::update> MongoModel::update_many(
    const bsoncxx::document::view_or_value &filter, const bsoncxx::document::view_or_value &update,
    const mongocxx::options::update &options
)
{
    std::lock_guard<std::mutex> lock(this->_mutex);

    return this->_model.update_many(filter, update, options);
}