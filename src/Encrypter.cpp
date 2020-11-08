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
	qulonglong ulonglong = 1992060300072645261L;
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

	//Niemals überhalb des Passwort checks ein return machen, würde das PW aushebeln

	for (int i = 0; i < RADIO_COUNT; i++) {
		quint64 password = Encrypter::getInstance().GetPassword(i).toULongLong();
		if (password == 0) continue;

		SimpleCrypt crypto(password);
		QString decrypted = crypto.decryptToString(value);
		if (crypto.lastError() == SimpleCrypt::ErrorNoError && !decrypted.isEmpty()) {
			KRTComms::Log("Decrypted: " + decrypted);
			return decrypted;
		}
	}

	bool ok;
	int parsed = value.toInt(&ok);
	if (ok) {
		int radio_id = KRTComms::getInstance().GetRadioId(serverConnectionHandlerID, parsed);
		if (!Encrypter::getInstance().GetPassword(radio_id).isEmpty()) return "-1";
		return value;
	}

	return "-1";
}