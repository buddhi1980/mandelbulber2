/*
 * netrender_file_receiver.h
 *
 *  Created on: 17 sie 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_NETRENDER_FILE_RECEIVER_HPP_
#define MANDELBULBER2_SRC_NETRENDER_FILE_RECEIVER_HPP_

#include <QtCore>

class cNetRenderFileReceiver : public QObject
{
	Q_OBJECT
	const qint64 CHUNK_SIZE = 1024 * 1024;

public:
	cNetRenderFileReceiver(QObject *parent = nullptr);
	~cNetRenderFileReceiver() override;

public slots:
	void ReceiveHeader(int clientIndex, qint64 size, QString fileName);
	void ReceiveChunk(int clientIndex, int chunkIndex, QByteArray data);

private:
	struct sFileInfo
	{
		QString fileName;
		QString fullFilePathInCache;
		qint64 fileSize;
		int chunkIndex = 0;
		bool receivingStarted = false;
	};

	QMap<int, sFileInfo> fileInfos;
};

#endif /* MANDELBULBER2_SRC_NETRENDER_FILE_RECEIVER_HPP_ */
