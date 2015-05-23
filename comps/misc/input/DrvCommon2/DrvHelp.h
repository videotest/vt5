#pragma once


struct CHtmlTopic
{
	DWORD dwData;
	LPCTSTR lpTopicName;
};

inline LPCTSTR _topic_by_data(DWORD dwData, CHtmlTopic *pTopics)
{
	for (int i = 0; pTopics[i].dwData != 0; i++)
	{
		if (pTopics[i].dwData == dwData-0x20000)
			return pTopics[i].lpTopicName;
	}
	return _T("unnamed");
}
