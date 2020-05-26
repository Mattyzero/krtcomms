#pragma once
#include <QtCore/QString>
#include "teamspeak/public_definitions.h"
#include "KRTComms.h"

class Encrypter
{
public:
	static Encrypter& getInstance();

	Encrypter();
	~Encrypter() = default;

	static void SetPassword(int radio_id, QString value);
	static QString GetPassword(int radio_id);
	static QString Encrypt(int radio_id, QString value);
	static QString Decrypt(uint64 serverConnectionHandlerID, QString value);
	
private:

	QString _password[RADIO_COUNT];
};

