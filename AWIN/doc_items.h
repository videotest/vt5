class doc_item_type
{
};

class doc_item_group
{
};

class doc_item
{
	virtual long load( IStream *pstream, dword flags, long ver );
	virtual long store( IStream *pstream, dword flags, long ver );

	doc_item			*parent;
	_list_t<doc_item*>	childs;
};