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
				if (tempBuffer[1] == ' ')
				{
					D3DXVECTOR3 v;
					sscanf_s(tempBuffer, "%*s %f %f %f", &v.x, &v.z, &v.y);
					pos.push_back(v);
				}

				else if (tempBuffer[1] == 't')
				{
					D3DXVECTOR2 vt;
					sscanf_s(tempBuffer, "%*s %f %f", &vt.x, &vt.y);
					vt.y = 1 - vt.y;
					uv.push_back(vt);
				}

				else if (tempBuffer[1] == 'n')
				{
					D3DXVECTOR3 vn;
					sscanf_s(tempBuffer, "%*s %f %f %f", &vn.x, &vn.z, &vn.y);
					normal.push_back(vn);
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
				FVF_PositionNormalTexture fvf[3];
				int posIndex[3], uvIndex[3], normalIndex[3];
				int destBufferSize = sizeof(int);

				int res = sscanf_s(tempBuffer,
					"%*s %d/%d/%d %d/%d/%d %d/%d/%d",
					&posIndex[0], &uvIndex[0], &normalIndex[0],
					&posIndex[1], &uvIndex[1], &normalIndex[1],
					&posIndex[2], &uvIndex[2], &normalIndex[2],
					destBufferSize, destBufferSize, destBufferSize,
					destBufferSize, destBufferSize, destBufferSize,
					destBufferSize, destBufferSize, destBufferSize
					);

				assert(res == 9 && "The format of face is not conforming to this loader!");

				for (int i = 0; i < 3; i++)
				{
					fvf[i] = FVF_PositionNormalTexture(
							pos[posIndex[i] - 1],
							normal[normalIndex[i] - 1],
							uv[uvIndex[i] - 1]
							);
				}

				vertex.push_back(fvf[0]);
				vertex.push_back(fvf[2]);
				vertex.push_back(fvf[1]);
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
	char fullPath[512];
	strcpy_s(fullPath, OBJ_DIRECTORY);
	strcat_s(fullPath, fileName);

	FILE* fp;
	fopen_s(&fp, fullPath, "r");

	if (fp != nullptr)
	{
		std::string materialName = "";
		MaterialTexture* newMT = nullptr;

		while (!feof(fp))
		{
			char tempBuffer[1024];
			fgets(tempBuffer, 1024, fp);
			//OutputDebugString(tempBuffer);

			if (tempBuffer[0] == '#')
			{
				continue;
			}

			else if (tempBuffer[0] == 'n')
			{
				if (newMT)
				{
					materialTextures[materialName] = newMT;
					newMT = nullptr;
				}

				char mtlName[1024];
				sscanf_s(tempBuffer, "%*s %s", mtlName, 1024);
				materialName = mtlName;
				if (materialTextures.find(materialName) == materialTextures.end())
				{
					newMT = new MaterialTexture;
				}
				else
				{
					newMT = materialTextures[materialName];
				}
			}

			else if (tempBuffer[0] == 'K')
			{
				D3DXCOLOR color;
				float r, g, b;
				sscanf_s(tempBuffer, "%*s %f %f %f", &r, &g, &b);
				color = D3DXCOLOR(r, g, b, 1.0f);

				switch (tempBuffer[1])
				{
				case 'a':
					newMT->material.Ambient = color;

					break;

				case 'd':
					newMT->material.Diffuse = color;

					break;

				case 's':
					newMT->material.Specular = color;

					break;

				default:
					break;
				}
			}

			else if (tempBuffer[0] == 'm' && tempBuffer[5] == 'd')
			{
				char texFileName[128];
				sscanf_s(tempBuffer, "%*s %s", texFileName, 128);

				newMT->texture = TextureManager::GetTexture(texFileName);
			}
		}

		if (newMT)
		{
			materialTextures[materialName] = newMT;
			newMT = nullptr;
		}

		fclose(fp);
	}
}