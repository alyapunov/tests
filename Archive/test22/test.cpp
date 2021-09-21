#include <algorithm>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <alya.h>

template <class KEY, class VALUE>
class Trie
{
    struct Node
    {
        std::vector<KEY> m_Sequence;
        std::unordered_set<VALUE> m_Values;
        std::unordered_map<KEY, Node*> m_Children;
    };

    std::unordered_map<KEY, Node*> m_Children;

    template <class Iterator>
    std::unique_ptr<Node> createNode(Iterator aBeg, Iterator aEnd)
    {
        std::unique_ptr<Node> sRes(new Node);
        sRes->m_Sequence.resize(std::distance(aBeg, aEnd));
        std::copy(aBeg, aEnd, sRes->m_Sequence.begin());
        return sRes;
    }

    template <class Iterator>
    std::unique_ptr<Node> createNode(Iterator aBeg, Iterator aEnd, VALUE aValue)
    {
        std::unique_ptr<Node> sRes = createNode(aBeg, aEnd);
        sRes->m_Values.insert(aValue);
        return sRes;
    }

public:
    template <class Iterator>
    bool insert(Iterator aBeg, Iterator aEnd, VALUE aValue)
    {
        if (aBeg == aEnd)
            return false;
        std::unordered_map<KEY, Node*>* sChildren = &m_Children;
        while (true)
        {
            auto sItr = sChildren->find(*aBeg);
            if (sChildren->end() == sItr)
            {
                auto sTail = aBeg;
                ++sTail;
                std::unique_ptr<Node> sNewNode = createNode(sTail, aEnd, aValue);
                sChildren->insert({*aBeg, sNewNode.get()});
                sNewNode.release();
                return true;
            }
            ++aBeg;
            Node* sNode = sItr->second;
            auto sCur = sNode->m_Sequence.begin();
            size_t sCount = 0;
            while (sNode->m_Sequence.end() != sCur && aBeg != aEnd && *aBeg == *sCur)
            {
                ++sCur;
                ++aBeg;
                ++sCount;
            }

            if (sNode->m_Sequence.end() == sCur)
            {
                if (aBeg == aEnd)
                {
                    return sNode->m_Values.insert(aValue);
                }
                else
                {
                    sChildren = &sNode->m_Children;
                    continue;
                }
            }
            else
            {
                if (aBeg == aEnd)
                {
                    auto sTail = sCur;
                    ++sTail;
                    std::unique_ptr<Node> sNewNode = createNode(sTail, sNode->m_Sequence.end());
                    std::unordered_set<VALUE> sNewValues({aValue});
                    sNode->m_Children.insert({*sCur, sNewNode->get()});
                    std::swap(sNode->m_Children, sNewNode->m_Children);
                    std::swap(sNode->m_Values, sNewValues);
                    sNode->m_Sequence.resize(sCount);
                    std::swap(sNewNode->Values, sNewValues);
                    sNewNode.release();
                    return true;
                }
                else
                {
                }
            }

        }



        return false;
    }
    bool insert(const std::vector<KEY>& aSequence, VALUE aValue)
    {
        return insert(aSequence.begin(), aSequence.end(), aValue);
    }
};


int main()
{
    Trie<std::string, uint32_t> t;
    t.insert(std::vector<std::string>{"a", "b", "c"}, 1);
    t.insert(std::vector<std::string>{"a", "b", "d"}, 2);
}
