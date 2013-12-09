//
// Distributed under the ITensor Library License, Version 1.0.
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_OPTION_H
#define __ITENSOR_OPTION_H

#include <map>
#include "real.h"

class Opt
    {
    public:

    typedef std::string
    Name;

    enum Type { Boolean, Numeric, String, None };

    Opt();

    Opt(const char* name);

    Opt(const Name& name);

    Opt(const Name& name, bool bval);

    Opt(const Name& name, const char* sval);
    Opt(const Name& name, const std::string& sval);

    Opt(const Name& name, int ival);

    Opt(const Name& name, Real rval);

    //
    // Operators for comparison and sorting
    //

    // Two Opts are equal if they have the same name,
    // regardless of other fields that may be set.
    bool
    operator==(const Opt& other) const
        { 
        return name_ == other.name_;
        }

    // Compares two Opts based on their name, using
    // the < operator of std::string. Useful for sorting.
    bool
    operator<(const Opt& other) const
        { 
        return name_ < other.name_;
        }

    //
    // Accessor methods
    //

    const Name&
    name() const { return name_; }

    bool
    boolVal() const { assertType(Boolean); return bool(rval_); }

    const std::string&
    stringVal() const { assertType(String); return sval_; }

    int
    intVal() const { assertType(Numeric); return int(rval_); }

    Real
    realVal() const { assertType(Numeric); return rval_; }

    bool
    isNull() const { return type_ == None; }
    bool
    isNotNull() const { return type_ != None; }

    Type
    type() const { return type_; }

    //operator const Name&() const { return name_; }

    static Opt&
    Null()
        {
        static Opt null_;
        return null_;
        }

    private:

    /////////////////////
    // Data Members

    Name name_;

    Type type_;

    std::string sval_;
    Real rval_;

    //
    /////////////////////

    void
    assertType(Type t) const;

    };

//
// OptSet
//

class OptSet
    {
    public:

    typedef Opt::Name
    Name;

    OptSet();

    OptSet(const Opt& opt1);

    OptSet(const Opt& opt1, 
           const Opt& opt2, 
           const Opt& opt3 = Opt::Null(),
           const Opt& opt4 = Opt::Null());

    OptSet(const char* ostring);
    OptSet(const std::string& ostring);

    OptSet(const OptSet& other);

    //
    // Methods for accessing Opts
    //

    bool
    defined(const Opt& opt) const;

    void
    add(const Opt& opt) { if(opt.isNotNull()) opts_[opt.name()] = opt; }
    void
    add(const Name& name, bool bval) { add(Opt(name,bval)); }
    void
    add(const Name& name, int ival) { add(Opt(name,ival)); }
    void
    add(const Name& name, const std::string& sval) { add(Opt(name,sval)); }
    void
    add(const Name& name, Real rval) { add(Opt(name,rval)); }

    void
    add(const Opt& opt1, 
        const Opt& opt2,
        const Opt& opt3 = Opt::Null(), 
        const Opt& opt4 = Opt::Null());

    void
    add(const char* ostring);

    const Opt&
    get(const Name& name) const;

    //
    // Methods for getting fields of a specific Opt
    //

    bool
    getBool(const Name& name) const;
    bool
    getBool(const Name& name, bool default_val) const;

    const std::string&
    getString(const Name& name) const;
    const std::string&
    getString(const Name& name, const std::string& default_val) const;

    int
    getInt(const Name& name) const;
    int
    getInt(const Name& name, int default_val) const;

    Real
    getReal(const Name& name) const;
    Real
    getReal(const Name& name, Real default_val) const;

    friend std::ostream& 
    operator<<(std::ostream & s, const OptSet& oset);

    static OptSet&
    GlobalOpts()
        {
        const bool isGlobal = true;
        static OptSet gos_(isGlobal);
        return gos_;
        }

    private:

    typedef std::map<Name,Opt>
    storage_type;

    typedef storage_type::iterator
    iterator;

    typedef storage_type::const_iterator
    const_iterator;

    ///////////////
    //
    // Data Members

    storage_type opts_;

    const bool is_global_;

    //
    ///////////////

    OptSet(bool isGlobal);

    void
    processString(std::string ostring);

    void
    addByString(std::string ostring);

    };

OptSet
operator&(const Opt& opt1, const Opt& opt2);

OptSet
operator&(OptSet oset, const Opt& opt);

OptSet&
operator&=(OptSet& oset, const Opt& opt);

OptSet
operator&(const Opt& opt, OptSet oset);

OptSet
operator&(const Opt& opt, const char* ostring);

OptSet
operator&(const char* ostring, const Opt& opt);

OptSet
operator&(OptSet oset, const char* ostring);

OptSet
operator&(const char* ostring, OptSet oset);

std::ostream& 
operator<<(std::ostream & s, const Opt& opt);

std::ostream& 
operator<<(std::ostream & s, const OptSet& oset);


#endif
