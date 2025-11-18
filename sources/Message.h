#pragma once
#include <string>
#include <atomic>

class Message
{
    static std::atomic<int> num;		//уникальный номер, который получает id
    static std::atomic<int> messageCounter;
	int _id;
	std::string _sender;
	int _destID;
	std::string _text;
    std::string _timestamp;
public:
	Message();
    Message(std::string timestamp, std::string writer, std::string text);//сообщение в чат
    Message(std::string timestamp, std::string writer, int target, std::string text);//личное сообщение

  bool searchByTarget(int) const;
  std::string getSender() const;
  int getDest() const;
  int getID() const;
  std::string getText() const;
  std::string getTimestamp() const;
};
