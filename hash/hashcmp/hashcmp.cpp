// hashcmp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "hashcmp.h"

using namespace std;
namespace{
	struct strLess
	{
		bool operator()(const tstring&left,const tstring&right)const
		{
			return _tcsicmp(left.c_str(),right.c_str()) < 0;
		}
	};
}
typedef map<tstring,tstring,strLess> mapss;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	mapss m1,m2;

	tifstream f1;
	if(argc<=1){
		wcerr<<_T("Invalid 1st parameter")<<endl;
		return 1;
	}
	f1.open(argv[1]);
	tstring fn;
	while(getline(f1,fn,_T('\t'))){
		tstring hash;
		getline(f1,hash);
		m1[fn]=hash;
	}
	f1.close();
	if(argc<=2){
		wcerr<<_T("Invalid 2nd parameter")<<endl;
		return 1;
	}
	f1.clear();
	f1.open(argv[2]);
	while(getline(f1,fn,_T('\t'))){
		tstring hash;
		getline(f1,hash);
		m2[fn]=hash;
	}
	f1.close();
	tostream* pRes;
	tofstream* ofs=NULL;
	if(argc>3){
		tofstream* ofs=new tofstream();
		ofs->open(argv[3],ios_base::out | ios_base::trunc);
		if(!ofs)
		{
			wcerr<<_T("Invalid 3th parameter");
			return 3;
		}
		pRes=ofs;
	}else{
		pRes=&wcout;
	}
	//CArchive Res(pRes,CArchive::store);
	for( mapss::const_iterator pos = m1.begin(); pos !=m1.end(); ++pos)
	{
		const tstring& fnNew=pos->first;
		const tstring& hashNew=pos->second;
		mapss::const_iterator it=m2.find(fnNew);
		if(it!=m2.end()){
			const tstring&hashOld=it->second;
			if(hashOld!=hashNew){
				(*pRes)<<fnNew<<endl;
			}
		}else{
			(*pRes)<<fnNew<<endl;
		}
	}
	if(NULL!=ofs){
		(*ofs).close();
		delete ofs;
	}

	return nRetCode;
}
