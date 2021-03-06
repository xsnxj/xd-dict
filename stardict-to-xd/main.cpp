#include <QApplication>
#include <QtCore>
#include <iostream>

#ifdef ARM
static inline quint32 get_uint32(const char *addr)
{
	quint32 result;
	memcpy(&result, addr, sizeof(quint32));
	return result;
}
#else
#define get_uint32(x) *reinterpret_cast<const quint32 *>(x)
#endif
#include <winsock2.h>

void startDict(){
	QString LF = "\n";

	//QString fromDictIndexName = "KoreanDic.idx";
	//QString fromDictName = "KoreanDic.dict";
	//QString toDictName = "korean.dict";
	//quint32 fromDictIndexSize = 2601262;
	//quint32 wordCount = 0;

	//QString fromDictIndexName = "langdao-ce-gb.idx";
	//QString fromDictName = "langdao-ce-gb.dict";
	//QString toDictName = "cn-en_langdao.dict";
	//quint32 fromDictIndexSize = 8918534;
	//quint32 wordCount = 405719;
	
	//QString fromDictIndexName = "langdao-ec-gb.idx";
	//QString fromDictName = "langdao-ec-gb.dict";
	//QString toDictName = "en-cn_langdao.dict";
	//quint32 fromDictIndexSize = 10651674;
	//quint32 wordCount = 435468;
	//QString dictName = QString::fromLocal8Bit("朗道英汉词典");

	QString fromDictIndexName = "Concise_Oxford_English_Diction.idx";
	QString fromDictName = "Concise_Oxford_English_Diction.dict";
	QString toDictName = "en-cn_Concise_Oxford_English_Diction.dict";
	quint32 fromDictIndexSize = 1447923;
	quint32 wordCount = 78754;
	QString dictName = QString::fromLocal8Bit("Concise Oxford English Dictionary");

	//QString fromDictIndexName = "oxford-gb.idx";
	//QString fromDictName = "oxford-gb.dict";
	//QString toDictName = "en-cn_oxford.dict";
	//quint32 fromDictIndexSize = 721264;
	//quint32 wordCount = 39429;
	//QString dictName = QString::fromLocal8Bit("牛津现代英汉双解词典");

	//QString fromDictIndexName = "jmdict-en-ja.idx";
	//QString fromDictName = "jmdict-en-ja.dict";
	//QString toDictName = "en-ja_jmdict.dict";
	//quint32 fromDictIndexSize = 2392521;
	//quint32 wordCount = 95443;
	//QString dictName = QString::fromLocal8Bit("JMDict");

	quint32 wordoffset = 0;
    quint32 wordsize = 0;
	char *buf = new char[fromDictIndexSize];
	QFile file("./" + fromDictIndexName);
	file.open(QIODevice::ReadOnly);
	QDataStream datastream(&file);
	while(!datastream.atEnd()){
		int rt = datastream.readRawData(buf, fromDictIndexSize);
		if (rt < 0)
			break;
	}
	char *p_temp = buf;
	quint32 len;

	QFile tofile("./" + toDictName);
	if (!tofile.open(QIODevice::WriteOnly)) {
		return;
	}
	QTextStream out(&tofile);
	out.setCodec("UTF-8");
	out << "Dict-Name: " + dictName + LF;
	out << "Content-Type: text" + LF;
	out << "Word-Count: " + QString::number(wordCount) + LF;
	out << LF;

	QFile fromfile("./" + fromDictName);
	if (!fromfile.open(QIODevice::ReadOnly)) {
		return;
	}
	for (quint64 i = 0; i < fromDictIndexSize;) {
		QString word = QString::fromUtf8(p_temp);
		len = qstrlen(p_temp);
		p_temp += len;
		p_temp += 1;
		i += len + 1;
		
		wordoffset = ntohl(get_uint32(p_temp));
		p_temp += sizeof(quint32);
		i += sizeof(quint32);
		
		wordsize = ntohl(get_uint32(p_temp));
		p_temp += sizeof(quint32);
		i += sizeof(quint32);

		fromfile.seek(wordoffset);
		QByteArray ba = fromfile.read(wordsize);

		QString description = QString::fromUtf8(ba.data());
		QStringList descList = description.split("\n");
		description = "";
		for (qint32 k = 0; k < descList.count(); k++) {
			if (descList.at(k).trimmed() != "") {
				description += descList.at(k) + "\n";
			}
		}
		QString firstLine = descList.at(0);
		QString pron = "";
		if (firstLine.left(2) == "*[") {
			pron = firstLine.mid(2, firstLine.size() - 3);
			description = description.mid(firstLine.size() + 1);
		}

		QString temp;
		temp.append(word + LF);
		if (pron != "") {
			pron.replace(QString(0x04D9), QString(0x0259));
			pron.replace(QString(0x0252), QString(0x0254));
			//pron.replace("ɒ", "ɔ");
			temp.append("[[" + pron + "]]" + LF);
		}
		temp.append(description.trimmed() + LF + LF);
		out << temp;
	}
	tofile.close();
	fromfile.close();
	file.close();
	delete[] buf;
}

int main(int argc, char *argv[])
{
	startDict();
	QApplication app(argc, argv);
    return app.exec();
}