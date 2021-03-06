#include "RTJSBuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtklog.h"
#include "StatusCode.h"

void RTJSBuffer::writeShort(char** pptr, unsigned short anInt)
{
	**pptr = (char)(anInt / 256);
	(*pptr)++;
	**pptr = (char)(anInt % 256);
	(*pptr)++;
}

unsigned short RTJSBuffer::readShort(char** pptr)
{
	char* ptr = *pptr;
	unsigned short len = 256 * ((unsigned char)(*ptr)) + (unsigned char)(*(ptr + 1));
	*pptr += 2;
	return len;
}

RTJSBuffer::RTJSBuffer()
	: m_nBufOffset(0)
{
	m_nBufLen = REQUEST_BUFFER_SIZE_IN_BYTES_32;
	m_pBuffer = new char[m_nBufLen];
	m_nParseBufLen = REQUEST_BUFFER_SIZE_IN_BYTES_32;
	m_pParseBuf = new char[m_nParseBufLen];
}


RTJSBuffer::~RTJSBuffer()
{
	delete[] m_pBuffer;
	m_pBuffer = NULL;
	delete[] m_pParseBuf;
	m_pParseBuf = NULL;
}

void RTJSBuffer::RecvData(const char*data, int size)
{
	{
		while ((m_nBufOffset + size) > m_nBufLen)
		{
			int newLen = m_nBufLen + REQUEST_BUFFER_SIZE_IN_BYTES_32;
			if (size > newLen)
				newLen = m_nBufLen + size;
			char* temp = new char[newLen];
			if (temp == NULL)
				continue;
			memcpy(temp, m_pBuffer, m_nBufLen);
			delete[] m_pBuffer;
			m_pBuffer = temp;
			m_nBufLen = newLen;
            //LI("RTJSBuffer::RecvData total newLen is :%d\n", newLen);
		}
        //LI("RTJSBuffer::RecvData total m_nBufLen is :%d\n", m_nBufLen);

		memcpy(m_pBuffer + m_nBufOffset, data, size);
		m_nBufOffset += size;
	}

    //LI("RTJSBuffer::RecvData m_nBufOffset:%d, m_nBufLen:%d, m_nParseBufLen:%d\n", m_nBufOffset, m_nBufLen, m_nParseBufLen);
	while (m_nBufOffset > 3)
	{
		int parsed = 0;
		if (m_pBuffer[0] != '$')
		{// Has error!
            LE("RTJSBuffer::RecvData mTestName:%s, m_pBuffer[0] is not $, %c, %d\n", mTestName.c_str(), m_pBuffer[0], m_pBuffer[0]);
			parsed = m_nBufOffset;
		}
		else
		{
			char*pptr = m_pBuffer + 1;
			int packLen = readShort(&pptr);
            //LI("RTJSBuffer::RecvData readShort packLen is :%d\n", packLen);
			if ((packLen + 3) <= m_nBufOffset)
			{
				ParseMessage(pptr, packLen);
				parsed = packLen + 3;
			}
			else
			{
                LE("RTJSBuffer::RecvData packLen:%d, packLen+3 <= m_nBufOffset :%d\n", packLen, m_nBufOffset);
				break;
			}
		}

		if (parsed > 0 && m_pBuffer != NULL)
		{
			m_nBufOffset -= parsed;
			if (m_nBufOffset == 0)
			{
				memset(m_pBuffer, 0, m_nBufLen);
			}
			else
			{
				memmove(m_pBuffer, m_pBuffer + parsed, m_nBufOffset);
			}
		}
	}
}

void RTJSBuffer::ParseMessage(const char*message, int nLen)
{
	if (nLen >= m_nParseBufLen)
	{
		m_nParseBufLen = nLen + 1;
		delete[] m_pParseBuf;
		m_pParseBuf = new char[m_nParseBufLen];
	}
	memcpy(m_pParseBuf, message, nLen);
	m_pParseBuf[nLen] = '\0';
	OnRecvMessage(m_pParseBuf, nLen);
}
