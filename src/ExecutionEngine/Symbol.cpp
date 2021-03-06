/**
 * \file Symbol.cpp
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
 * \date   23 February 2016
 */

#include "Symbol.h"


unsigned Symbol::ID = 0;


ExprPtr Symbol::convertToExpr() {
    return Expression::getExprFromValue(v);
}

// static
void Symbol::dump(Value *v) {
    if (ConstantInt *CI = dyn_cast<ConstantInt>(v)) {
        int val = CI->getSExtValue();
        std::cout << val;
    }
    else if (isa<GlobalVariable>(v)) {
        std::cout << v->getName().str();
    } else {
        std::string fname = "U";
        Value::use_iterator it = v->use_begin();
        if (Instruction *I = dyn_cast<Instruction>(*it)) {
            Function *F = I->getParent()->getParent();
            fname = F->getName().str();
        }
        std::cout << v->getName().str() << "_" << fname;
    }  
}

// static 
std::string Symbol::getPredicateStr(CmpInst::Predicate p) {
    switch (p) {
        case CmpInst::FCMP_UEQ: // True if unordered or equal
        case CmpInst::FCMP_OEQ: // True if ordered and equal
        case CmpInst::ICMP_EQ:  // equal
            return "=";
        case CmpInst::FCMP_ONE: // True if ordered and operands are unequal
        case CmpInst::FCMP_UNE: // True if unordered or not equal
        case CmpInst::ICMP_NE:  // not equal
            return "!=";
        case CmpInst::FCMP_UGT: // True if unordered or greater than
        case CmpInst::FCMP_OGT: // True if ordered and greater than
        case CmpInst::ICMP_UGT: // unsigned greater than
        case CmpInst::ICMP_SGT: // signed greater than
            return ">";
        case CmpInst::FCMP_UGE: // True if unordered, greater than, or equal
        case CmpInst::FCMP_OGE: // True if ordered and greater than or equal
        case CmpInst::ICMP_UGE: // unsigned greater or equal
        case CmpInst::ICMP_SGE: // signed greater or equal
            return ">=";
        case CmpInst::FCMP_ULT: //  True if unordered or less than
        case CmpInst::FCMP_OLT: // True if ordered and less than
        case CmpInst::ICMP_ULT: // unsigned less than
        case CmpInst::ICMP_SLT: // signed less than
            return "<";
        case CmpInst::FCMP_OLE: // True if ordered and less than or equal
        case CmpInst::FCMP_ULE: // True if unordered, less than, or equal
        case CmpInst::ICMP_ULE: // unsigned less or equal
        case CmpInst::ICMP_SLE: // signed less or equal
            return "<=";
            /*case CmpInst::FCMP_UNO: // True if unordered: isnan(X) | isnan(Y)
             case CmpInst::FCMP_ORD: // True if ordered (no nans)
             case CmpInst::FCMP_FALSE: // Always false (always folded)
             case CmpInst::FCMP_TRUE: //  Always true (always folded)
             case CmpInst::FIRST_FCMP_PREDICATE: // = FCMP_FALSE
             case CmpInst::LAST_FCMP_PREDICATE:  // = FCMP_TRUE
             case CmpInst::BAD_FCMP_PREDICATE: // = FCMP_TRUE + 1
             case CmpInst::FIRST_ICMP_PREDICATE: // = ICMP_EQ
             case CmpInst::LAST_ICMP_PREDICATE: // = ICMP_SLE
             case CmpInst::BAD_ICMP_PREDICATE: // = ICMP_SLE + 1 */
        default:
            return "op";
    }
}
