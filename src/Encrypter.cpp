#include "Encrypter.h"
#include "teamspeak/public_definitions.h"
#include "SimpleCrypt.h"
#include "KRTComms.h"

Encrypter::Encrypter() {
	for (int i = 0; i < RADIO_COUNT; i++) {
		_password[i] = "";
	}
}

Encrypter& Encrypter::getInstance() {
	static Encrypter instance;
	return instance;
}

void Encrypter::SetPassword(int radio_id, QString value) {
	if (value.isEmpty()) {
		Encrypter::getInstance()._password[radio_id] = "";
		return;
	}
	qulonglong ulonglong = 255;
	for (int i = 0; i < value.length(); i++) {
		ulonglong += value.at(i).toLatin1() * i;
	}	
	Encrypter::getInstance()._password[radio_id] = QString::number(ulonglong);	
}

QString Encrypter::GetPassword(int radio_id) {
	if (radio_id < 0 || radio_id >= RADIO_COUNT) return "";
	return Encrypter::getInstance()._password[radio_id];
}

QString Encrypter::Encrypt(int radio_id, QString value) {
	if (Encrypter::getInstance().GetPassword(radio_id).isEmpty()) return value;

	SimpleCrypt crypto(Encrypter::getInstance().GetPassword(radio_id).toULongLong());
	return crypto.encryptToString(value);
}

QString Encrypter::Decrypt(uint64 serverConnectionHandlerID, QString value) {
	bool no_match = false;
	for (int i = 0; i < RADIO_COUNT; i++) {
		//KRTComms::Log(QString::number(Encrypter::getInstance().GetPassword(i).toULongLong()));

		quint64 password = Encrypter::getInstance().GetPassword(i).toULongLong();
		if (password == 0) continue;
		no_match = true;

		SimpleCrypt crypto(password);
		QString decrypted = crypto.decryptToString(value);
		if (crypto.lastError() == SimpleCrypt::ErrorNoError && !decrypted.isEmpty()) {
			KRTComms::Log("Decrypted: " + decrypted);
			return decrypted;
		}
	}

	if (no_match) {
		return "-1";
	}

	int radio_id = KRTComms::getInstance().GetRadioId(serverConnectionHandlerID, value.toInt());
	if (!Encrypter::getInstance().GetPassword(radio_id).isEmpty()) return "-1";

	return value;
}