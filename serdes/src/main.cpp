/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: bogdan
 *
 * Created on March 18, 2019, 1:12 PM
 */

#include <cstdlib>
#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <memory>
#include <limits>
#include <iostream>
#include <fstream>

using namespace std;

#define CHAR_ARRAY_LEN 10

enum HolderType : int {
    templateHolderType = 0,
    intHolderType,
    charArrayHolderType,
    doubleHolderType
};

class BaseHolder
{
public:
    BaseHolder(){}
    virtual ~BaseHolder() {}

    virtual void print() = 0;
    virtual void serialize(ostream& fs) = 0;
    virtual void deserialize(istream& fs) = 0;
};

template<class T>
class TemplateHolder : public BaseHolder
{
public:
    TemplateHolder() {}
    TemplateHolder(T data): _data(data) {}
    ~TemplateHolder() {}

    virtual void print() { cout << "Template holder: data is " << _data << endl; };
    virtual HolderType getType() { return templateHolderType; };
    virtual void serialize(ostream& fs) { HolderType type = getType(); fs.write((char*)&type, sizeof(type)).write((char*)&_data, sizeof(_data)); }
    virtual void deserialize(istream& fs) { fs.read((char*)&_data, sizeof(_data));
        clog << "_data = " << _data << endl; }

protected:
    T _data;
};

class IntHolder : public TemplateHolder<int>
{
public:
    IntHolder() {}
    IntHolder(int data): TemplateHolder(data) {}
    ~IntHolder() {}

    virtual void print() { cout << "Int holder: data is " << _data << endl; };
    virtual HolderType getType() { return intHolderType; };
};

class CharArrayHolder : public TemplateHolder<char[CHAR_ARRAY_LEN]>
{
public:
    CharArrayHolder() {}
    CharArrayHolder(const char data[]) { strncpy(_data, data, CHAR_ARRAY_LEN); }
    ~CharArrayHolder() {}

    virtual void print() { cout << "Char[] holder: data is " << _data << endl; };
    virtual HolderType getType() { return charArrayHolderType; };
};

class DoubleHolder  : public TemplateHolder<double>
{
public:
    DoubleHolder() {}
    DoubleHolder(double data): TemplateHolder(data) {}
    ~DoubleHolder() {}

    virtual void print() { cout << "Double holder: data is " << _data << endl; };
    virtual HolderType getType() { return doubleHolderType; };
};

struct HolderTree final
{
    shared_ptr<BaseHolder> holder;
    vector<shared_ptr<HolderTree>> children;

    void serialize(ostream& fs)
    {
        holder.get()->serialize(fs);
        int children_size = children.size();
        fs.write((char*)&children_size, sizeof(children_size));
        for (shared_ptr<HolderTree> child : children)
        {
            child.get()->serialize(fs);
        }
    }

    void deserialize(istream& fs)
    {
        clog << "this" << this << endl;
        int type = 0;
        int size = 0;

        fs.read((char*)&type, sizeof(type));
        clog << "type = " << type << endl;
        switch (type)
        {
            case intHolderType:
                holder = shared_ptr<BaseHolder>(new IntHolder());
                break;
            case charArrayHolderType:
                holder = shared_ptr<BaseHolder>(new CharArrayHolder());
                break;
            case doubleHolderType:
                holder = shared_ptr<BaseHolder>(new DoubleHolder());
                break;
        }
        clog << "holder created" << endl;
        holder.get()->deserialize(fs);
        fs.read((char*)&size, sizeof(size));
        clog << "size = " << size << endl;
        children.clear();
        while (size--)
        {
            children.push_back(shared_ptr<HolderTree>(new HolderTree()));
            children.back().get()->deserialize(fs);
        }
    }

    void print()
    {
        set<HolderTree*> path;
        print(0, &path);
    }

private:
    void print(int depth, set<HolderTree*> *path)
    {
        path->insert(this);
        clog << "added " << this << endl;
        cout << string(2 * depth, ' ');
        holder.get()->print();
        for (shared_ptr<HolderTree> child : children)
        {
            clog << "checking " << child.get() << endl;
            if(path->find(child.get()) == path->end())
                child.get()->print(depth+1, path);
        }
        clog << "removed " << this << endl;
        path->erase(this);
    }
};

shared_ptr<HolderTree> testTree()
{
    vector<shared_ptr<HolderTree>> leafs(0);
    for (int i = 0; i < 10; ++i)
    {
        leafs.push_back(shared_ptr<HolderTree>(new HolderTree()));
    }

    leafs[0].get()->holder = shared_ptr<BaseHolder>(new IntHolder(1));
    leafs[1].get()->holder = shared_ptr<BaseHolder>(new IntHolder(2011));
    leafs[2].get()->holder = shared_ptr<BaseHolder>(new CharArrayHolder(string("C++").data()));
    leafs[3].get()->holder = shared_ptr<BaseHolder>(new DoubleHolder(3.14));
    leafs[4].get()->holder = shared_ptr<BaseHolder>(new CharArrayHolder(string("FULCRUM").data()));
    leafs[5].get()->holder = shared_ptr<BaseHolder>(new CharArrayHolder(string("TEST").data()));
    leafs[6].get()->holder = shared_ptr<BaseHolder>(new IntHolder(9));
    leafs[7].get()->holder = shared_ptr<BaseHolder>(new IntHolder(6));
    leafs[8].get()->holder = shared_ptr<BaseHolder>(new CharArrayHolder(string("LINUX").data()));
    leafs[9].get()->holder = shared_ptr<BaseHolder>(new IntHolder(7));

    leafs[0].get()->children.push_back(leafs[1]);
    leafs[0].get()->children.push_back(leafs[2]);
    leafs[0].get()->children.push_back(leafs[3]);

    leafs[1].get()->children.push_back(leafs[4]);
    leafs[3].get()->children.push_back(leafs[5]);
    leafs[3].get()->children.push_back(leafs[6]);
    leafs[3].get()->children.push_back(leafs[7]);

    leafs[4].get()->children.push_back(leafs[8]);
    leafs[4].get()->children.push_back(leafs[9]);

    return leafs[0];
}

int main(int argc, char** argv)
{
    shared_ptr<HolderTree> test_tree_root = testTree();
    shared_ptr<HolderTree> new_tree_root = shared_ptr<HolderTree>(new HolderTree());

    test_tree_root.get()->print();

    ofstream output("data.bin", ofstream::binary);
    test_tree_root.get()->serialize(output);
    output.close();

    cout << endl;

    ifstream input("data.bin", ifstream::binary);
    new_tree_root.get()->deserialize(input);
    input.close();

    new_tree_root.get()->print();

    return 0;
}

