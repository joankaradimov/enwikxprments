#include <Windows.h>

#include <assert.h>
#include <queue>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

enum class Label {
    ZERO,
    ONE
};

struct HuffmanNode {
    float weight;
    int parent_index;
    Label label;
};

struct HuffmanTreeCompare
{
    bool operator()(const HuffmanNode* lhs, const HuffmanNode* rhs)
    {
        return lhs->weight > rhs->weight;
    }
};

struct HuffmanTree {
    int category_count;
    HuffmanNode nodes[];
};

extern "C" {

__declspec(dllexport) HuffmanTree* create_tree(int category_count) {
    assert(category_count > 1);
    printf("Size of node: %d\n", sizeof(HuffmanNode));
    HuffmanTree* tree = (HuffmanTree*) malloc(sizeof(HuffmanTree) + sizeof(HuffmanNode) * (2 * category_count - 2));
    tree->category_count = category_count;

    return tree;
}

__declspec(dllexport) void destroy_tree(HuffmanTree* tree) {
    free(tree);
}

__declspec(dllexport) void load_weights(HuffmanTree* tree, float* weights) {
    std::vector<HuffmanNode*> nodes(tree->category_count);

    for (int i = 0; i < tree->category_count; i++) {
        tree->nodes[i].weight = weights[i];
        nodes[i] = tree->nodes + i;
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanTreeCompare> heap(nodes.begin(), nodes.end());

    int new_parent_index = tree->category_count;

    while (heap.size() > 2) {
        assert(new_parent_index < 2 * tree->category_count - 2);

        HuffmanNode* left = heap.top();
        heap.pop();
        left->parent_index = new_parent_index;
        left->label = Label::ZERO;

        HuffmanNode* right = heap.top();
        heap.pop();
        right->parent_index = new_parent_index;
        right->label = Label::ONE;

        HuffmanNode* new_parent = tree->nodes + new_parent_index;
        new_parent->weight = left->weight + right->weight;

        heap.push(new_parent);
        ++new_parent_index;
    }

    heap.top()->parent_index = -1;
    heap.top()->label = Label::ZERO;
    heap.pop();
    heap.top()->parent_index = -1;
    heap.top()->label = Label::ONE;
}

__declspec(dllexport) int get_code_length(HuffmanTree* tree, int category) {
    int depth = 0;

    for (int node_index = category; node_index != -1; node_index = tree->nodes[node_index].parent_index) {
        ++depth;
    }

    return depth;
}

__declspec(dllexport) int get_code_zero_count(HuffmanTree* tree, int category) {
    int depth = 0;

    for (int node_index = category; node_index != -1; node_index = tree->nodes[node_index].parent_index) {
        depth += (tree->nodes[node_index].label == Label::ZERO);
    }

    return depth;
}

__declspec(dllexport) char* create_code_string(HuffmanTree* tree, int category) {
    char* code = new char[tree->category_count];
    int position = 0;

    for (int node_index = category; node_index != -1; node_index = tree->nodes[node_index].parent_index) {
        code[position++] = tree->nodes[node_index].label == Label::ONE ? '1' : '0';
    }
    code[position] = 0;

    std::reverse(code, code + position);
    return code;
}

}