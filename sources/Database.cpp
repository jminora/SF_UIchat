#include "Database.h"
#include "Parsing.h"
#include "sha1.h"
#include <memory>
#include <QTime>

//'Ανθρωπος

int Database::searchUserByName(string username)
{
	auto uit = _usersMapByName.find(username);
    if (uit != _usersMapByName.end()) return uit->second;
  return -1;
}

vector<string> Database::getUserList() const
{
  vector<string> userList;
  for(auto user : _usersMapByName)
  {
    userList.push_back(user.first);
  }
  return userList;
}

string Database::getUserName(int userId) const
{
  for (auto it = _usersMapByName.begin(); it != _usersMapByName.end(); ++it) {
    if (it->second == userId)
      return it->first;
  }
  return "";
}

Database::Database() : _users(), _messages()
{
}

int Database::addUser(string username, string password)
{
	if (!correctName(username)) return -1;
	auto uit = _usersMapByName.find(username);
	if (uit != _usersMapByName.end()) return -2;
	User newUser = User(username, sha1(password));
	_users.push_back(newUser);
  _usersMapByName.insert({ username, newUser.getUserID() });
	return newUser.getUserID();
}

int Database::checkPassword(string username, string password)
{
	int result = -1;
	Hash passHash = sha1(password);
	for (const auto &u : _users)
	{
		result = u.checklogin(username, passHash);
		if (result != -1) return result;
	}
	return result;
}

bool Database::checkIsBanned(const string username)
{
    for (const auto &u : _users)
    {
        if (searchUserByName(username) == u.getUserID())
        {
            return u.isBanned();
        }
    }
    return 0;
}

bool Database::checkIsBlocked(string username)
{
    for (const auto &u : _users)
    {
        if (searchUserByName(username) == u.getUserID())
        {
            return u.isBlocked();
        }
    }
    return 0;
}

void Database::banUser(string username)
{
    for (auto &u : _users)
    {
        if (searchUserByName(username) == u.getUserID())
        {
            u.setIsBanned(true);
        }
    }
}

void Database::unbanUser(string username)
{
    for (auto &u : _users)
    {
        if (searchUserByName(username) == u.getUserID())
        {
            u.setIsBanned(false);
        }
    }
}

void Database::blockUser(string username)
{
    for (auto &u : _users)
    {
        if (searchUserByName(username) == u.getUserID())
        {
            u.setIsBlocked(true);
        }
    }
}

void Database::unblockUser(string username)
{
    for (auto &u : _users)
    {
        if (searchUserByName(username) == u.getUserID())
        {
            u.setIsBlocked(false);
        }
    }
}

void Database::addChatMessage(string timestamp, string sender, string text)
{
    _messages.push_back(Message(timestamp, sender, text));
}

bool Database::addPrivateMessage(string timestamp, string sender, string target, string message)
{
	int targetUser = searchUserByName(target);
	if (targetUser < 0)
	{
		return false;
	}
  _messages.push_back(Message(timestamp, sender, targetUser, message));
	return true;
}

vector<string> Database::getChatMessages()
{
	vector<string> strings;
	for (auto &m: _messages)
	{
		if (m.getDest() == -1)
		{
            strings.push_back("[" + m.getTimestamp() + "] " + "<" + m.getSender() + ">: " + m.getText());
		}
	}
	return strings;
}

vector<Message> Database::getPrivateMessage(int userID)
{
	vector<Message> strings;
	//int userID = searchUserByName(username);
	for (auto &m : _messages)
  {
    if(userID == -1 && m.getDest() != -1)
      strings.push_back(m);
    else if(userID != -1 && m.getDest() == userID)
      strings.push_back(m);
	}
	return strings;
}
