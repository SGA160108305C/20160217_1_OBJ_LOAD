#include "stdafx.h"
#include "Obj.h"
#include "ObjLoader.h"

Obj::Obj()
{
}


Obj::~Obj()
{
}

void Obj::Initialize(char* mapFileName)
{
	Destroy();
	ObjLoader::LoadObjFileData(objGroups, mapFileName);
	
	D3DXMatrixIdentity(&world);
}

void Obj::Destroy()
{
	for ( auto iter = objGroups.begin(); iter != objGroups.end(); ++iter )
	{
		delete *iter;
		*iter = nullptr;
	}
	objGroups.clear();
}

void Obj::Update()
{
	D3DXMatrixTranslation(&world, position.x, position.y, position.z);
}

void Obj::Render()
{
	GameManager::GetDevice()->SetRenderState(D3DRS_LIGHTING, true);
	GameManager::GetDevice()->SetTransform(D3DTS_WORLD, &world);
	for ( auto iter = objGroups.begin(); iter != objGroups.end(); ++iter )
	{
		( *iter )->Render();
	}
}
