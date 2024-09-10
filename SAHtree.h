#pragma once
#include"d3dUtil.h"
#include<cmath>
#include<process.h>
#include <atlcoll.h>
#include<windowsx.h>
struct AABBbox {
    XMFLOAT3 center;
    UINT isLeaf = 0;
    XMFLOAT3 extent;
    int missIndex;
    UINT triangleStart = -1;
    UINT triangleNum = 0;
};
struct triangle {
    XMFLOAT3 pos1;
    UINT matIndex;
    XMFLOAT3 pos2;
    UINT texIndex;
    XMFLOAT3 pos3;
    float pad1;
    XMFLOAT3 color;
    float pad2;
    XMFLOAT3 n;
    float pad3;
    XMFLOAT4 uv12;
    XMFLOAT2 uv3;
};
struct SAHnode {
    SAHnode() {
        triangles.init();
    }
    SAHnode* left = nullptr;
    SAHnode* right = nullptr;
    BoundingBox box;
    EST::vector<triangle>triangles;
};


class SAHtree {
public:
    SAHtree();
    void init(EST::vector<triangle>tris);
    void divide(SAHnode* node, int divtype, int d);
    float calcCost(SAHnode* node);
    void BuildBoundingBox(BoundingBox& box, triangle* triangles, int faceCount);
    int CurrentTriangleIndex = 0;
    EST::vector<int>missIndexTable;
    SAHnode* head;
    int depth;
    UINT boxNum = 0;
    EST::vector<BoundingBox>leafBox;
    EST::vector<AABBbox>SortedBoxes;
    EST::vector<triangle>Triangles;
};
