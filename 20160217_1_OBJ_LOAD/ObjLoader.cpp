#include "stdafx.h"
#include "ObjLoader.h"

const char* ObjLoader::OBJ_DIRECTORY = "./Data/Obj/";

ObjLoader::ObjLoader()
{
}


ObjLoader::~ObjLoader()
{
}

void ObjLoader::LoadObjFileData(std::vector<ObjFileGroup*>& group, char* fileName)
{
	std::map<std::string, MaterialTexture*> materialTextures;

	std::vector<D3DXVECTOR3> pos;
	std::vector<D3DXVECTOR2> uv;
	std::vector<D3DXVECTOR3> normal;
	std::vector<FVF_PositionNormalTexture> vertex;
	std::string materialName;

	char fullPath[512];
	strcpy_s(fullPath, OBJ_DIRECTORY);
	strcat_s(fullPath, fileName);
	FILE* fp;
	fopen_s(&fp, fullPath, "r");

	if (fp != nullptr)
	{
		group.clear();
		while (!feof(fp))
		{
			char tempBuffer[1024];
			fgets(tempBuffer, 1024, fp);
			//OutputDebugString(tempBuffer);

			if (tempBuffer[0] == 'm')
			{
				char mtlLib[1024];
				sscanf_s(tempBuffer, "%*s %s", mtlLib, 1024);

				LoadMtlFileData(materialTextures, mtlLib);
			}
			else if (tempBuffer[0] == 'g' || tempBuffer[0] == 'o')
			{
				if (!vertex.empty())
				{
					MaterialTexture* mtl = nullptr;
					if (materialTextures.find(materialName) != materialTextures.end())
					{
						mtl = materialTextures[materialName];
					}

					ObjFileGroup* newGroup = new ObjFileGroup(mtl, vertex);
					vertex.clear();
					group.push_back(newGroup);
				}
			}			

			else if (tempBuffer[0] == 'v')
			{
				if (tempBuffer[1] == 't')
				{
					D3DXVECTOR2 vt;
					sscanf_s(tempBuffer, "%*s %f %f\n", &vt.x, &vt.y);
					uv.push_back(vt);
				}

				else if (tempBuffer[1] == 'n')
				{
					D3DXVECTOR3 vn;
					sscanf_s(tempBuffer, "%*s %f %f %f\n", &vn.x, &vn.y, &vn.z);
					normal.push_back(vn);
				}

				else
				{
					D3DXVECTOR3 v;
					sscanf_s(tempBuffer, "%*s %f %f %f\n", &v.x, &v.y, &v.z);
					pos.push_back(v);
				}
			}

			else if (tempBuffer[0] == 'u')
			{
				char mtlName[1024];
				sscanf_s(tempBuffer, "%*s %s", mtlName, 1024);
				materialName = mtlName;
			}	

			else if (tempBuffer[0] == 'f')
			{
				FVF_PositionNormalTexture pnt;
				//int res = sscanf_s(tempBuffer, "%*s %d/%d/%d %d/%d/%d %d/%d/%d\n", &pnt.pos.x, &pnt.pos.y, &pnt.pos.z, );
			}
		}



		if (!vertex.empty())
		{
			MaterialTexture* mtl = nullptr;
			if (materialTextures.find(materialName) != materialTextures.end())
			{
				mtl = materialTextures[materialName];
			}

			ObjFileGroup* newGroup = new ObjFileGroup(mtl, vertex);
			vertex.clear();
			group.push_back(newGroup);
		}

		fclose(fp);
	}

	for (auto iter = materialTextures.begin(); iter != materialTextures.end(); ++iter)
	{
		SAFE_RELEASE(iter->second);
	}
}

void ObjLoader::LoadMtlFileData(OUT std::map<std::string, MaterialTexture*>& materialTextures, char* fileName)
{
}