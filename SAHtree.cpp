#include"SAHtree.h"
int comparex(const void* a, const void* b)
{
    triangle first = *(triangle*)a;
    triangle second = *(triangle*)b;
    float firstg = (first.pos1.x + first.pos2.x + first.pos3.x) / 3.0f;
    float secondg = (second.pos1.x + second.pos2.x + second.pos3.x) / 3.0f;
    if (firstg < secondg)
        return -1;
    else
        return 1;
}
int comparey(const void* a, const void* b)
{
    triangle first = *(triangle*)a;
    triangle second = *(triangle*)b;
    float firstg = (first.pos1.y + first.pos2.y + first.pos3.y) / 3.0f;
    float secondg = (second.pos1.y + second.pos2.y + second.pos3.y) / 3.0f;
    if (firstg < secondg)
        return -1;
    else
        return 1;
}
int comparez(const void* a, const void* b)
{
    triangle first = *(triangle*)a;
    triangle second = *(triangle*)b;
    float firstg = (first.pos1.z + first.pos2.z + first.pos3.z) / 3.0f;
    float secondg = (second.pos1.z + second.pos2.z + second.pos3.z) / 3.0f;
    if (firstg < secondg)
        return -1;
    else
        return 1;
}
SAHtree::SAHtree() {

}
void SAHtree::init(EST::vector<triangle>tris) {
    head = new SAHnode();
    head->triangles = tris;
    BuildBoundingBox(head->box, tris.Getdata(), tris.size());
    missIndexTable.resize(500);
    missIndexTable.memset(missIndexTable.Getdata(), 500, -1);
    divide(head, 0, 1);
}
void SAHtree::divide(SAHnode* node, int divtype, int d) {
    if (missIndexTable[d - 1] == -1)
        missIndexTable[d - 1] = boxNum;
    else {
        SortedBoxes[missIndexTable[d - 1]].missIndex = boxNum;
        missIndexTable[d - 1] = boxNum;
        int deeper = d;
        while (missIndexTable[deeper] != -1) {
            SortedBoxes[missIndexTable[deeper]].missIndex = boxNum;
            missIndexTable[deeper] = -1;
            deeper++;
        }
    }
    boxNum++;
    depth = max(depth, d);
    AABBbox box;
    box.center = node->box.Center;
    box.extent = node->box.Extents;
    box.triangleNum = node->triangles.size();
    box.triangleStart = CurrentTriangleIndex;
    if (node->triangles.size() < 5) {
        box.isLeaf = 1;
        leafBox.push_back(node->box);
        CurrentTriangleIndex += box.triangleNum;
        SortedBoxes.push_back(box);
        for (int i = 0;i < node->triangles.size();i++)
            Triangles.push_back((node->triangles)[i]);
        return;
    }
    SortedBoxes.push_back(box);
    /*       if (divtype == 0)
               qsort(node->triangles.Getdata(), node->triangles.size(), sizeof(triangle), comparex);
           else if (divtype == 1)
               qsort(node->triangles.Getdata(), node->triangles.size(), sizeof(triangle), comparey);
           else
               qsort(node->triangles.Getdata(), node->triangles.size(), sizeof(triangle), comparez);*/
    SAHnode* left = new SAHnode;
    SAHnode* right = new SAHnode;
    float mincost = MathHelper::Infinity;
    for (int i = 1;i < node->triangles.size();i++) {
        SAHnode l;
        SAHnode r;
        EST::vector<triangle>lefttri(node->triangles.Getdata(), i);
        EST::vector<triangle>righttri(&node->triangles.Getdata()[i], node->triangles.size() - i);
        l.triangles.addData(lefttri.Getdata(), lefttri.size());
        r.triangles.addData(righttri.Getdata(), righttri.size());
        float cost = calcCost(&l) + calcCost(&r);
        if (cost < mincost) {
            left->triangles.free();
            right->triangles.free();
            left->triangles.resize(0);
            right->triangles.resize(0);
            mincost = cost;
            left->triangles.addData(lefttri.Getdata(), lefttri.size());
            left->box = l.box;
            right->box = r.box;
            right->triangles.addData(righttri.Getdata(), righttri.size());
        }
    }
    node->left = left;
    node->right = right;
    divide(node->left, (divtype + 1) % 3, d + 1);
    divide(node->right, (divtype + 1) % 3, d + 1);
}
float SAHtree::calcCost(SAHnode* node) {
    BuildBoundingBox(node->box, node->triangles.Getdata(), node->triangles.size());
    float S = 0;
    S += 2 * node->box.Extents.x * 2 * node->box.Extents.y * 2;
    S += 2 * node->box.Extents.y * 2 * node->box.Extents.z * 2;
    S += 2 * node->box.Extents.z * 2 * node->box.Extents.x * 2;
    return S * node->triangles.size();
}
void SAHtree::BuildBoundingBox(BoundingBox& box, triangle* triangles, int faceCount) {
    XMFLOAT3 f3min = { +MathHelper::Infinity,+MathHelper::Infinity ,+MathHelper::Infinity };
    XMFLOAT3 f3max = { -MathHelper::Infinity,-MathHelper::Infinity ,-MathHelper::Infinity };
    XMVECTOR v3min = XMLoadFloat3(&f3min);
    XMVECTOR v3max = XMLoadFloat3(&f3max);
    for (int i = 0;i < faceCount;i++) {
        XMVECTOR p1 = XMLoadFloat3(&triangles[i].pos1);
        XMVECTOR p2 = XMLoadFloat3(&triangles[i].pos2);
        XMVECTOR p3 = XMLoadFloat3(&triangles[i].pos3);
        XMVECTOR mi = XMVectorMin(p1, p2);
        mi = XMVectorMin(p1, p3);
        v3min = XMVectorMin(v3min, mi);
        XMVECTOR ma = XMVectorMax(p1, p2);
        ma = XMVectorMax(p1, p3);
        v3max = XMVectorMax(v3max, ma);
    }
    XMStoreFloat3(&box.Center, 0.5f * (v3min + v3max));
    XMStoreFloat3(&box.Extents, 0.5f * (v3max - v3min) + XMVectorSet(1, 1, 1, 1));
}