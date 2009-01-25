/*
 * PUBLIC DOMAIN PCCTS-BASED C++ GRAMMAR (cplusplus.g, stat.g, expr.g)
 *
 * Authors: Sumana Srinivasan, NeXT Inc.;            sumana_srinivasan@next.com
 *          Terence Parr, Parr Research Corporation; parrt@parr-research.com
 *          Russell Quong, Purdue University;        quong@ecn.purdue.edu
 *
 * VERSION 1.1
 *
 * SOFTWARE RIGHTS
 *
 * This file is a part of the ANTLR-based C++ grammar and is free
 * software.  We do not reserve any LEGAL rights to its use or
 * distribution, but you may NOT claim ownership or authorship of this
 * grammar or support code.  An individual or company may otherwise do
 * whatever they wish with the grammar distributed herewith including the
 * incorporation of the grammar or the output generated by ANTLR into
 * commerical software.  You may redistribute in source or binary form
 * without payment of royalties to us as long as this header remains
 * in all source distributions.
 *
 * We encourage users to develop parsers/tools using this grammar.
 * In return, we ask that credit is given to us for developing this
 * grammar.  By "credit", we mean that if you incorporate our grammar or
 * the generated code into one of your programs (commercial product,
 * research project, or otherwise) that you acknowledge this fact in the
 * documentation, research report, etc....  In addition, you should say nice
 * things about us at every opportunity.
 *
 * As long as these guidelines are kept, we expect to continue enhancing
 * this grammar.  Feel free to send us enhancements, fixes, bug reports,
 * suggestions, or general words of encouragement at parrt@parr-research.com.
 * 
 * NeXT Computer Inc.
 * 900 Chesapeake Dr.
 * Redwood City, CA 94555
 * 12/02/1994
 * 
 * Restructured for public consumption by Terence Parr late February, 1995.
 *
 * DISCLAIMER: we make no guarantees that this grammar works, makes sense,
 *             or can be used to do anything useful.
 */
/*
 * 2001-2003 Version 2.0 September 2003
 *
 * Some modifications were made to this file to support a project by
 * Jianguo Zuo and David Wigg at
 * The Centre for Systems and Software Engineering
 * South Bank University
 * London, UK.
 * wiggjd@bcs.ac.uk
 * blackse@lsbu.ac.uk
*/
/* 2003-2004 Version 3.0 July 2004
 * Modified by David Wigg at London South Bank University for CPP_parser.g
 *
 * See MyReadMe.txt for further information
 *
 * This file is best viewed in courier font with tabs set to 4 spaces
 */

#include <iostream>
#include "CPPParser.hpp"
#include "CPPSymbol.hpp"

extern int statementTrace;	// Used to control selected (level) tracing

// Shorthand for a string of (qualifiedItemIs()==xxx||...)
int CPPParser::
qualifiedItemIsOneOf(QualifiedItem qiFlags,   // Ored combination of flags
					int lookahead_offset)
	{	
	QualifiedItem qi = qualifiedItemIs(lookahead_offset); 
	return (qi & qiFlags) != 0; 
	}

// This is an important function, but will be replaced with
// an enhanced predicate in the future, once predicates
// and/or predicate guards can contain loops.
//
// Scan past the ::T::B:: to see what lies beyond.
// Return qiType if the qualified item can pose as type name.
// Note that T::T is NOT a type; it is a constructor.  Also,
// class T { ... T...} yields the enclosed T as a ctor.  This
// is probably a type as I separate out the constructor defs/decls,
// I want it consistent with T::T.
//
// In the below examples, I use A,B,T and example types, and
// a,b as example ids.
// In the below examples, any A or B may be a
// qualified template, i.e.,  A<...>
//
// T::T outside of class T yields qiCtor.
// T<...>::T outside of class T yields qiCtor.
// T inside of class T {...} yields qiCtor.
// T, ::T, A::T outside of class T yields qiType.
// a, ::a,  A::B::a yields qiId
// a::b yields qiInvalid
// ::operator, operator, A::B::operator yield qiOPerator
// A::*, A::B::* yield qiPtrMember
// ::*, * yields qiInvalid
// ::~T, ~T, A::~T yield qiDtor
// ~a, ~A::a, A::~T::, ~T:: yield qiInvalid

CPPParser::QualifiedItem 
CPPParser::qualifiedItemIs(int lookahead_offset)
	{
	int k = lookahead_offset + 1;
	int final_type_idx = 0;
	bool scope_found = false;
	// Skip leading "::"
	if (LT(k)->getType() == SCOPE)
		{k++; scope_found = true;}
	// Skip sequences of T:: or T<...>::
	//printf("support.cpp qualifiedItemIs while reached k %d type %d isType %d, isClass %d, guessing %d\n",
	//	k,LT(k)->getType(),isTypeName((LT(k)->getText()).data()),isClassName((LT(k)->getText()).data()),inputState->guessing);
	while (LT(k)->getType() == ID && isTypeName((LT(k)->getText()).data()))
		{// If this type is the same as the last type, then ctor
		if (final_type_idx != 0 && 
			strcmp((LT(final_type_idx)->getText()).data(), (LT(k)->getText()).data()) == 0)
			{// Like T::T
			// As an extra check, do not allow T::T::
			if (LT(k+1)->getType() == SCOPE)
				{//printf("support.cpp qualifiedItemIs qiInvalid returned\n");
				return qiInvalid;
				} 
			else 
				{//printf("support.cpp qualifiedItemIs qiCtor returned\n");
				return qiCtor;
				}
			}

		// Record this as the most recent type seen in the series
		final_type_idx = k;
		
		//printf("support.cpp qualifiedItemIs if step reached final_type_idx %d\n",final_type_idx);
		
		// Skip this token
		k++;

		// Skip over any template qualifiers <...>
		// I believe that "T<..." cannot be anything valid but a template
		if (LT(k)->getType() == LESSTHAN)
			{
			if (!skipTemplateQualifiers(k))
				{//printf("support.cpp qualifiedItemIs qiInvalid(2) returned\n");
				return qiInvalid;}
			//printf("support.cpp qualifiedItemIs template skipped, k %d\n",k);
			// k has been updated to token following <...>
			}
		if (LT(k)->getType() == SCOPE)
			// Skip the "::" and keep going
			{k++; scope_found = true;}
		else 
			{// Series terminated -- last ID in the sequence was a type
			// Return ctor if last type is in containing class
			// We already checked for T::T inside loop
			if ( strcmp(enclosingClass, (LT(final_type_idx)->getText()).data())==0 ) 
				{// Like class T  T()
				//printf("support.cpp qualifiedItemIs qiCtor(2) returned\n");
				return qiCtor;
				} 
			else 
				{//printf("support.cpp qualifiedItemIs qiType returned\n");
				return qiType;}
			}
		}
	// LT(k) is not an ID, or it is an ID but not a typename.
	//printf("support.cpp qualifiedItemIs second switch reached\n");
	switch (LT(k)->getType())
		{
		case ID:
			// ID but not a typename
			// Do not allow id::
			if (LT(k+1)->getType() == SCOPE)
				{
				//printf("support.cpp qualifiedItemIs qiInvalid(3) returned\n");
				return qiInvalid;}
			if (strcmp(enclosingClass,(LT(k)->getText()).data())==0 ) 
				{// Like class T  T()
				//printf("support.cpp qualifiedItemIs qiCtor(3) returned\n");
				return qiCtor;
				}
			else 
				{
				if (scope_found)
					// DW 25/10/03 Assume type if at least one SCOPE present (test12.i)
					return qiType;
	            else 			
					//printf("support.cpp qualifiedItemIs qiVar returned\n");
					return qiVar;	// DW 19/03/04 was qiID Could be function?
				}
		case TILDE:
			// check for dtor
			if (LT(k+1)->getType() == ID && 
				isTypeName((LT(k+1)->getText()).data()) &&
				LT(k+2)->getType() != SCOPE)
				{// Like ~B or A::B::~B
				 // Also (incorrectly?) matches ::~A.
				//printf("support.cpp qualifiedItemIs qiDtor returned\n");
				return qiDtor;
				} 
			else 
				{// ~a or ~A::a is qiInvalid
				//printf("support.cpp qualifiedItemIs qiInvalid(4) returned\n");
				return qiInvalid;
				}
			break;
		case STAR:
			// Like A::*
			// Do not allow * or ::*
			if (final_type_idx == 0)
				{// Haven't seen a type yet
				//printf("support.cpp qualifiedItemIs qiInvalid(5) returned\n");
				return qiInvalid;
				} 
			else 
				{//printf("support.cpp qualifiedItemIs qiPtrMember returned\n");
				return qiPtrMember;}
		case OPERATOR:
			// Like A::operator, ::operator, or operator
			//printf("support.cpp qualifiedItemIs qiOperator returned\n");
			return qiOperator;
		default:
			// Something that neither starts with :: or ID, or
			// a :: not followed by ID, operator, ~, or *
			//printf("support.cpp qualifiedItemIs qiInvalid(6) returned\n");
			return qiInvalid;
		}
	}

// Skip over <...>.  This correctly handles nested <> and (), e.g:
//    <T>
//    < (i>3) >
//    < T2<...> >
// but not
//    < i>3 >
//
// On input, kInOut is the index of the "<"
// On output, if the return is true, then 
//                kInOut is the index of the token after ">"
//            else
//                kInOut is unchanged
int CPPParser::
skipTemplateQualifiers(int& kInOut)
	{// Start after "<"
	int k = kInOut + 1;

	while (LT(k)->getType() != GREATERTHAN) // scan to end of <...>
		{
		switch (LT(k)->getType())
			{
			case EOF:
				return 0;
			case LESSTHAN:
				if (!skipTemplateQualifiers(k))
					{return 0;}
				break;
			case LPAREN:
				if (!skipNestedParens(k))
					{return 0;}
				break;
			default:
				k++;     // skip everything else
				break;
			}
		if (k > MaxTemplateTokenScan)
			{return 0;}
		}

	// Update output argument to point past ">"
	kInOut = k + 1;
	return 1;
	}

// Skip over (...).  This correctly handles nested (), e.g:
//    (i>3, (i>5))
//
// On input, kInOut is the index of the "("
// On output, if the return is true, then 
//                kInOut is the index of the token after ")"
//            else
//                kInOut is unchanged
int CPPParser::
skipNestedParens(int& kInOut)
	{// Start after "("
	int k = kInOut + 1;

	while (LT(k)->getType() != RPAREN)   // scan to end of (...)
		{
		switch (LT(k)->getType())
			{
			case EOF:
				return 0;
			case LPAREN:
				if (!skipNestedParens(k))
					{return 0;}
				break;
			default:
				k++;     // skip everything else
				break;
			}
		if (k > MaxTemplateTokenScan)
			{return 0;}
		}

	// Update output argument to point past ")"
	kInOut = k + 1;
	return 1;
	}

// Return true if "::blah" or "fu::bar<args>::..." found.
int CPPParser::
scopedItem(int k)
	{
	//printf("support.cpp scopedItem k %d\n",k);
	return (LT(k)->getType()==SCOPE ||
			(LT(k)->getType()==ID && !finalQualifier(k)));
	}

// Return true if ID<...> or ID is last item in qualified item list.
// Return false if LT(k) is not an ID.
// ID must be a type to check for ID<...>,
// or else we would get confused by "i<3"
int CPPParser::
finalQualifier(int k)
	{
	if (LT(k)->getType()==ID)
		{
		if (isTypeName((LT(k)->getText()).data()) && LT(k+1)->getType()==LESSTHAN)
			{// Starts with "T<".  Skip <...>
			k++;
			skipTemplateQualifiers(k);
			} 
		else 
			{// skip ID;
			k++;
			}
		return (LT(k)->getType() != SCOPE );
		} 
	else 
		{// not an ID
		return 0;
		}
	}

/*
 * Return true if 's' can pose as a type name
 */
int CPPParser::
isTypeName(const char *s)
	{
	CPPSymbol *cs = (CPPSymbol *) symbols->lookup(s);

	if (cs==NULL)
		{
		//printf("support.cpp isTypeName %s not found\n",s);
		return 0;
		}

	if (cs->getType()==CPPSymbol::otTypedef||
		cs->getType()==CPPSymbol::otEnum||
		cs->getType()==CPPSymbol::otClass||
		cs->getType()==CPPSymbol::otStruct||
		cs->getType()==CPPSymbol::otUnion)
		{

		// DW 13/06/03 Check that scope level found is not greater 
		//  than the current scope i.e. lower level
//		if (cs->this_scope > symbols->getCurrentScopeIndex() )
//			{
//			return 0;
//			}
//
//		if (cs->getInScope()==false)
//			{
//			return 1;
//			}
		return 1;
		}

	return 0;
	}

/*
 * Return true if 's' is a class name (or a struct which is a class
 * with all members public).
 */
int CPPParser::
isClassName(const char *s)
	{
	CPPSymbol *cs = (CPPSymbol *) symbols->lookup(s);
	
	if ( cs==NULL ) return 0;
	
	if (cs->getType()==CPPSymbol::otClass||
		cs->getType()==CPPSymbol::otStruct||
		cs->getType()==CPPSymbol::otUnion) 
		{
		return 1;
		}

	return 0;
	}

void CPPParser::
beginDeclaration()
	{
	}

void CPPParser::
endDeclaration()
	{
	}

void CPPParser::
beginFunctionDefinition()
	{
	functionDefinition = 1;
	}

void CPPParser::
endFunctionDefinition()
	{
	// Remove parameter scope
	symbols->dumpScope(stdout);
	//fprintf(stdout, "endFunctionDefinition remove parameter scope(%d):\n",symbols->getCurrentScopeIndex());
	symbols->removeScope();
	symbols->restoreScope();
	//printf("endFunctionDefinition restoreScope() now %d\n",symbols->getCurrentScopeIndex());
	functionDefinition = 0;
	}

void CPPParser::
beginConstructorDefinition()
	{functionDefinition = 1;}

void CPPParser::
endConstructorDefinition()
	{
	symbols->dumpScope(stdout);
	symbols->removeScope();
	symbols->restoreScope();
	//printf("endConstructorDefinition restoreScope() now %d\n",symbols->getCurrentScopeIndex());
	functionDefinition = 0;
	}

void CPPParser::
beginConstructorDeclaration(const char *ctor)
	{ }

void CPPParser::
endConstructorDeclaration()
	{ }

void CPPParser::
beginDestructorDefinition()
	{functionDefinition = 1;}

void CPPParser::
endDestructorDefinition()
	{
	symbols->dumpScope(stdout);
	symbols->removeScope();
	symbols->restoreScope();
	//printf("endDestructorDefinition restoreScope() now %d\n",symbols->getCurrentScopeIndex());
	functionDefinition = 0;
	}

void CPPParser::
beginDestructorDeclaration(const char *dtor)
	{ }

void CPPParser::
endDestructorDeclaration()
	{ }

void CPPParser::
beginParameterDeclaration()
	{ }

void CPPParser::
beginFieldDeclaration()
	{ }

static CPPParser::StorageClass
get_storage_class(const char *s)
{
	CPPParser::StorageClass sc;

	if(s == NULL)
		sc = CPPParser::scInvalid;
	else if(strcmp(s, "auto") == 0)
		sc = CPPParser::scAUTO;
	else if (strcmp(s, "register") == 0)
		sc = CPPParser::scREGISTER;
	else if (strcmp(s, "static") == 0)
		sc = CPPParser::scSTATIC;
	else if (strcmp(s, "extern") == 0)
		sc = CPPParser::scEXTERN;
	else if (strcmp(s, "mutable") == 0)
		sc = CPPParser::scMUTABLE;
	else
		sc = CPPParser::scInvalid;

	return sc;
}

static CPPParser::TypeQualifier
get_type_qualifier(const char *s)
{
	CPPParser::TypeQualifier tq;

	if(s == NULL)
		tq = CPPParser::tqInvalid;
	else if(strcmp(s, "const") == 0)
		tq = CPPParser::tqCONST;
	else if (strcmp(s, "volatile") == 0)
		tq = CPPParser::tqVOLATILE;
	else if (strcmp(s, "cdecl") == 0)
		tq = CPPParser::tqCDECL;
	else
		tq = CPPParser::tqInvalid;

	return tq;
}

static CPPParser::TypeSpecifier
get_type_specifier(ASTNode *node)
{
	CPPParser::TypeSpecifier ts = tsInvalid;

	if(node->has_child("char"))
		ts |= tsCHAR;
	else if (node->has_child("void"))
		ts |= tsVOID;
	else if (node->has_child("short"))
		ts |= tsSHORT;
	else if (node->has_child("int"))
		ts |= tsINT;
	else if (node->has_child("long"))
		ts |= tsLONG;
	else if (node->has_child("float"))
		ts |= tsFLOAT;
	else if (node->has_child("double"))
		ts |= tsDOUBLE;
	else if (node->has_child("signed"))
		ts |= tsSIGNED;
	else if (node->has_child("unsigned"))
		ts |= tsUNSIGNED;
	else if (node->has_child("typeid"))
		ts |= tsTYPEID;
	else if (node->has_child("struct"))
		ts |= tsSTRUCT;
	else if (node->has_child("enum"))
		ts |= tsENUM;
	else if (node->has_child("union"))
		ts |= tsUNION;
	else if (node->has_child("class"))
		ts |= tsCLASS;
	else if (node->has_child("wchar_t"))
		ts |= tsWCHAR_T;
	else if (node->has_child("bool"))
		ts |= tsBOOL;

	return ts;
}


void CPPParser::
declarationSpecifier(bool td, bool fd, const char *sc, const char *tq,
			 ASTNode *ts, DeclSpecifier ds)
	{
	//printf("support.cpp declarationSpecifier td %d, fd %d, sc %d, tq, %d, ts %d, ds %d, qi %d\n",
	//	td,fd,sc,tq,ts,ds,qi);
	_td = td;	// For typedef
	_fd = fd;	// For friend
	_sc = get_storage_class(sc);
	_tq = get_type_qualifier(tq);
	_ts = get_type_specifier(ts);
	_ds = ds;
//	_qi = qi;
	}

/* Symbols from declarators are added to the symbol table here. 
 * The symbol is added to whatever the current scope is in the symbol table. 
 * See list of object types below.
 */
void CPPParser::
declaratorID(const char *id,QualifiedItem qi)
	{
	//printf("declaratorID  entered for %s\n",id);
	CPPSymbol *c;
	//printf("support.cpp declaratorID line %d %s found, _td = %d, qi = %d\n",LT(1)->getLine(),id,_td,qi);
	if (qi==qiType || _td)	// Check for type declaration
		{
		// DW 21/05/04 Check consistency of local qi with global _td with view to removing test for global _td
		if (!(qi==qiType && _td))
			printf("%d support.cpp declaratorID warning qi %d, _td %d inconsistent for %s\n",
				LT(1)->getLine(),qi,_td,id);
		
		c = new CPPSymbol(id, CPPSymbol::otTypedef);
		if (c==NULL) panic("can't alloc CPPSymbol");
		symbols->defineInScope(id, c, externalScope);
		if(statementTrace >= 2)
			printf("%d support.cpp declaratorID declare %s in external scope 1, ObjectType %d\n",
				LT(1)->getLine(),id,c->getType());
		// DW 04/08/03 Scoping not fully implemented
		// Typedefs all recorded in 'external' scope and therefor never removed
		}
	else if (qi==qiFun)	// For function declaration
		{
		c = new CPPSymbol(id, CPPSymbol::otFunction);
		if (c==NULL) panic("can't alloc CPPSymbol");
		symbols->define(id, c);	// Add to current scope
		if(statementTrace >= 2)
			printf("%d support.cpp declaratorID declare %s in current scope %d, ObjectType %d\n",
				LT(1)->getLine(),id,symbols->getCurrentScopeIndex(),c->getType());
		}
	else	   
		{
		if (qi!=qiVar)
			printf("%d support.cpp declaratorID warning qi (%d) not qiVar (%d) for %s\n",
				LT(1)->getLine(),qi,qiVar,id); 

		c = new CPPSymbol(id, CPPSymbol::otVariable);	// Used to leave otInvalid DW 02/07/03 Think this should be otVariable
		if (c==NULL) panic("can't alloc CPPSymbol");
		symbols->define(id, c);	// Add to current scope
		if(statementTrace >= 2)
			printf("%d support.cpp declaratorID declare %s in current scope %d, ObjectType %d\n",
				LT(1)->getLine(),id,symbols->getCurrentScopeIndex(),c->getType());
		}
	}
/* These are the object types
	0 = otInvalid
	1 = otFunction
	2 = otVariable
	3 = otTypedef	Note. 3-7 are type names
	4 = otStruct	Note. 4, 5 & 7 are class names
	5 = otUnion
	6 = otEnum
	7 = otClass
	8 = otEnumElement
*/

void CPPParser::
declaratorArray()
	{ }

void CPPParser::
declaratorParameterList(int def)
	{
	symbols->saveScope();
	//printf("declaratorParameterList saveScope() now %d\n",symbols->getCurrentScopeIndex());
	}

void CPPParser::
declaratorEndParameterList(int def)
	{
	if (!def)
		{
		symbols->dumpScope(stdout);
		symbols->removeScope();
		symbols->restoreScope();
		//printf("declaratorEndParameterList restoreScope() now %d\n",symbols->getCurrentScopeIndex());
		}
	}

void CPPParser::
functionParameterList()
	{
	symbols->saveScope();
	//printf("functionParameterList saveScope() now %d\n",symbols->getCurrentScopeIndex());
	// DW 25/3/97 change flag from function to parameter list
	functionDefinition = 2;
	}

void CPPParser::
functionEndParameterList(int def)
	{
	// If this parameter list is not in a definition then this
	if ( !def)
		{
		symbols->dumpScope(stdout);
		symbols->removeScope();
		symbols->restoreScope();
		//printf("functionEndParameterList restoreScope() now %d\n",symbols->getCurrentScopeIndex());
		}
	else
		{
		// Change flag from parameter list to body of definition
		functionDefinition = 3;   
		}
	/* Otherwise endFunctionDefinition removes the parameters from scope */
	}

void CPPParser::
enterNewLocalScope()
	{
	symbols->saveScope();
	//printf("enterNewLocalScope saveScope() now %d\n",symbols->getCurrentScopeIndex());
	}

void CPPParser::
exitLocalScope()
	{
	symbols->dumpScope(stdout);
	symbols->removeScope();
	symbols->restoreScope();
	//printf("exitLocalScope restoreScope() now %d\n",symbols->getCurrentScopeIndex());
	}

void CPPParser::
enterExternalScope()
	{// Scope has been initialised to 1 in CPPParser.init() in CPPParser.hpp
	// DW 25/3/97 initialise
	functionDefinition = 0;
	//classDefinition = 0;	DW 19/03/04 not used anywhere
	}

void CPPParser::
exitExternalScope()
	{
	symbols->dumpScope(stdout);
	symbols->removeScope();		// This just removes the symbols stored in the current scope
	symbols->restoreScope();	// This just reduces the current scope by 1
	/*if (symbols->getCurrentScopeIndex()==0)
		printf("\nSupport exitExternalScope, scope now %d\n",symbols->getCurrentScopeIndex());
	else
		printf("\nSupport exitExternalScope, scope now %d, should be 0\n",symbols->getCurrentScopeIndex());
	*/
	}

void CPPParser::
classForwardDeclaration(TypeSpecifier ts, DeclSpecifier ds, const char *tag)
	{
	CPPSymbol *c = NULL;

	// if already in symbol table as a class, don't add
	// of course, this is incorrect as you can rename
	// classes by entering a new scope, but our limited
	// example basically keeps all types globally visible.
	if ( symbols->lookup(tag)!=NULL )
		{
		CPPSymbol *cs = (CPPSymbol *) symbols->lookup(tag);
		if (statementTrace >= 2)
			printf("%d support.cpp classForwardDeclaration %s already stored in dictionary, ObjectType %d\n",
				LT(1)->getLine(),tag,cs->getType());
		return;
		}

	switch (ts)
		{
		case tsSTRUCT :
			c = new CPPSymbol(tag, CPPSymbol::otStruct);
			break;
		case tsUNION :
			c = new CPPSymbol(tag, CPPSymbol::otUnion);
			break;
		case tsCLASS :
			c = new CPPSymbol(tag, CPPSymbol::otClass);
			break;
		default:
			printf("Yuck - unknown type specifier in class forward decl!\n");
			break;
		}

	if ( c==NULL ) panic("can't alloc CPPSymbol");

	symbols->defineInScope(tag, c, externalScope);
	if (statementTrace >= 2)
		printf("%d support.cpp classForwardDeclaration declare %s in external scope, ObjectType %d\n",
			LT(1)->getLine(),tag,c->getType());

	// If it's a friend class forward decl, put in global scope also.
	// DW 04/07/03 No need if already in external scope. See above.
	//if ( ds==dsFRIEND )
	//	{
	//	CPPSymbol *ext_c = new CPPSymbol(tag, CPPSymbol::otClass);
	//	if ( ext_c==NULL ) panic("can't alloc CPPSymbol");
	//	if ( symbols->getCurrentScopeIndex()!=externalScope )	// DW 04/07/03 Not sure this test is really necessary
	//		{
	//		printf("classForwardDeclaration defineInScope(externalScope)\n");
	//		symbols->defineInScope(tag, ext_c, externalScope);
	//		}
	//	}

	}

void CPPParser::
beginClassDefinition(TypeSpecifier ts, const char *tag)
	{
	CPPSymbol *c = NULL;

	// if already in symbol table as a class, don't add
	// of course, this is incorrect as you can rename
	// classes by entering a new scope, but our limited
	// example basically keeps all types globally visible.
	if (symbols->lookup(tag) != NULL)
		{
		symbols->saveScope();   // still have to use scope to collect members
		//printf("support.cpp beginClassDefinition_1 saveScope() now %d\n",symbols->getCurrentScopeIndex());
		if (statementTrace >= 2)
			printf("%d support.cpp beginClassDefinition classname %s already in dictionary\n",
				LT(1)->getLine(),tag);
		return;
		}

	switch (ts)
		{
		case tsSTRUCT:
			c = new CPPSymbol(tag, CPPSymbol::otStruct);
			break;
		case tsUNION:
			c = new CPPSymbol(tag, CPPSymbol::otUnion);
			break;
		case tsCLASS:
			c = new CPPSymbol(tag, CPPSymbol::otClass);
			break;
		default:
			printf("Yuck - unknown type specifier in class forward decl!\n");
			break;
		}
	if (c==NULL) panic("can't alloc CPPSymbol");
	symbols->defineInScope(tag, c, externalScope);
	if (statementTrace >= 2)
		printf("%d support.cpp beginClassDefinition define %s in external scope (1), ObjectType %d\n",
			LT(1)->getLine(),tag,c->getType());
	strcat(qualifierPrefix, tag);
	strcat(qualifierPrefix, "::");

	// add all member type symbols into the global scope (not correct, but
	// will work for most code).
	// This symbol lives until the end of the file

	symbols->saveScope();   // use the scope to collect list of fields
	}

void CPPParser::
endClassDefinition()
	{
	symbols->dumpScope(stdout);
	symbols->removeScope();
	symbols->restoreScope();
	// remove final T:: from A::B::C::T::
	// upon backing out of last class, qualifierPrefix is set to ""
	char *p = &(qualifierPrefix[strlen(qualifierPrefix)-3]);
	while ( p > &(qualifierPrefix[0]) && *p!=':' ) 
		{p--;}
	if ( p > &(qualifierPrefix[0]) ) 
		p++;
	*p = '\0';
	}

void CPPParser::
enumElement(const char *e)
	{ }

void CPPParser::
beginEnumDefinition(const char *e)
	{// DW 26/3/97 Set flag for new class
	
	// add all enum tags into the global scope (not correct, but
	// will work for most code).
	// This symbol lives until the end of the file
	CPPSymbol *c = new CPPSymbol(e, CPPSymbol::otEnum);
	symbols->defineInScope(e, c, externalScope);
	if (statementTrace >= 2)
		printf("%d support.cpp beginEnumDefinition %s define in external scope, ObjectType %d\n",
			LT(1)->getLine(),e,c->getType());
	}

void CPPParser::
endEnumDefinition()
	{	
	}

void CPPParser::
templateTypeParameter(const char *t)
	{
	//DW 11/06/03 Symbol saved in templateParameterScope (0)
	//  as a temporary measure until scope is implemented fully
	// This symbol lives until the end of the file
	CPPSymbol *e = new CPPSymbol(t, CPPSymbol::otTypedef);
	if (e==NULL) panic("can't alloc CPPSymbol");
	symbols->defineInScope(t, e, templateParameterScope);
	if (statementTrace >= 2)
		printf("%d support.cpp templateTypeParameter declare %s in template parameter scope (0), ObjectType %d\n",
			LT(1)->getLine(),t,e->getType());
	}

void CPPParser::
beginTemplateDeclaration()
	{
	}

void CPPParser::
endTemplateDeclaration()
	{// DW 30/05/03 Remove any template typenames from lower scope
	symbols->saveScope();
	//printf("endTemplateDeclaration saveScope() now %d\n",symbols->getCurrentScopeIndex());
	//symbols->dumpScope(stdout);
	symbols->removeScope();
	symbols->restoreScope();
	//printf("endTemplateDeclaration restoreScope() now %d\n",symbols->getCurrentScopeIndex());
	}

void CPPParser::
beginTemplateDefinition()
	{
	}

void CPPParser::
endTemplateDefinition()
	{
	}

void CPPParser::
beginTemplateParameterList()
	{ 
	// DW 26/05/03 To scope template parameters
	symbols->saveScope();	// All this does is increase currentScope (lower level)
	}

void CPPParser::
endTemplateParameterList()
	{ 
	// DW 26/05/03 To end scope template parameters
	symbols->restoreScope();	// All this does is reduce currentScope (higher level)
	}

void CPPParser::
exceptionBeginHandler()
	{ }

void CPPParser::
exceptionEndHandler()
	{// remove parm elements from the handler scope
	symbols->dumpScope(stdout);
	symbols->removeScope();
	symbols->restoreScope();
	}

void CPPParser::
end_of_stmt()
	{
#ifdef MYCODE
	myCode_end_of_stmt();
#endif //MYCODE
	}                    

void CPPParser::
panic(const char *err)
	{
	fprintf(stdout, "CPPParser: %s\n", err);
	exit(-1);
	}

/* DW 090204 leave out of support.cpp for the time being
// want to see how supplied routine in LLkParser works first
void CPPParser::
tracein(char *r)
{
// testing
	printf("support tracein entered\n");
	getchar();
   if ( !doTracing ) return;
   for (int i=1; i<=traceIndentLevel; i++)
	   fprintf(stdout, " ");
   traceIndentLevel++;
   fprintf(stdout, "enter %s('%s %s')%s\n",
      r,
      (LT(1)->getText()).data(),
      (LT(2)->getText()).data(),
      inputState->guessing?" [guessing]":""
   );
   fflush(stdout);
}

void CPPParser::
traceout(char *r)
{
   if ( !doTracing ) return;
   traceIndentLevel--;
   for (int i=1; i<=traceIndentLevel; i++)
	   fprintf(stdout, " ");
   fprintf(stdout, "exit %s('%s %s')%s\n",
      r,
      (LT(1)->getText()).data(),
      (LT(2)->getText()).data(),
      inputState->guessing?" [guessing]":""
   );
   fflush(stdout);
}
*/

//	Functions which may be overridden by MyCode subclass

void CPPParser::
myCode_pre_processing(int argc,char *argv[])
	{
	}

void CPPParser::
myCode_post_processing()
	{
	}

void CPPParser::
myCode_end_of_stmt()
	{
	}

void CPPParser::
myCode_function_direct_declarator(const char *id)
	{
	//printf("support myCode_function_direct_declarator entered for %s\n",id);
	}

ASTNode *
CPPParser::mk_node(const char *name)
{
	ASTNode *newnode;
	newnode = new ASTNode(name);

	if(inputState->guessing) {
		/* Remember this so we can throw it away later */
		if (temp_nodes_next >= temp_nodes_end) {
			temp_nodes_end += TEMP_NODES_ALLOC;
			temp_nodes = (ASTNode **)realloc(temp_nodes, temp_nodes_end * sizeof(ASTNode *));
		}
		temp_nodes[temp_nodes_next] = newnode;
		temp_nodes_next ++;
	}

	return newnode;
}

void
CPPParser::free_temp_nodes(void)
{
	int i;

	for(i = 0; i < temp_nodes_next; i++) {
		temp_nodes[i]->forget_children();
		delete temp_nodes[i];
	}

	free(temp_nodes);
	temp_nodes = NULL;
}

ASTNode *
CPPParser::exprnode_acc(ASTNode *node, ASTNode *child, const char *leaf)
{
	if(node == NULL)
		node = mk_node("unfinished_expression_node");
	if(!(node->has_leaf(leaf))) {
		ASTNode *new_parent = mk_node("expression");
		new_parent->add_child(node);
		new_parent->add_child(child);
		new_parent->set_leaf(leaf);
		node = new_parent;
	} else {
		node->add_child(child);
	}
	return node;
}

