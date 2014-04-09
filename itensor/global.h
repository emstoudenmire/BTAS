//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_GLOBAL_H
#define __ITENSOR_GLOBAL_H
#include <cmath>
#include <cstdlib>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <complex>
#include "assert.h"
#include <unistd.h>

#include "tinyformat.h"

#include "itensor/real.h"
#include "itensor/option.h"

namespace itensor {

using tinyformat::printf;
using tinyformat::format;

template <typename... Args>
void
printn(const char* fmt_string, const Args&... args)
    {
    tinyformat::printf(fmt_string,args...);
    std::cout << std::endl;
    }
void inline
printn(const char* string)
    {
    std::cout << string << std::endl;
    }

enum Printdat { ShowData, HideData };

#define PrintEither(X,Y) \
    {\
    if(Y) printn("\n%s = %f",#X,X);\
    else  printn("\n%s = %s",#X,X);\
    }
#define PrintVar(X)  PrintEither(X,false)
#define PrintData(X) PrintEither(X,true)


enum Direction { Fromright, Fromleft, Both, None };

static const int NMAX = 8;
static const Real MIN_CUT = 1E-20;
static const int MAX_M = 5000;

typedef std::complex<Real>
Complex;

static const Complex Complex_1 = Complex(1,0);
static const Complex Complex_i = Complex(0,1);

// The PAUSE macro is useful for debugging. 
// Prints the current line number and pauses
// execution until the enter key is pressed.
#define PAUSE { std::cout << "(Paused, Line " << __LINE__ << ")"; std::cin.get(); }


#ifndef DEBUG

#ifndef NDEBUG
#define NDEBUG //turn off asserts
#endif

#ifndef BOOST_DISABLE_ASSERTS
#define BOOST_DISABLE_ASSERTS
#endif

#endif



bool inline
fileExists(const std::string& fname)
    {
    std::ifstream file(fname.c_str());
    return file.good();
    }


template<class T> 
void inline
readFromFile(const std::string& fname, T& t) 
    { 
    std::ifstream s(fname.c_str()); 
    if(!s.good()) 
        Error("Couldn't open file \"" + fname + "\" for reading");
    t.read(s); 
    s.close(); 
    }


template<class T> 
void inline
writeToFile(const std::string& fname, const T& t) 
    { 
    std::ofstream s(fname.c_str()); 
    if(!s.good()) 
        Error("Couldn't open file \"" + fname + "\" for writing");
    t.write(s); 
    s.close(); 
    }


//Given a prefix (e.g. pfix == "mydir")
//and an optional location (e.g. locn == "/var/tmp/")
//creates a temporary directory and returns its name
//without a trailing slash
//(e.g. /var/tmp/mydir_SfqPyR)
std::string inline
mkTempDir(const std::string& pfix,
          const std::string& locn = "./")
    {
    //Construct dirname
    std::string dirname = locn;
    if(dirname[dirname.length()-1] != '/')
        dirname += '/';
    //Add prefix and template string of X's for mkdtemp
    dirname += pfix + "_XXXXXX";

    //Create C string version of dirname
    char* cstr;
    cstr = new char[dirname.size()+1];
    strcpy(cstr,dirname.c_str());

    //Call mkdtemp
    char* retval = mkdtemp(cstr);
    //Check error condition
    if(retval == NULL)
        {
        delete[] cstr;
        throw ITError("mkTempDir failed");
        }

    //Prepare return value
    std::string final_dirname(retval);

    //Clean up
    delete[] cstr;

    return final_dirname;
    }


/*
*
* The Arrow enum is used to label how indices
* transform under a particular symmetry group. 
* Indices with an Out Arrow transform as vectors
* (kets) and with an In Arrow as dual vectors (bras).
*
* Conventions regarding arrows:
*
* * Arrows point In or Out, never right/left/up/down.
*
* * The Site indices of an MPS representing a ket point Out.
*
* * Conjugation switches arrow directions.
*
* * All arrows flow Out from the ortho center of an MPS 
*   (assuming it's a ket - In if it's a bra).
*
* * IQMPOs are created with the same arrow structure as if they are 
*   orthogonalized to site 1, but this is just a default since they 
*   aren't actually ortho. If position is called on an IQMPO it follows 
*   the same convention as for an MPS except Site indices point In and 
*   Site' indices point Out.
*
* * Local site operators have two IQIndices, one unprimed and pointing In, 
*   the other primed and pointing Out.
*
*/

enum Arrow { In = -1, Out = 1, Neither = 0 };

Arrow inline
operator-(Arrow dir)
    {
#ifdef DEBUG
    if(dir == Neither)
        Error("Cannot reverse Arrow direction 'Neither'");
#endif
    return (dir == In ? Out : In);
    }

inline std::ostream& 
operator<<(std::ostream& s, Arrow D)
    { 
    switch(D)
        {
        case In:
            s << "In";
            return s;
        case Out:
            s << "Out";
            return s;
        case Neither:
            s << "Neither";
            return s;
        default:
            Error("Missing Arrow case");
        }
    return s; 
    }

////////
///////


class Global
    {
    public:

    static bool& 
    printdat()
        {
        static bool printdat_ = false;
        return printdat_;
        }
    static Real& 
    printScale()
        {
        static Real printScale_ = 1E-10;
        return printScale_;
        }
    static bool& 
    debug1()
        {
        static bool debug1_ = false;
        return debug1_;
        }
    static bool& 
    debug2()
        {
        static bool debug2_ = false;
        return debug2_;
        }
    static bool& 
    debug3()
        {
        static bool debug3_ = false;
        return debug3_;
        }
    static bool& 
    debug4()
        {
        static bool debug4_ = false;
        return debug4_;
        }
    /*
    static Vector& 
    lastd()
        {
        static Vector lastd_(1);
        return lastd_;
        }
        */
    static bool& 
    checkArrows()
        {
        static bool checkArrows_ = true;
        return checkArrows_;
        }
    //Global option set
    static OptSet&
    opts()
        {
        return OptSet::GlobalOpts();
        }
    //Shortcut for adding global Opts,
    //so you don't have to write Global::opts().add(Opt("MyOption",3));
    //but just Global::opts(Opt("MyOption",3));
    //Also see name,val shortcuts below.
    void static
    opts(const Opt& o)
        {
        OptSet::GlobalOpts().add(o);
        }
    //Get a global Opt by just providing its name
    Opt static
    opts(const Opt::Name& name)
        {
        return OptSet::GlobalOpts().get(name);
        }
    //Set global opts by providing their name and value
    void static
    opts(const Opt::Name& name, bool bval)
        {
        OptSet::GlobalOpts().add(name,bval);
        }
    void static
    opts(const Opt::Name& name, int ival)
        {
        OptSet::GlobalOpts().add(name,ival);
        }
    void static
    opts(const Opt::Name& name, Real rval)
        {
        OptSet::GlobalOpts().add(name,rval);
        }
    void static
    opts(const Opt::Name& name, const std::string& sval)
        {
        OptSet::GlobalOpts().add(name,sval);
        }
    void static
    warnDeprecated(const std::string& message)
        {
        static int depcount = 1;
        if(depcount <= 10)
            {
            std::cout << "\n\n" << message << "\n" << std::endl;
            ++depcount;
            }
        }
    };


class ResultIsZero : public ITError
    {
    public:

    typedef ITError
    Parent;

    ResultIsZero(const std::string& message) 
        : Parent(message)
        { }
    };

class ArrowError : public ITError
    {
    public:

    typedef ITError
    Parent;

    ArrowError(const std::string& message) 
        : Parent(message)
        { }
    };

}; //namespace itensor


#endif
