#include <Windows.h>

#include <assert.h>
#include <queue>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

enum class Label: unsigned { // This needs to be unsigned so that it can be used as a bit field of length 1
    ZERO,
    ONE
};

struct HuffmanNode {
    float weight;
    int parent_index : 31;
    Label label : 1;
};

struct HeavierNode
{
    bool operator()(const HuffmanNode* lhs, const HuffmanNode* rhs)
    {
        return lhs->weight < rhs->weight;
    }
};

struct LighterNode
{
    bool operator()(const HuffmanNode* lhs, const HuffmanNode* rhs)
    {
        return lhs->weight > rhs->weight;
    }
};

struct HuffmanTree {
    int root_node_index;
    int category_count;
    HuffmanNode nodes[];
};

void initialize_full_tree(HuffmanTree* tree) {
    std::vector<HuffmanNode*> nodes;

    for (int i = 0; i < tree->category_count; i++) {
        if (tree->nodes[i].parent_index == -1) {
            nodes.push_back(tree->nodes + i);
        }
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, LighterNode> heap(nodes.begin(), nodes.end());

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

    heap.top()->parent_index = tree->root_node_index;
    heap.top()->label = Label::ZERO;
    heap.pop();
    heap.top()->parent_index = tree->root_node_index;
    heap.top()->label = Label::ONE;
}

void ensure_category_initialization(HuffmanTree* tree, int category) {
    if (tree->nodes[category].parent_index == -1) {
        initialize_full_tree(tree);
    }
}

extern "C" {

__declspec(dllexport) HuffmanTree* create_tree(int category_count) {
    assert(category_count > 1);
    assert(sizeof(HuffmanNode) == 8);

    HuffmanTree* tree = (HuffmanTree*) malloc(sizeof(HuffmanTree) + sizeof(HuffmanNode) * (2 * category_count - 2));
    tree->category_count = category_count;

    return tree;
}

__declspec(dllexport) void destroy_tree(HuffmanTree* tree) {
    free(tree);
}

__declspec(dllexport) void load_weights(HuffmanTree* tree, float* weights) {
    tree->root_node_index = 2 * tree->category_count - 2;

    std::vector<HuffmanNode*> nodes(tree->category_count);
    float total_weight = 0;

    for (int i = 0; i < tree->category_count; i++) {
        total_weight += weights[i];
        tree->nodes[i].weight = weights[i];
        tree->nodes[i].parent_index = -1;
        tree->nodes[i].label = Label::ONE;
        nodes[i] = tree->nodes + i;
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HeavierNode> heap(nodes.begin(), nodes.end());

    for (HuffmanNode* node = heap.top(); heap.size() > 2; heap.pop(), node = heap.top()) {
        if (node->weight < total_weight / 2) {
            break;
        }
        node->parent_index = tree->root_node_index;

        int new_root_index = tree->root_node_index - 1;
        tree->nodes[new_root_index].parent_index = tree->root_node_index;
        tree->nodes[new_root_index].label = Label::ZERO;
        tree->root_node_index = new_root_index;
        total_weight -= node->weight;
    }
}

__declspec(dllexport) int get_code_length(HuffmanTree* tree, int category) {
    int depth = 0;

    ensure_category_initialization(tree, category);

    for (int node_index = category; node_index != 2 * tree->category_count - 2; node_index = tree->nodes[node_index].parent_index) {
        ++depth;
    }

    return depth;
}

__declspec(dllexport) int get_code_zero_count(HuffmanTree* tree, int category) {
    int depth = 0;

    ensure_category_initialization(tree, category);

    for (int node_index = category; node_index != 2 * tree->category_count - 2; node_index = tree->nodes[node_index].parent_index) {
        depth += (tree->nodes[node_index].label == Label::ZERO);
    }

    return depth;
}

__declspec(dllexport) char* create_code_string(HuffmanTree* tree, int category) {
    char* code = new char[tree->category_count];
    int position = 0;

    ensure_category_initialization(tree, category);

    for (int node_index = category; node_index != 2 * tree->category_count - 2; node_index = tree->nodes[node_index].parent_index) {
        code[position++] = tree->nodes[node_index].label == Label::ONE ? '1' : '0';
    }
    code[position] = '\0';

    std::reverse(code, code + position);
    return code;
}

void destroy_code_string(char* string) {
    delete[] string;
}

}