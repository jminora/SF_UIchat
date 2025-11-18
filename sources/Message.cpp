#include "Message.h"
std::atomic<int> Message::num{0};
std::atomic<int> Message::messageCounter{0};
//сообщение в чат
Message::Message() : _id(num.load()), _sender(""), _destID(0), _text(""), _timestamp("")
{
}
Message::Message(std::string timestamp, std::string writer, std::string text) : _id(++num), _sender(writer), _destID(-1), _text(text), _timestamp(timestamp)
{   
    messageCounter++;
}
//личное сообщение
Message::Message(std::string timestamp, std::string writer, int target, std::string text) : _id(++num), _sender(writer), _destID(target), _text(text), _timestamp(timestamp)
{
    messageCounter++;
}

bool Message::searchByTarget(int n) const {
    return (_destID == n); 
}

std::string Message::getSender() const {
    return _sender;
}

int Message::getDest() const {
    return _destID;
}

std::string Message::getText() const {
    return _text;
}

int Message::getID() const {
    return _id;
}

std::string Message::getTimestamp() const
{
    return _timestamp;
}
