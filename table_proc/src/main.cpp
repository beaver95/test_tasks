/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: bogdan
 *
 * Created on March 20, 2019, 5:54 AM
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <iostream>
#include <regex>
#include <limits>
#include <stdexcept>

using namespace std;

class Table;
class EmptyCell;
class IntegerCell;
class StringCell;
class ReferenceCell;

class Cell
{
public:
    virtual ~Cell() {}

    virtual string getValue() = 0;
    virtual string getFormula() = 0;
    virtual void linkTable(Table* table) { _table = table; }

protected:
    Cell() : _table(0), _valid(true) {}

    Table* _table;
    string _data;
    bool _valid;
};

class Table
{
public:
    Table (unsigned short row_count, unsigned short column_count) : _row_count(row_count), _column_count(column_count) {}

    shared_ptr<Cell> getAt(unsigned short row, unsigned short column)
    {
        int index = row * _column_count + column;
        auto cell_iter = cells.find(index);
        if (cell_iter == cells.end())
        {
            shared_ptr<Cell> cell = dynamic_pointer_cast<Cell>(make_shared<EmptyCell>());
            setAt(cell, row, column);
            return cell;
        }
        return cell_iter->second;
    }

    void setAt(shared_ptr<Cell> cell, unsigned short row, unsigned short column)
    {
        int index = row * _column_count + column;
        cell.get()->linkTable(this);
        cells[index] = cell;
    }

    void print()
    {
        for (unsigned short r = 0; r < _row_count; ++r)
        {
            for (unsigned short c = 0; c < _column_count - 1; ++c)
            {
                cout << getAt(r, c).get()->getValue() << "\t";
            }
            cout << getAt(r, _column_count - 1).get()->getValue() << "\n";
        }
    }

private:
    unsigned short _row_count;
    unsigned short _column_count;
    map<int, shared_ptr<Cell>> cells;
};

class EmptyCell : public Cell
{
public:
    EmptyCell() {}
    virtual ~EmptyCell() {}

    virtual string getValue()
    {
        return string("");
    }
    virtual string getFormula() 
    {
        return string("");
    }
};

class IntegerCell : public Cell
{
public:
    IntegerCell() {}
    IntegerCell(int value) : _int(value) { _data = to_string(value); }
    IntegerCell(string value)
    {
        _data = value;
        try
        {
            _int = stoi(value);
        }
        catch (exception e)
        {
            clog << e.what() << endl;
            _valid = false;
        }
    }
    virtual ~IntegerCell() {}

    virtual string getValue()
    {
        if (!_valid)
            return string("#error");
        return _data;
    }
    virtual string getFormula()
    {
        return _data;
    }

protected:
    int _int;
};

class StringCell : public Cell
{
public:
    StringCell() {}
    StringCell(string value) { _data = value; }
    virtual ~StringCell() {}

    virtual string getValue()
    {
        if (!_valid)
            return string("#error");
        return _data;
    }
    virtual string getFormula()
    {
        return string("‘") + _data;
    }
};

class ReferenceCell : public Cell
{
public:
    ReferenceCell() {}
    ReferenceCell(unsigned short row, unsigned short column) : _row(row), _column(column) {}
    ReferenceCell(string ref)
    {
        regex ex("^([A-Z]{1,4})([1-9][0-9]{0,4})$");
        smatch sm;
        if (regex_match(ref, sm, ex))
        {
            try
            {
                int row = stoi(sm[2]) - 1;
                if (row > numeric_limits<unsigned short>::max())
                    throw out_of_range("invalid row index");
                _row = row;
                _column = atoiColumn(sm[1]);
            }
            catch (exception e)
            {
                clog << e.what() << endl;
                _valid = false;
            }
        }
        else
            _valid = false;
    }
    virtual ~ReferenceCell() {}

    virtual string getValue()
    {
        if (!_valid)
            return string("#error");
        _valid = false;
        string ret = _table->getAt(_row, _column).get()->getValue();
        _valid = true;
        return ret;
    }
    virtual string getFormula()
    {
        return string("=") + itoaColumn(_column) + to_string(_row + 1);
    }

protected:
    int numsys = 'Z' - 'A' + 1;

    string itoaColumn(int column)
    {
        string ret;
        column++;
        while(column)
        {
            ret += ('A' + column % numsys - 1);
            column -= column % numsys;
            column /= numsys;
        }
        reverse(ret.begin(), ret.end());
        return ret;
    }
    int atoiColumn(string column)
    {
        int ret = 0, nsp = 1;
        reverse(column.begin(), column.end());
        for (int i = 0; i < column.size(); ++i)
        {
            ret += (column[i] - 'A' + 1) * nsp;
            nsp *= numsys;
        }
        return ret - 1;
    }

    unsigned short _row;
    unsigned short _column;
};

class CellFabric final
{
public:
    static shared_ptr<Cell> getCellFromString(string data)
    {
        smatch sm;
        if (data.size() == 0)
        {
            clog << "empty " << data << endl;
            return make_shared<EmptyCell>();
        }
        else if (regex_match(data, sm, regex("(-?[0-9]+)")))
        {
            clog << "integer " << data << endl;
            return make_shared<IntegerCell>(sm.str(1));
        }
        else if (regex_match(data, sm, regex("‘(.*)")))
        {
            clog << "string " << data << endl;
            return make_shared<StringCell>(sm.str(1));
        }
        else if (regex_match(data, sm, regex("=([A-Z]+[0-9]+)")))
        {
            clog << "reference " << data << endl;
            return make_shared<ReferenceCell>(sm.str(1));
        }
        else
        {
            clog << "unexpected " << data << endl;
            return make_shared<EmptyCell>();
        }
    }
};

int main(int argc, char** argv) {

    int row_count = 0, column_count = 0;
    cin >> row_count >> column_count;
    Table table(row_count, column_count);

    string cell_data;

    getline(cin, cell_data, '\n');
    for (int r = 0; r < row_count; ++r)
    {
        int c = 0;
        for (; c < column_count - 1; ++c)
        {
            getline(cin, cell_data, '\t');
            table.setAt(CellFabric::getCellFromString(cell_data), r, c);
        }
        getline(cin, cell_data, '\n');
        table.setAt(CellFabric::getCellFromString(cell_data), r, c);
    }

    table.print();

    return 0;
}

