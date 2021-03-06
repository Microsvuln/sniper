/**
 * \file SymbolicPath.h
 *
 * ----------------------------------------------------------------------
 *                SNIPER : Automatic Fault Localization 
 *
 * Copyright (C) 2016 Si-Mohamed LAMRAOUI
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program (see LICENSE.TXT).  
 * If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 * \author Si-Mohamed Lamraoui
 * \date   16 February 2016
 */

#ifndef _SYMBOLICPATH_H
#define _SYMBOLICPATH_H

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include "ExecutionEngine/Symbol.h"
#include "Logic/Expression.h"

using namespace llvm;

class ExprCell;
class BranchExprCell;
class CallExprCell;
class AssertExprCell;
class AssumeExprCell;

typedef std::shared_ptr<ExprCell> ExprCellPtr;
typedef std::shared_ptr<BranchExprCell> BranchExprCellPtr;
typedef std::shared_ptr<CallExprCell> CallExprCellPtr;
typedef std::shared_ptr<AssertExprCell> AssertExprCellPtr;
typedef std::shared_ptr<AssumeExprCell> AssumeExprCellPtr;

/**
 * \brief State of a branching point.
 */
typedef struct State {
    bool branch; /**< Value of the branch instruction condition (true:bb1,false:bb2). */
    bool done; /**< True if both branches were explored */
} State_t;

/**
 * \class SymbolicPath
 *　
 * \brief This class represents a path constraint (PC) 
 * to collect symbolic predicate expressions from branching 
 * points. 
 * 
 * This class is used in the concolic execution engine. 
 * The conjunction of the predicates in PC holds for 
 * the execution path. The CUTE algorithm (concolic execution 
 * algorithm) checks whether the PC with the last constraint 
 * negated is satisfiable. If so, new input values can be generated 
 * allowing each test run to exercise different program paths.
 */
class SymbolicPath {
    
private:
    /**
     * Path constraint (PC).
     */
    std::vector<ExprCellPtr> path;
    /**
     * Post-conditions.
     */
    std::vector<ExprCellPtr> asserts;
     /**
     * Pre-conditions.
     */
    std::vector<ExprCellPtr> assumes;
     /**
     * Record the current path state.
     */
    std::vector<State_t> stack;
    /**
     * Record the number of branching points.
     */
    unsigned nbBranch;

public:
    /**
     * Default constructor.
     */
    SymbolicPath() : nbBranch(0) { }
    /**
     * Copy constructor.
     *
     * \param obj A symbolic path object to copy.
     */
    SymbolicPath(SymbolicPath &obj) {
        path = obj.path;
        nbBranch = obj.nbBranch;
    }
    /**
     * Destructor.
     */
    ~SymbolicPath() { }
    SymbolicPath* clone() {
        return new SymbolicPath(*this);
    }
    
    /**
     * Return the path constraint (PC).
     */
    std::vector<ExprCellPtr> getPath();
    /**
     * Return the post-conditions.
     */
    std::vector<ExprCellPtr> getAsserts();
    /**
     * Return the current path state.
     */
    std::vector<State_t> getStack();
    /**
     * Set a new path state.
     */
    void setStack(std::vector<State_t> s);
    /**
     * Add a new symbolic predicate expression (branch instruction) 
     * to the path constraint and update the stack.
     *
     * \param svcond A symbol representating a branch instruction.
     * \param branchTaken The outcome of the branch instruction. 
     * \param i An LLVM branch (br) instruction.
     */
    void addBranchCell(SymbolPtr svcond, bool branchTaken, Instruction *i);
    /**
     * Add a new symbolic predicate expression (call instruction) 
     * to the path constraint.
     *
     * \param vret Returned value of the function beging called. 
     * \param sarg1 A symbol representating a call instruction.
     * \param i An LLVM call instruction.
     */
    void addCallCell(Value *vret, SymbolPtr sarg1, Instruction *i);
    /**
     * Add a new symbolic predicate expression (call to an assert function) 
     * to the path constraint.
     *
     * \param svcond A symbol representating a call instruction.
     * \param assertResult The outcome of the assert (fail or not).
     * \param i An LLVM call instruction.
     */
    void addAssertCell(SymbolPtr s, bool assertResult, Instruction *i);
    /**
     * Add a new symbolic predicate expression (call to an assume function) 
     * to the path constraint.
     *
     * \param svcond A symbol representating a call instruction.
     * \param assertResult The outcome of the assume (fail or not).
     * \param i An LLVM call instruction.
     */
    void addAssumeCell(SymbolPtr s, bool assumeResult, Instruction *i);
    /**
     * Return the number of branching points. 
     */
    unsigned getNbBranch();
    /**
     * Clear the path constraint.
     */
    void clear();
    /**
     * Clear the state of the current path.
     */
    void clearStack();
    /**
     * Add a new branching point state to the stack. 
     */
    void updateStack(bool branch);
    /**
     * Dump to the standard output the path constraint.
     */
    void dump();
    
private:
    /** 
     * Add a new symbolic predicate expression 
     * to the path constraint.
     *
     * \param n Cell holding a symbolic predicate expression. 
     */
    void addCell(ExprCellPtr n);
    
};

/**
 *\class ExprCell
 *
 * \brief Base class to represent an element (cell) of a path constraint. 
 */
class ExprCell {

protected:
    /**
     * Constraint encoding the cell's instruction.
     */
    ExprPtr expr;
    /**
     * Negation of ExprCell#expr.
     */
    ExprPtr notExpr;
    Instruction *i;

public:
     /**
     * Default constructor.
     *
     * \param i The LLVM instruction related to this cell.
     */
    ExprCell(Instruction *i) : i(i) { }
     /**
     * Destructor.
     */
    virtual ~ExprCell() { }

    /**
     * Return true if this cell holds a branch instruction, 
     * false otherwise.
     */
    virtual bool isBranch() { 
        return false; 
    }
    /**
     * Return true if this cell holds a call instruction, 
     * false otherwise.
     */
    virtual bool isFunCall() { 
        return false; 
    }
    /**
     * Return true if this cell holds a call to an assert function, 
     * false otherwise.
     */
    virtual bool isAssert() { 
        return false; 
    }
     /**
     * Return true if this cell holds a call to an assume function, 
     * false otherwise.
     */
    virtual bool isAssume() { 
        return false; 
    }
    /**
     * Dump information to the standard output.
     */
    virtual void dump() = 0;
    /**
     * Return the constraint encoding the cell's instruction.
     */
    ExprPtr getExpr() { 
        return expr; 
    }
    /**
     * Return a negation of the constraint encoding the cell's instruction.
     */
    ExprPtr getNotExpr() { 
        return notExpr; 
    }
     /**
     * Return the cell's instruction.
     */
    Instruction* getInstruction() { 
        return i; 
    }

};

/**
 *\class BranchExprCell
 *
 * \brief This class represents an element (cell) of a path constraint, 
 * which holds a branch instruction. 
 */
class BranchExprCell : public ExprCell {

protected:
    /**
     * True if the "then" branch was taken, 
     * false if the "else" branch was taken.
     */
    bool branchTaken;

public:
     /**
     * Default constructor.
     *
     * \param svcond The symbolic value related to this cell.
     * \param branchTaken True if the "then" branch was taken, 
     * false if the "else" branch was taken.
     * \param i An LLVM branch instruction.
     */
    BranchExprCell(SymbolPtr svcond, bool branchTaken, Instruction *i) 
    : ExprCell(i), branchTaken(branchTaken) {
        this->expr    = svcond->convertToExpr();
        this->notExpr = Expression::mkNot(this->expr);
    }

    /**
     * Return true if the "then" branch was taken, 
     * false if the "else" branch was taken.
     */
    bool getBranchTaken() { 
        return branchTaken; 
    }
    /**
     * Set which branch was taken by the br instruction.
     */
    void setBranchTaken(bool branch) { 
        branchTaken = branch; 
    }
    /**
     * Always return true since this cell 
     * holds a branch instruction.
     */
    virtual bool isBranch() { 
        return true; 
    }
    /**
     * Dump to the standard output the cell expression 
     * and the branch taken.
     */
    virtual void dump() {
        std::cout << "(BN: "; 
        expr->dump();
        std::cout << " )";
        if (branchTaken) {
            std::cout << " [T] ";
        } else {
            std::cout << " [F] ";
        }
    }
};

/**
 *\class CallExprCell
 *
 * \brief This class represents an element (cell) of a path constraint, 
 * which holds a call instruction. 
 */
class CallExprCell : public ExprCell {

protected:
    /**
     * Return the value of the function being call.
     */
    Value *vret;
    /**
     * Symbolic arguments of the function being call.
     */
    std::vector<SymbolPtr> sargs;
    /**
     *  Arguments (concrete values) of the function being call.
     */
    std::vector<Value*> vargs;

public:
    /**
     * Constructor (call with one argument).
     *
     * \param vret The return value of the related call instruction.
     * \param varg1 The argument value of the related call instruction.
     * \param i The call instruction related to this cell.
     */
    CallExprCell(Value *vret, Value *varg1, Instruction *i) 
    : ExprCell(i), vret(vret) { 
        vargs.push_back(varg1);
    }
    /**
     * Constructor (call with many arguments).
     *
     * \param vret The return value of the related call instruction.
     * \param vargs The argument values of the related call instruction.
     * \param i The call instruction related to this cell.
     */
    CallExprCell(Value *vret, std::vector<Value*> vargs, Instruction *i) 
    : ExprCell(i), vret(vret), vargs(vargs) { 
    }
    /**
     * Constructor (call with one symbolic arguement).
     *
     * \param vret The return value of the related call instruction.
     * \param sarg1 The symbolic argument of the related call instruction.
     * \param i The call instruction related to this cell.
     */
    CallExprCell(Value *vret, SymbolPtr sarg1, Instruction *i) 
    : ExprCell(i), vret(vret) {
        sargs.push_back(sarg1);
    }
    /**
     * Constructor (call with many symbolic arguments).
     *
     * \param vret The return value of the related call instruction.
     * \param sargs The symbolic arguments of the related call instruction.
     * \param i The call instruction related to this cell.
     */
    CallExprCell(Value *vret, std::vector<SymbolPtr> sargs, Instruction *i) 
    : ExprCell(i), vret(vret), sargs(sargs) { 
    }

    /**
     * Return the return value of the function being call.
     */
    Value* getRetValue() { 
        return vret; 
    }
    /**
     * Return the symbolic arguments of the function being call.
     */
    std::vector<SymbolPtr> getArgsSymbols() { 
        return sargs; 
    }
    /**
     * Return the arguments (concrete values) of the function being call.
     */
    std::vector<Value*> getArgsValues() { 
        return vargs; 
    }
    /**
     * Always return true since this cell 
     * holds a call instruction.
     */
    virtual bool isFunCall() { 
        return true; 
    }
    /**
     * \warning{This function cannot be called because 
     * a CallExprCell does not hold any expressions.}
     */
    ExprPtr getExpr() {
        std::cout << "error: no expression for function node!\n";
        exit(1);
    }
    /**
     * \warning{This function cannot be called because 
     * a CallExprCell does not hold any expressions.}
     */
    ExprPtr getNotExpr() {
        std::cout << "error: no expression for function node!\n";
        exit(1);
    }
    /**
     * Dump to the standard ouput the type of this cell.
     */
    virtual void dump() {
        std::cout << "(FCN: "; 
        std::cout << " )";
    }

};

/**
 *\class AssertExprCell
 *
 * \brief This class represents an element (cell) of a path constraint, 
 * which holds a call to a assert function (sniper_assert(bool)). 
 */
class AssertExprCell : public CallExprCell {

private:
    /**
     * Outcome of the assert function.
     */
    bool assertResult;

public:
    /**
     * Default constructor.
     *
     * \param sarg1 The symbolic value related to this cell.
     * \param assertResult The outcome of this assert function.
     * \param i An LLVM function call to an assert function.
     */
    AssertExprCell(SymbolPtr sarg1, bool assertResult, Instruction *i) 
    : CallExprCell(NULL, sarg1, i), assertResult(assertResult) {
        assert(i && "Expecting an instruction!");
        ExprPtr e = sarg1->convertToExpr();
        if (i->getType()->isIntegerTy(1)) {
            this->expr = e;
            this->notExpr = Expression::mkNot(e);
        } else {
            ExprPtr oneExpr = Expression::mkSInt32Num(1);
            this->expr      = Expression::mkEq(e, oneExpr);
            this->notExpr   = Expression::mkDiseq(e, oneExpr);
        }
    }

    /**
     * Return true if the outcome of the assert function is 
     * not failing, false owtherwise. 
     */
    bool getResult() { 
        return assertResult; 
    }
    /**
     * Always return true since this cell 
     * holds a call to an assert function.
     */
    virtual bool isAssert() { 
        return true; 
    } 
    /**
     * Dump to the standard output the cell expression 
     * and the assert outcome.
     */
    virtual void dump() {
        std::cout << "(AC: ";
        expr->dump();
        std::cout << " )";
        if (assertResult) {
            std::cout << " <T>";
        } else {
            std::cout << " <F>";
        }
    }
    
};

/**
 *\class AssumeExprCell
 *
 * \brief This class represents an element (cell) of a path constraint, 
 * which holds a call to a assume function (sniper_assume(bool)). 
 */
class AssumeExprCell : public CallExprCell {

private:
    /**
     * Outcome of the assume function.
     */
    bool assumeResult;

public:
    /**
     * Default constructor.
     *
     * \param sarg1 The symbolic value related to this cell.
     * \param assertResult The outcome of this assume function.
     * \param i An LLVM function call to an assume function.
     */
    AssumeExprCell(SymbolPtr sarg1, bool assumeResult, Instruction *i) 
    : CallExprCell(NULL, sarg1, i), assumeResult(assumeResult) { 
        this->expr =  sarg1->convertToExpr();
        this->notExpr = Expression::mkNot(this->expr);
    }

    /**
     * Return true if the outcome of the assume function is 
     * not failing, false owtherwise. 
     */
    bool getResult() { 
        return assumeResult; 
    }
    /**
     * Always return true since this cell 
     * holds a call to an assume function.
     */
    virtual bool isAssume() { 
        return true; 
    }
    /**
     * Dump to the standard output the cell expression 
     * and the assume outcome.
     */
    virtual void dump() {
        std::cout << "(AS: ";
        expr->dump();
        std::cout << " )";
        if (assumeResult) {
            std::cout << " <T>";
        } else {
            std::cout << " <F>";
        }
    }
    
};

#endif // _SYMBOLICPATH_H
