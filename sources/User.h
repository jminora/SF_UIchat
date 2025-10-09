#pragma once
#include <string>
#include "sha1.h"
using namespace std;

class User
{
	static int userCounter;
	int _id;
	string _login;
	string _username;
	Hash _passwordHash;
    bool _isBanned;
    bool _isBlocked;
public:
	User();
	User(string username, Hash passHash);
	int checklogin(const string& login, const Hash& passHash) const;
	string& getuserName();
	int getUserID() const;
    bool isBanned() const;
    void setIsBanned(bool newIsBanned);
    bool isBlocked() const;
    void setIsBlocked(bool newIsBlocked);
};
