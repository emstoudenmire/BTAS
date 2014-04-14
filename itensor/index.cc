//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#include "index.h"

using std::istream;
using std::ostream;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;
using std::shared_ptr;
using std::make_shared;

namespace itensor {

ostream& 
operator<<(ostream& s, const IndexType& it)
    { 
    if(it == Link) s << "Link"; 
    else if(it == Site) s << "Site"; 
    else if(it == All) s << "All"; 
    return s; 
    }

int 
IndexTypeToInt(IndexType it)
    {
    if(it == Link) return 1;
    if(it == Site) return 2;
    if(it == All)  return 3;
    Error("No integer value defined for IndexType.");
    return -1;
    }

IndexType 
IntToIndexType(int i)
    {
    if(i == 1) return Link;
    if(i == 2) return Site;
    if(i == 3) return All;
    printn("No IndexType value defined for i=%d",i);
    Error("Undefined IntToIndexType value");
    return Link;
    }

string 
putprimes(string s, int plev)
    { 
    for(int i = 1; i <= plev; ++i) 
        s += "\'"; 
    return s;
    }

string 
nameindex(IndexType it, int plev)
    { 
    static const std::array<string,3> itname = {{ "Link", "Site", "All" }};
    return putprimes(itname.at(int(it)),plev);
    }

string 
nameint(const string& f, int n)
    { 
    stringstream ss; 
    ss << f << n; 
    return ss.str(); 
    }


//
// IndexDat
// Storage for Index objects.
//
struct IndexDat
    {
    //////////////
    const Index::IDType id;
    const int m;
    const IndexType type;
    const string sname;
    //////////////

    IndexDat(const string& sn, int m_, IndexType t_, Index::IDType id_)
        : 
        id(id_),
        m(m_), 
        type(t_), 
        sname(sn)
        { }

    }; //class IndexDat


//
// class Index
//


Index::IDType static
generateID()
    {
    static Index::RNG rng(std::time(NULL) + getpid());
    return rng();
    }


Index::
Index() 
    : 
    primelevel_(0) 
    { }

Index::
Index(const string& name, int mm, IndexType it, int plev) 
    : 
    p(make_shared<IndexDat>(name,mm,it,generateID())), 
    primelevel_(plev) 
    { 
    if(it == All) Error("Constructing Index with type All disallowed");
    }

int Index::
m() const { return p->m; }

IndexType Index::
type() const { return p->type; }

string Index::
name() const  { return putprimes(rawname(),primelevel_); }

const string& Index::
rawname() const { return p->sname; }

Index::IDType Index::
id() const { return p->id; }

bool Index::
isNull() const { return !p; }

int Index::
primeLevel() const { return primelevel_; }

Index& Index::
primeLevel(int plev) 
    { 
    primelevel_ = plev; 
#ifdef DEBUG
    if(primelevel_ < 0) Error("primeLevel must be non-negative");
#endif
    return *this;
    }


bool Index::
operator==(const Index& other) const 
    { 
    return ((p->id == other.p->id) && (primelevel_ == other.primelevel_));
    }

bool Index::
noprimeEquals(const Index& other) const
    { 
    return p->id == other.p->id;
    }

bool Index::
operator<(const Index& other) const 
    { 
    return (p->id < other.p->id) || (primelevel_ < other.primelevel_);
    }

IndexVal Index::
operator()(int i) const { return IndexVal(*this,i); }

Index& Index::
mapprime(int plevold, int plevnew, IndexType type)
    {
    if(primelevel_ == plevold)
        {
        if(type == All || type == this->type())
            {
            primelevel_ = plevnew;
#ifdef DEBUG
            if(primelevel_ < 0) Error("Negative primeLevel");
#endif
            }
        }
    return *this;
    }

Index& Index::
prime(int inc) 
    { 
    primelevel_ += inc; 
#ifdef DEBUG
    if(primelevel_ < 0) Error("Negative primeLevel");
#endif
    return *this;
    }

Index& Index::
prime(IndexType type, int inc)
    {
    if(type == this->type() || type == All)
        {
        primelevel_ += inc;
#ifdef DEBUG
        if(primelevel_ < 0) Error("Increment led to negative primeLevel");
#endif
        }
    return *this;
    }

void Index::
write(ostream& s) const 
    { 
    if(isNull()) Error("Index::write: Index is null");

    s.write((char*) &primelevel_,sizeof(primelevel_));

    const int t = IndexTypeToInt(p->type);
    s.write((char*) &t,sizeof(t));

    s.write((char*) &(p->id),sizeof(p->id));

    s.write((char*) &(p->m),sizeof(p->m));

    const int nlength = p->sname.length();
    s.write((char*) &nlength,sizeof(nlength));

    s.write(p->sname.data(),nlength+1);
    }

Index& Index::
read(istream& s)
    {
    s.read((char*) &primelevel_,sizeof(primelevel_));
#ifdef DEBUG
    if(primelevel_ < 0) Error("Negative primeLevel");
#endif

    int t; 
    s.read((char*) &t,sizeof(t));

    IDType id;
    s.read((char*) &id, sizeof(id));

    int mm; 
    s.read((char*) &mm,sizeof(mm));

    int nlength; 
    s.read((char*) &nlength,sizeof(nlength));

    char* newname = new char[nlength+1]; 
    s.read(newname,nlength+1);
    string ss(newname); 
    delete newname;

    p = make_shared<IndexDat>(ss,mm,IntToIndexType(t),id);
    return *this;
    }

ostream& 
operator<<(ostream& s, const Index& t)
    {
    if(t.name() != "" && t.name() != " ") s << t.name();
    s << "(" << nameindex(t.type(),t.primeLevel()) << ",";

    //Print last ndigits digits of id
    //(in reverse order but doesn't matter since it's only
    // an aid for determining which indices match)
    const auto ndigits = 4;
    auto id = t.id();
    for(size_t n = 1; n <= ndigits; ++n)
        {
        s << id%10;
        id /= 10;
        }
    s << "):" << t.m();

    return s;
    }

IndexVal::
IndexVal() 
    : 
    i(0) 
    { }

IndexVal::
IndexVal(const Index& index, int i_) 
    : 
    Index(index),
    i(i_)
    { 
#ifdef DEBUG
    if(index.isNull()) Error("IndexVal initialized with null Index");
    if(i_ < 1 || i_ > index.m())
        {
        cout << "i = " << i_ << endl;
        cout << "index = " << index << endl;
        Error("i out of range");
        }
#endif
    }

bool IndexVal::
operator==(const IndexVal& other) const
    {
    return (Index::operator==(other) && i == other.i);
    }

bool IndexVal::
operator!=(const IndexVal& other) const
    {
    return !operator==(other);
    }


string
showm(const Index& I) { return nameint("m=",I.m()); }

ostream& 
operator<<(ostream& s, const IndexVal& iv)
    { 
    const Index& ii = iv;
    return s << "IndexVal: i = " << iv.i << ", ind = " << ii << "\n"; 
    }

};
