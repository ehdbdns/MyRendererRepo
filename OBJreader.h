#pragma once
#include"d3dUtil.h"
#include"rayTracerUtil.h"


class OBJReader {//不用读空行
public:
	void readOBJFile(std::string filename,XMFLOAT3 color) {
		std::ifstream fin(filename);
		std::string ignore;
		std::string a;
		fin >> a;
		while (a == "v") {
			XMFLOAT3 pos;
			fin>> pos.x >> pos.y >> pos.z;
			positions.push_back(pos);
			fin >> a;
		}
		while(a=="vn") {
			XMFLOAT3 n;
			fin  >> n.x  >> n.y  >> n.z;
			normals.push_back(n);
			fin >> a;
		}
		while (a == "vt") {
			XMFLOAT2 uv;
			fin >> uv.x >> uv.y;
			fin >> a;
		}
		int indice = 0;
		if (a != "f")
			fin >> ignore>>a;
		while (a == "f") {
			for (int i = 0;i < 3;i++) {
				Vertex v;
				int posIndex, nIndex;
				std::string Vstr;
				fin >> Vstr;
				int divideIndex = Vstr.find("/");
				std::string IndexStr = Vstr.substr(0, divideIndex);
				std::istringstream ss(IndexStr);
				ss >> posIndex;
				IndexStr = Vstr.substr(divideIndex + 1, Vstr.size() - divideIndex - 1);
				std::istringstream ss1(IndexStr);
				ss1 >> nIndex;
				v.position = XMFLOAT4{ positions[posIndex-1].x, positions[posIndex-1].y ,positions[posIndex-1].z ,1.0f };
				v.normal = normals[nIndex-1];
				v.color = color;
				XMVECTOR up = abs(v.normal.z) < 0.999 ? XMVectorSet(0.0f, 0.0f, 1.0f,0) : XMVectorSet(1.0f, 0.0f, 0.0f,0);
				XMVECTOR tangent = XMVector3Normalize(XMVector3Cross(up, XMVectorSet(v.normal.x,v.normal.y,v.normal.z,0)));
				XMStoreFloat3(&v.TangentU, tangent);
				vertices.push_back(v);
				indices.push_back(indice);
				indice++;
			}
				fin >> a;
				if(a!="f")
					break;
				if (fin.eof())
					break;
		}
	}
	EST::vector<XMFLOAT3>positions;
	EST::vector<XMFLOAT3>normals;
	EST::vector<Vertex>vertices;
	EST::vector<std::uint16_t>indices;
};