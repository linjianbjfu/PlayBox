#ifndef _C_DROP_TARGET_H
#define _C_DROP_TARGET_H

#include <vector>
#include <string>

using std::vector;
using std::string;


class CDropTarget
{

public:
	virtual void	DropFile( const vector<string>& vecDropFile,const CPoint& pt )  = 0;
	virtual void	DropOver( const CPoint& pt)	= 0;
};

#endif