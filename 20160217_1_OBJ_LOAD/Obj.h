#pragma once
#include "ObjFileGroup.h"


class Obj
{
public:
	Obj();
	~Obj();

	void Initialize(char* mapFileName);
	void Destroy();
	void Update();
	void Render();
	
	inline void SetPosition(D3DXVECTOR3& pos){ position = pos; }

private:
	
	D3DXMATRIX world;
	D3DXMATRIX rotation;

	D3DXVECTOR3 position = D3DXVECTOR3(0, 0, 0);

	std::vector<ObjFileGroup*> objGroups;
};

