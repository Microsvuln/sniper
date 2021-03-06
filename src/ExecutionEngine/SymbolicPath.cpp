/**
 * \file SymbolicPath.cpp
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

#include "SymbolicPath.h"


std::vector<ExprCellPtr> SymbolicPath::getPath() {
    return path;
}

std::vector<ExprCellPtr> SymbolicPath::getAsserts() {
    return asserts;
}

std::vector<State_t> SymbolicPath::getStack() {
    return stack;
}

void SymbolicPath::setStack(std::vector<State_t> s) { 
    stack = s; 
}

unsigned SymbolicPath::getNbBranch() { 
    return nbBranch; 
}

// Add a node to the path.
void SymbolicPath::addCell(ExprCellPtr n) {
    // Record information about Branch nodes
    if (n->isBranch()) {
        // Get which branch we took
        BranchExprCellPtr bn = std::static_pointer_cast<BranchExprCell>(n);
        // Update the maps
        updateStack(bn->getBranchTaken());
        nbBranch++;
    }
    path.push_back(n);
}

void SymbolicPath::addBranchCell(SymbolPtr svcond, bool branchTaken,
                                 Instruction *i) {
    ExprCellPtr n = std::make_shared<BranchExprCell>(svcond,  branchTaken, i);
    addCell(n);
}

void SymbolicPath::addCallCell(Value *vret, SymbolPtr sarg1, Instruction *i) {
    ExprCellPtr n = std::make_shared<CallExprCell>(vret, sarg1, i);
    addCell(n);
}

void SymbolicPath::addAssertCell(SymbolPtr s, bool assertResult, Instruction *i) {
    ExprCellPtr n = std::make_shared<AssertExprCell>(s, assertResult, i);
    asserts.push_back(n);
}

void SymbolicPath::addAssumeCell(SymbolPtr s, bool assumeResult, Instruction *i) {
    ExprCellPtr n = std::make_shared<AssumeExprCell>(s, assumeResult, i);
    assumes.push_back(n);
}

void SymbolicPath::updateStack(bool branch) {
    // Update branch
    if (nbBranch<stack.size()) {
        // Wrong branch taken
        assert(stack[nbBranch].branch==branch && "Invalid stack!");
        if (nbBranch==(stack.size()-1)) {
            stack[nbBranch].branch = branch;
            stack[nbBranch].done = true;
        }   
    } 
    // New branch
    else {
        State_t s;
        s.branch = branch;
        s.done = false;
        stack.push_back(s);
    }
}

void SymbolicPath::clear() {
    path.clear();
    asserts.clear();
    assumes.clear();
    nbBranch = 0;
}

void SymbolicPath::clearStack() { 
    stack.clear(); 
}

void SymbolicPath::dump() {
    std::cout << "\n<Symbolic Path Dump>\n";
    std::vector<ExprCellPtr>::iterator it1;
    for (it1=path.begin(); it1!=path.end(); ++it1) {
        ExprCellPtr n = *it1;
        n->dump();
    }
    std::cout << std::endl;
}