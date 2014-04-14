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


using Generator = std::mt19937;

using IDType = Generator::result_type;

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
    if(it == All) return 3;
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
    static const std::array<string,3>
    indextypename = {{ "Link","Site", "All" }};
#ifdef DEBUG
    return putprimes(indextypename.at(int(it)),plev);
#else
    return putprimes(indextypename[int(it)],plev); 
#endif
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
    // Public Data Members

    const IDType id;
    const int m;
    const IndexType type;
    const string sname;

    //
    //////////////

    IndexDat(const string& ss, int mm, IndexType it, IDType id);

    static const shared_ptr<IndexDat>&
    Null();

    private:

    //These methods are not implemented
    //to disallow copying
    IndexDat(const IndexDat&);
    void operator=(const IndexDat&);

    }; //class IndexDat

IndexDat::
IndexDat(const string& ss, int m_, IndexType it, IDType id_)
    : 
    id(id_),
    m(m_), 
    type(it), 
    sname(ss)
    { }

const shared_ptr<IndexDat>& IndexDat::
Null()
    {
    static shared_ptr<IndexDat> Null_ = make_shared<IndexDat>("Null",1,Site,0);
    return Null_;
    }

//
// class Index
//


IDType static
generateID()
    {
    static Generator rng(std::time(NULL) + getpid());
    return rng();

    //static IDType nextid = 0;
    //++nextid;
    //return nextid;
    }


Index::
Index() 
    : 
    p(IndexDat::Null()), 
    primelevel_(0) 
    { 
    //setUniqueReal();
    }

Index::
Index(const string& name, int mm, IndexType it, int plev) 
    : 
    p(make_shared<IndexDat>(name,mm,it,generateID())), 
    primelevel_(plev) 
    { 
    if(it == All) Error("Constructing Index with type All disallowed");
    //setUniqueReal();
    }


int Index::
m() const { return p->m; }

IndexType Index::
type() const { return p->type; }

string Index::
name() const  { return putprimes(rawname(),primelevel_); }

const string& Index::
rawname() const { return p->sname; }

bool Index::
isNull() const { return (p == IndexDat::Null()); }

int Index::
primeLevel() const { return primelevel_; }

Index& Index::
primeLevel(int plev) 
    { 
    primelevel_ = plev; 
#ifdef DEBUG
    if(primelevel_ < 0)
        Error("Negative primeLevel");
#endif
    return *this;
    }

const int _Nshifts = 11;
using shift_table_t = std::array<Real,_Nshifts>;

Real static
calcPrimeShift(int p) { return 1.+sin(p)/10.; }

shift_table_t static
makeShifts()
    {
    shift_table_t res;
    for(int p = 0; p < _Nshifts; ++p)
        {
        res[p] = calcPrimeShift(p);
        }
    return res;
    }

//Lookup table for primelevel
//shifts to uniqueReals
Real static
primeShift(int p)
    {
    static shift_table_t shift = makeShifts();
    if(p < _Nshifts)
        return shift[p];
    else
        return calcPrimeShift(p);
    }

Real Index::
uniqueReal() const
    {
    return (1.+cos(p->id))*primeShift(primelevel_);
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
    return (uniqueReal() < other.uniqueReal()); 
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
            if(primelevel_ < 0)
                {
                Error("Negative primeLevel");
                }
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
    if(primelevel_ < 0)
        {
        Error("Negative primeLevel");
        }
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
        if(primelevel_ < 0)
            {
            Error("Increment led to negative primeLevel");
            }
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
    if(primelevel_ < 0)
        {
        Error("Negative primeLevel");
        }
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

const Index& Index::
Null()
    {
    static const Index Null_;
    return Null_;
    }

ostream& 
operator<<(ostream& s, const Index& t)
    {
    if(t.name() != "" && t.name() != " ") s << t.name();
    const auto iur = size_t(abs(10000*noprime(t).uniqueReal()));
    return s << "(" << nameindex(t.type(),t.primeLevel()) 
             << "," << iur << "):" << t.m();
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
    if(index == Index::Null())
        {
        Error("IndexVal initialized with null Index");
        }
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


const IndexVal& IndexVal::
Null()
    {
    static const IndexVal Null_;
    return Null_;
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
