/* ###
 * IP: GHIDRA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
%{
#include "slgh_compile.h"

#define YYERROR_VERBOSE
#define YYMALLOC
#define YYFREE

  extern SleighCompile *slgh;
  extern int4 actionon;
  extern FILE *yyin;
  extern int yydebug;
  extern int yylex(void);
  extern int yyerror(const char *str, ...);
%}

%union {
  char ch;
  intb i;
  intb big;
  CString *str;
  //vector<string> *strlist;
  struct dynarray   *strlist;
  // vector<intb> *biglist;
  struct dynarray   *biglist;
  //vector<ExpTree *> *param;
  struct dynarray   *param;
  SpaceQuality *spacequal;
  FieldQuality *fieldqual;
  StarQuality *starqual;
  VarnodeTpl *varnode;
  ExpTree *tree;
  struct dynarray  *stmt;
  ConstructTpl *sem;
  SectionVector *sectionstart;
  Constructor *construct;
  PatternEquation *pateq;
  PatternExpression *patexp;

  //vector<SleighSymbol *> *symlist;
  struct dynarray   *symlist;
  //vector<ContextChange *> *contop;
  struct dynarray  *contop;
  SleighSymbol *anysym;
  SpaceSymbol *spacesym;
  SectionSymbol *sectionsym;
  TokenSymbol *tokensym;
  UserOpSymbol *useropsym;
  MacroSymbol *macrosym;
  LabelSymbol *labelsym;
  SubtableSymbol *subtablesym;
  StartSymbol *startsym;
  EndSymbol *endsym;
  OperandSymbol *operandsym;
  VarnodeListSymbol *varlistsym;
  VarnodeSymbol *varsym;
  BitrangeSymbol *bitsym;
  NameSymbol *namesym;
  ValueSymbol *valuesym;
  ValueMapSymbol *valuemapsym;
  ContextSymbol *contextsym;
  FamilySymbol *famsym;
  SpecificSymbol *specsym;
}

%expect 5
// Conflicts
// 2 charstring conflicts          (do we lump CHARs together before appending to constructprint)
//     resolved by shifting which lumps before appending (best solution)
// 1 integervarnode ':' conflict   (does ':' apply to INTEGER or varnode)
//     resolved by shifting which applies ':' to INTEGER (best solution)
// 2 statement -> STRING . conflicts (STRING might be mislabelled varnode, or temporary declaration)
//     resolved by shifting which means assume this is a temporary declaration

%left OP_BOOL_OR
%left OP_BOOL_AND OP_BOOL_XOR
%left '|' OP_OR
%left ';'
%left '^' OP_XOR
%left '&' OP_AND
%left OP_EQUAL OP_NOTEQUAL OP_FEQUAL OP_FNOTEQUAL
%nonassoc '<' '>' OP_GREATEQUAL OP_LESSEQUAL OP_SLESS OP_SGREATEQUAL OP_SLESSEQUAL OP_SGREAT OP_FLESS OP_FGREAT OP_FLESSEQUAL OP_FGREATEQUAL
%left OP_LEFT OP_RIGHT OP_SRIGHT
%left '+' '-' OP_FADD OP_FSUB
%left '*' '/' '%' OP_SDIV OP_SREM OP_FMULT OP_FDIV
%right '!' '~'
%token OP_ZEXT OP_CARRY OP_BORROW OP_SEXT OP_SCARRY OP_SBORROW OP_NAN OP_ABS
%token OP_SQRT OP_CEIL OP_FLOOR OP_ROUND OP_INT2FLOAT OP_FLOAT2FLOAT
%token OP_TRUNC OP_CPOOLREF OP_NEW OP_POPCOUNT

%token BADINTEGER GOTO_KEY CALL_KEY RETURN_KEY IF_KEY
%token DEFINE_KEY ATTACH_KEY MACRO_KEY SPACE_KEY TYPE_KEY RAM_KEY DEFAULT_KEY
%token REGISTER_KEY ENDIAN_KEY WITH_KEY ALIGN_KEY OP_UNIMPL
%token TOKEN_KEY SIGNED_KEY NOFLOW_KEY HEX_KEY DEC_KEY BIG_KEY LITTLE_KEY
%token SIZE_KEY WORDSIZE_KEY OFFSET_KEY NAMES_KEY VALUES_KEY VARIABLES_KEY PCODEOP_KEY IS_KEY LOCAL_KEY
%token DELAYSLOT_KEY CROSSBUILD_KEY EXPORT_KEY BUILD_KEY CONTEXT_KEY ELLIPSIS_KEY GLOBALSET_KEY BITRANGE_KEY

%token <ch> CH
%token <i> INTEGER
%token <big> INTB
%token <str> STRING SYMBOLSTRING
%token <spacesym> SPACESYM
%token <sectionsym> SECTIONSYM
%token <tokensym> TOKENSYM
%token <useropsym> USEROPSYM
%token <valuesym> VALUESYM
%token <valuemapsym> VALUEMAPSYM
%token <contextsym> CONTEXTSYM
%token <namesym> NAMESYM
%token <varsym> VARSYM
%token <bitsym> BITSYM
%token <specsym> SPECSYM
%token <varlistsym> VARLISTSYM
%token <operandsym> OPERANDSYM
%token <startsym> STARTSYM
%token <endsym> ENDSYM
%token <macrosym> MACROSYM
%token <labelsym> LABELSYM
%token <subtablesym> SUBTABLESYM

%type <macrosym> macrostart
%type <param> paramlist
%type <sem> rtl rtlmid
%type <sectionstart> rtlbody rtlfirstsection rtlcontinue
%type <stmt> statement
%type <tree> expr
%type <varnode> varnode integervarnode exportvarnode lhsvarnode jumpdest
%type <labelsym> label
%type <pateq> pequation bitpat_or_nil elleq ellrt atomic constraint
%type <patexp> pexpression
%type <str> charstring
%type <construct> constructprint subtablestart
%type <sectionsym> section_def
%type <varsym> contextprop
%type <tokensym> tokenprop
%type <spacequal> spaceprop
%type <fieldqual> fielddef contextfielddef
%type <starqual> sizedstar
%type <strlist> stringlist stringpart anystringlist anystringpart oplist
%type <biglist> intblist intbpart
%type <symlist> valuelist valuepart varlist varpart
%type <contop> contextlist contextblock
%type <anysym> anysymbol
%type <famsym> familysymbol
%type <specsym> specificsymbol
%type <subtablesym> id_or_nil

%%
spec: endiandef
  | spec aligndef
  | spec definition
  | spec constructorlike
  ;
definition: tokendef
  | contextdef
  | spacedef
  | varnodedef
  | bitrangedef
  | pcodeopdef
  | valueattach
  | nameattach
  | varattach
  | error ';'
  ;
constructorlike: constructor
  | macrodef
  | withblock
  | error '}'                          { SleighCompile_resetConstructor(slgh); }
  ;
endiandef: DEFINE_KEY ENDIAN_KEY '=' BIG_KEY ';' { SleighCompile_setEndian(slgh, 1); }
  | DEFINE_KEY ENDIAN_KEY '=' LITTLE_KEY ';' { SleighCompile_setEndian(slgh, 0); }
  ;
aligndef: DEFINE_KEY ALIGN_KEY '=' INTEGER ';' { SleighCompile_setAlignment(slgh, (int)$4); }
  ;
tokendef: tokenprop ';'                {}
  ;
tokenprop: DEFINE_KEY TOKEN_KEY STRING '(' INTEGER ')' { $$ = SleighCompile_defineToken(slgh, $3->data, $5); }
  | tokenprop fielddef		       { $$ = $1; SleighCompile_addTokenField(slgh, $1, $2); }
  | DEFINE_KEY TOKEN_KEY anysymbol     { yyerror("%s: redefined as a token", SleighSymbol_getName($3)); YYERROR; }
  ;
contextdef: contextprop ';'            {}
  ;
contextprop: DEFINE_KEY CONTEXT_KEY VARSYM { $$ = $3; }
  | contextprop contextfielddef		 { $$ = $1; if (!SleighCompile_addContextField(slgh, $1, $2))
                                            { yyerror("All context definitions must come before constructors"); YYERROR; } }
  ;
fielddef: STRING '=' '(' INTEGER ',' INTEGER ')' { $$ = FieldQuality_new($1->data,$4,$6); }
  | anysymbol '=' '(' INTEGER ',' INTEGER ')' { yyerror("%s: redefined as field", SleighSymbol_getName($1)); YYERROR; }
  | fielddef SIGNED_KEY			{ $$ = $1; $$->signext = true; }
  | fielddef HEX_KEY			{ $$ = $1; $$->hex = true; }
  | fielddef DEC_KEY			{ $$ = $1; $$->hex = false; }
  ;
contextfielddef: STRING '=' '(' INTEGER ',' INTEGER ')' { $$ = FieldQuality_new($1->data,$4,$6); }
  | anysymbol '=' '(' INTEGER ',' INTEGER ')' { yyerror("%s: redefined as field", SleighSymbol_getName($1)); YYERROR; }
  | contextfielddef SIGNED_KEY			{ $$ = $1; $$->signext = true; }
  | contextfielddef NOFLOW_KEY			{ $$ = $1; $$->flow = false; }
  | contextfielddef HEX_KEY			{ $$ = $1; $$->hex = true; }
  | contextfielddef DEC_KEY			{ $$ = $1; $$->hex = false; }
  ;
spacedef: spaceprop ';'			{ SleighCompile_newSpace(slgh, $1); SpaceQuality_delete($1); }
  ;
spaceprop: DEFINE_KEY SPACE_KEY STRING	{ $$ = SpaceQuality_new($3->data); cstr_delete($3); }
  | DEFINE_KEY SPACE_KEY anysymbol	{ yyerror("%s: redefined as space", SleighSymbol_getName($3)); YYERROR; }
  | spaceprop TYPE_KEY '=' RAM_KEY	{ $$ = $1; $$->type = RAMTYPE; }
  | spaceprop TYPE_KEY '=' REGISTER_KEY { $$ = $1; $$->type = REGISTERTYPE; }
  | spaceprop SIZE_KEY '=' INTEGER	{ $$ = $1; $$->size = (uint4)$4; }
  | spaceprop WORDSIZE_KEY '=' INTEGER	{ $$ = $1; $$->wordsize = (uint4)$4; }
  | spaceprop DEFAULT_KEY               { $$ = $1; $$->isdefault = true; }
  ;
varnodedef: DEFINE_KEY SPACESYM OFFSET_KEY '=' INTEGER SIZE_KEY '=' INTEGER stringlist ';' {
               SleighCompile_defineVarnodes(slgh, $2, $5, $8, $9); dynarray_delete($9); }
  | DEFINE_KEY SPACESYM OFFSET_KEY '=' BADINTEGER { yyerror("Parsed integer is too big (overflow)"); YYERROR; }
  ;
bitrangedef: DEFINE_KEY BITRANGE_KEY bitrangelist ';'
  ;
bitrangelist: bitrangesingle
  | bitrangelist bitrangesingle
  ;
bitrangesingle: STRING '=' VARSYM '[' INTEGER ',' INTEGER ']' {
               SleighCompile_defineBitrange(slgh, $1->data,$3,(uint4)$5,(uint4)$7); 
               }
  ;
pcodeopdef: DEFINE_KEY PCODEOP_KEY stringlist ';' { SleighCompile_addUserOp(slgh, $3); }
  ;
valueattach: ATTACH_KEY VALUES_KEY valuelist intblist ';' { SleighCompile_attachValues(slgh, $3, $4); }
  ;
nameattach: ATTACH_KEY NAMES_KEY valuelist anystringlist ';' { SleighCompile_attachNames(slgh, $3, $4); }
  ;
varattach: ATTACH_KEY VARIABLES_KEY valuelist varlist ';' { SleighCompile_attachVarnodes(slgh, $3, $4); }
  ;
macrodef: macrostart '{' rtl '}'	{  SleighCompile_buildMacro(slgh, $1, $3); }
  ;

withblockstart: WITH_KEY id_or_nil ':' bitpat_or_nil contextblock '{'  {  SleighCompile_pushWith(slgh, $2, $4, $5); }
  ;
withblockmid: withblockstart
  | withblockmid definition
  | withblockmid constructorlike
  ;
withblock: withblockmid '}'  { SleighCompile_popWith(slgh); }
  
id_or_nil: /* empty */  { $$ = (SubtableSymbol *)0; }
  | SUBTABLESYM         { $$ = $1; }
  | STRING              { $$ = SleighCompile_newTable(slgh, $1->data); }
  ;

bitpat_or_nil: /* empty */ { $$ = (PatternEquation *)0; }
  | pequation              { $$ = $1; }
  ;

macrostart: MACRO_KEY STRING '(' oplist ')' { $$ = SleighCompile_createMacro(slgh, $2->data, $4); dynarray_delete($4); }
  ;
rtlbody: '{' rtl '}' { $$ = SleighCompile_standaloneSection(slgh, $2); }
  | '{' rtlcontinue rtlmid '}' { $$ = SleighCompile_finalNamedSection(slgh, $2, $3); }
  | OP_UNIMPL        { $$ = (SectionVector *)0; }
  ;
constructor: constructprint IS_KEY pequation contextblock rtlbody { SleighCompile_buildConstructor(slgh, $1, $3, $4, $5); }
  | subtablestart IS_KEY pequation contextblock rtlbody           { SleighCompile_buildConstructor(slgh, $1, $3, $4, $5); }
  ;
constructprint: subtablestart STRING	{ $$ = $1; Constructor_addSyntax($$, $2->data); cstr_delete($2); }
  | subtablestart charstring		{ $$ = $1; Constructor_addSyntax($$, $2->data); cstr_delete($2); }
  | subtablestart SYMBOLSTRING		{ $$ = $1; if (SleighCompile_isInRoot(slgh, $1)) { 
                                        Constructor_addSyntax($$, $2->data); cstr_delete($2); } 
                                      else SleighCompile_newOperand(slgh, $1,$2->data); 
                                      }
  | subtablestart '^'				{ $$ = $1; if (!SleighCompile_isInRoot(slgh, $1)) { yyerror("Unexpected '^' at start of print pieces");  YYERROR; } }
  | constructprint '^'				{ $$ = $1; }
  | constructprint STRING			{ $$ = $1; Constructor_addSyntax($$, $2->data); cstr_delete($2); }
  | constructprint charstring		{ $$ = $1; Constructor_addSyntax($$, $2->data); cstr_delete($2); }
  | constructprint ' '				{ $$ = $1; Constructor_addSyntax($$, " "); }
  | constructprint SYMBOLSTRING		{ $$ = $1; SleighCompile_newOperand(slgh, $1, $2->data); cstr_delete($2); }
  ;
subtablestart: SUBTABLESYM ':'	{ $$ = SleighCompile_createConstructor(slgh, $1); }
  | STRING ':'					{ SubtableSymbol *sym= SleighCompile_newTable(slgh, $1->data); 
                          $$ = SleighCompile_createConstructor(slgh, sym);  cstr_delete($1); }
  | ':'							{ $$ = SleighCompile_createConstructor(slgh, (SubtableSymbol *)0); }
  | subtablestart ' '			{ $$ = $1; }
  ;
pexpression: INTB			{ $$ = ConstantValue_newB($1);  }
// familysymbol is not acceptable in an action expression because it isn't attached to an offset
  | familysymbol			{ if ((actionon==1)&&($1->type != context_symbol))
                                             { yyerror("Global symbol %s is not allowed in action expression", $1->name); } 
                                             $$ = $1->value.patval; }
//  | CONTEXTSYM                          { $$ = $1->value.patval; }
  | specificsymbol			{ $$ = SleighSymbol_getPatternExpression($1); }
  | '(' pexpression ')'			{ $$ = $2; }
  | pexpression '+' pexpression		{ $$ = PatternExpression_new(a_plusExp, $1,$3); }
  | pexpression '-' pexpression		{ $$ = PatternExpression_new(a_subExp, $1,$3); }
  | pexpression '*' pexpression		{ $$ = PatternExpression_new(a_multExp, $1,$3); }
  | pexpression OP_LEFT pexpression	{ $$ = PatternExpression_new(a_leftShiftExp, $1,$3); }
  | pexpression OP_RIGHT pexpression	{ $$ = PatternExpression_new(a_rightShiftExp, $1,$3); }
  | pexpression OP_AND pexpression	{ $$ = PatternExpression_new(a_andExp, $1,$3); }
  | pexpression OP_OR pexpression	{ $$ = PatternExpression_new(a_orExp, $1,$3); }
  | pexpression OP_XOR pexpression	{ $$ = PatternExpression_new(a_xorExp, $1,$3); }
  | pexpression '/' pexpression		{ $$ = PatternExpression_new(a_divExp, $1,$3); }
  | '-' pexpression %prec '!'		{ $$ = PatternExpression_new(a_minusExp, $2); }
  | '~' pexpression			{ $$ = PatternExpression_new(a_notExp, $2); }
  ;
pequation: elleq
  | pequation '&' pequation		{ $$ = PatternEquation_new(a_andEq, $1,$3); }
  | pequation '|' pequation		{ $$ = PatternEquation_new(a_orEq, $1,$3); }
  | pequation ';' pequation		{ $$ = PatternEquation_new(a_catEq, $1,$3); }
  ;
elleq: ELLIPSIS_KEY ellrt		{ $$ = PatternEquation_new(a_leftEllipsisEq, $2); }
  | ellrt
  ;
ellrt: atomic ELLIPSIS_KEY		{ $$ = PatternEquation_new(a_rightEllipsisEq, $1); }
  | atomic
  ;
atomic: constraint
  | '(' pequation ')'			{ $$ = $2; }
  ;
constraint: familysymbol '=' pexpression { $$ = PatternEquation_new(a_equalEq, SleighSymbol_getPatternValue($1), $3); }
  | familysymbol OP_NOTEQUAL pexpression { $$ = PatternEquation_new(a_notEqualEq, SleighSymbol_getPatternValue($1),$3); }
  | familysymbol '<' pexpression	{ $$ = PatternEquation_new(a_lessEq, SleighSymbol_getPatternValue($1),$3); }
  | familysymbol OP_LESSEQUAL pexpression { $$ = PatternEquation_new(a_lessEqualEq, SleighSymbol_getPatternValue($1),$3); }
  | familysymbol '>' pexpression	{ $$ = PatternEquation_new(a_greaterEq, SleighSymbol_getPatternValue($1),$3); }
  | familysymbol OP_GREATEQUAL pexpression { $$ = PatternEquation_new(a_greaterEqualEq, SleighSymbol_getPatternValue($1),$3); }
  | OPERANDSYM '=' pexpression		{ $$ = SleighCompile_constrainOperand(slgh, $1, $3);
                                          if ($$ == (PatternEquation *)0) 
                                            { yyerror("Constraining currently undefined operand %s", SleighSymbol_getName($1)); } }
  | OPERANDSYM				{ $$ = PatternEquation_new(a_operandEq, OperandSymbol_getIndex($1)); SleighCompile_selfDefine(slgh, $1); }
  | SPECSYM                             { $$ = PatternEquation_new(a_unconstrainedEq, SleighSymbol_getPatternExpression($1)); }
  | familysymbol                        { $$ = SleighCompile_defineInvisibleOperand(slgh, $1); }
  | SUBTABLESYM                         { $$ = SleighCompile_defineInvisibleOperand(slgh, $1); }
  ;
contextblock:				{ $$ = (struct dynarray *)0; }
  | '[' contextlist ']'			{ $$ = $2; }
  ;
contextlist: 				{ $$ = dynarray_new(NULL, NULL); }
  | contextlist CONTEXTSYM '=' pexpression ';'  { $$ = $1; if (!SleighCompile_contextMod(slgh, $1,$2,$4)) {
      yyerror("Cannot use 'inst_next' to set context variable: %s", SleighSymbol_getName($2)); YYERROR; } }
  | contextlist GLOBALSET_KEY '(' familysymbol ',' CONTEXTSYM ')' ';' { $$ = $1; SleighCompile_contextSet(slgh, $1,$4,$6); }
  | contextlist GLOBALSET_KEY '(' specificsymbol ',' CONTEXTSYM ')' ';' { $$ = $1; SleighCompile_contextSet(slgh, $1,$4,$6); }
  | contextlist OPERANDSYM '=' pexpression ';' { $$ = $1; SleighCompile_defineOperand(slgh, $2,$4); }
  | contextlist STRING                  { yyerror("Expecting context symbol, not %s", *$2); YYERROR; }
  ;
section_def: OP_LEFT STRING OP_RIGHT    { $$ = SleighCompile_newSectionSymbol( slgh, $2->data ); cstr_delete($2); }
  | OP_LEFT SECTIONSYM OP_RIGHT         { $$ = $2; }
  ;
rtlfirstsection: rtl section_def        { $$ = SleighCompile_firstNamedSection(slgh, $1,$2); }
  ;
rtlcontinue: rtlfirstsection { $$ = $1; }
  | rtlcontinue rtlmid section_def      { $$ = SleighCompile_nextNamedSection(slgh, $1,$2,$3); }
  ;
rtl: rtlmid { $$ = $1; if (!ConstructTpl_getOpvec($1)->len && (ConstructTpl_getResult($$) == (HandleTpl *)0)) SleighCompile_recordNop(slgh); }
  | rtlmid EXPORT_KEY exportvarnode ';' { $$ = SleighCompile_setResultVarnode(slgh, $1,$3); }
  | rtlmid EXPORT_KEY sizedstar lhsvarnode ';' { $$ = SleighCompile_setResultStarVarnode(slgh, $1,$3,$4); }
  | rtlmid EXPORT_KEY STRING		{ yyerror("Unknown export varnode: %s", *$3); YYERROR; }
  | rtlmid EXPORT_KEY sizedstar STRING	{ yyerror("Unknown pointer varnode: %s", *$4); vm_free($3); vm_free($4); YYERROR; }
  ;
rtlmid: /* EMPTY */			{ $$ = ConstructTpl_new(); }
  | rtlmid statement			{ $$ = $1; if (!ConstructTpl_addOpList($$, $2)) { vm_free($2); yyerror("Multiple delayslot declarations"); YYERROR; } dynarray_delete($2); }
  | rtlmid LOCAL_KEY STRING ';' { $$ = $1; PcodeCompile_newLocalDefinition(slgh->pcode, $3->data, 0); cstr_delete($3); }
  | rtlmid LOCAL_KEY STRING ':' INTEGER ';' { $$ = $1; PcodeCompile_newLocalDefinition(slgh->pcode, $3->data, (uint32_t)$5);  }
  ;
statement: lhsvarnode '=' expr ';'	{ ExpTree_setOutput($3, $1); $$ = ExpTree_toVector($3); }
  | LOCAL_KEY STRING '=' expr ';'	{ $$ = PcodeCompile_newOutput(slgh->pcode, true, $4, $2->data, 0); cstr_free($2); }
  | STRING '=' expr ';'			{ $$ = PcodeCompile_newOutput(slgh->pcode, false,$3,$1->data, 0); }
  | LOCAL_KEY STRING ':' INTEGER '=' expr ';'	{ $$ = PcodeCompile_newOutput(slgh->pcode, true,$6, $2->data, (u4)$4); cstr_delete($2); }
  | STRING ':' INTEGER '=' expr ';'	{ $$ = PcodeCompile_newOutput(slgh->pcode, true,$5,$1->data,(u4)$3); }
  | LOCAL_KEY specificsymbol '=' { $$ = (struct dynarray *)0; yyerror("Redefinition of symbol: %s", SleighSymbol_getName($2)); YYERROR; }
  | sizedstar expr '=' expr ';'		{ $$ = PcodeCompile_createStore(slgh->pcode, $1,$2,$4); StarQuality_delete($1); }
  | USEROPSYM '(' paramlist ')' ';'	{ $$ = PcodeCompile_createUserOpNoOut(slgh->pcode, $1,$3); }
  | lhsvarnode '[' INTEGER ',' INTEGER ']' '=' expr ';' { $$ = PcodeCompile_assignBitRange(slgh->pcode, $1,(uint4)$3,(uint4)$5,$8); }
  | BITSYM '=' expr ';'                 { $$= PcodeCompile_assignBitRange(slgh->pcode, 
                                          SleighSymbol_getVarnode (BitrangeSymbol_getParentSymbol($1)), 
                                          BitrangeSymbol_getBitOffset($1),
                                          BitrangeSymbol_numBits($1),$3); }
  | varnode ':' INTEGER '='		{ VarnodeTpl_delete($1); yyerror("Illegal truncation on left-hand side of assignment"); YYERROR; }
  | varnode '(' INTEGER ')'		{ VarnodeTpl_delete($1); yyerror("Illegal subpiece on left-hand side of assignment"); YYERROR; }
  | BUILD_KEY OPERANDSYM ';'		{ $$ = PcodeCompile_createOpConst(slgh->pcode, BUILD, OperandSymbol_getIndex($2)); }
  | CROSSBUILD_KEY varnode ',' SECTIONSYM ';' { $$ = SleighCompile_createCrossBuild(slgh, $2,$4); }
  | CROSSBUILD_KEY varnode ',' STRING ';'   { $$ = SleighCompile_createCrossBuild(slgh, $2, SleighCompile_newSectionSymbol(slgh, $4->data)); cstr_delete($4); }
  | DELAYSLOT_KEY '(' INTEGER ')' ';'	{ $$ = PcodeCompile_createOpConst(slgh->pcode, DELAY_SLOT,$3); }
  | GOTO_KEY jumpdest ';'		{ $$ = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_BRANCH,ExpTree_newV($2)); }
  | IF_KEY expr GOTO_KEY jumpdest ';'	{ $$ = PcodeCompile_createOpNoOut2(slgh->pcode, CPUI_CBRANCH,ExpTree_newV($4),$2); }
  | GOTO_KEY '[' expr ']' ';'		{ $$ = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_BRANCHIND,$3); }
  | CALL_KEY jumpdest ';'		{ $$ = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_CALL,ExpTree_newV($2)); }
  | CALL_KEY '[' expr ']' ';'		{ $$ = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_CALLIND,$3); }
  | RETURN_KEY ';'			{ yyerror("Must specify an indirect parameter for return"); YYERROR; }
  | RETURN_KEY '[' expr ']' ';'		{ $$ = PcodeCompile_createOpNoOut(slgh->pcode, CPUI_RETURN,$3); }
  | MACROSYM '(' paramlist ')' ';'      { $$ = SleighCompile_createMacroUse(slgh, $1,$3); }
  | label                               { $$ = PcodeCompile_placeLabel(slgh->pcode, $1); }
  ;
expr: varnode { $$ = ExpTree_newV($1); }
  | sizedstar expr %prec '!'	{ $$ = PcodeCompile_createLoad(slgh->pcode, $1,$2); StarQuality_delete($1); }
  | '(' expr ')'		{ $$ = $2; }
  | expr '+' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_ADD,$1,$3); }
  | expr '-' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SUB,$1,$3); }
  | expr OP_EQUAL expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_EQUAL,$1,$3); }
  | expr OP_NOTEQUAL expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_NOTEQUAL,$1,$3); }
  | expr '<' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LESS,$1,$3); }
  | expr OP_GREATEQUAL expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LESSEQUAL,$3,$1); }
  | expr OP_LESSEQUAL expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LESSEQUAL,$1,$3); }
  | expr '>' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LESS,$3,$1); }
  | expr OP_SLESS expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SLESS,$1,$3); }
  | expr OP_SGREATEQUAL expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SLESSEQUAL,$3,$1); }
  | expr OP_SLESSEQUAL expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SLESSEQUAL,$1,$3); }
  | expr OP_SGREAT expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SLESS,$3,$1); }
  | '-' expr	%prec '!'      	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_INT_2COMP,$2); }
  | '~' expr			{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_INT_NEGATE,$2); }
  | expr '^' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_XOR,$1,$3); }
  | expr '&' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_AND,$1,$3); }
  | expr '|' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_OR,$1,$3); }
  | expr OP_LEFT expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_LEFT,$1,$3); }
  | expr OP_RIGHT expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_RIGHT,$1,$3); }
  | expr OP_SRIGHT expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SRIGHT,$1,$3); }
  | expr '*' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_MULT,$1,$3); }
  | expr '/' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_DIV,$1,$3); }
  | expr OP_SDIV expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SDIV,$1,$3); }
  | expr '%' expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_REM,$1,$3); }
  | expr OP_SREM expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SREM,$1,$3); }
  | '!' expr			{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_BOOL_NEGATE,$2); }
  | expr OP_BOOL_XOR expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_BOOL_XOR,$1,$3); }
  | expr OP_BOOL_AND expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_BOOL_AND,$1,$3); }
  | expr OP_BOOL_OR expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_BOOL_OR,$1,$3); }
  | expr OP_FEQUAL expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_EQUAL,$1,$3); }
  | expr OP_FNOTEQUAL expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_NOTEQUAL,$1,$3); }
  | expr OP_FLESS expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_LESS,$1,$3); }
  | expr OP_FGREAT expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_LESS,$3,$1); }
  | expr OP_FLESSEQUAL expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_LESSEQUAL,$1,$3); }
  | expr OP_FGREATEQUAL expr	{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_LESSEQUAL,$3,$1); }
  | expr OP_FADD expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_ADD,$1,$3); }
  | expr OP_FSUB expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_SUB,$1,$3); }
  | expr OP_FMULT expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_MULT,$1,$3); }
  | expr OP_FDIV expr		{ $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_FLOAT_DIV,$1,$3); }
  | OP_FSUB expr %prec '!'      { $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_NEG,$2); }
  | OP_ABS '(' expr ')'		{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_ABS,$3); }
  | OP_SQRT '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_SQRT,$3); }
  | OP_SEXT '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_INT_SEXT,$3); }
  | OP_ZEXT '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_INT_ZEXT,$3); }
  | OP_CARRY '(' expr ',' expr ')' { $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_CARRY,$3,$5); }
  | OP_SCARRY '(' expr ',' expr ')' { $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SCARRY,$3,$5); }
  | OP_SBORROW '(' expr ',' expr ')' { $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_INT_SBORROW,$3,$5); }
  | OP_FLOAT2FLOAT '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_FLOAT2FLOAT,$3); }
  | OP_INT2FLOAT '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_INT2FLOAT,$3); }
  | OP_NAN '(' expr ')'		{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_NAN,$3); }
  | OP_TRUNC '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_TRUNC,$3); }
  | OP_CEIL '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_CEIL,$3); }
  | OP_FLOOR '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_FLOOR,$3); }
  | OP_ROUND '(' expr ')'	{ $$ = PcodeCompile_createOp(slgh->pcode, CPUI_FLOAT_ROUND,$3); }
  | OP_NEW '(' expr ')'     { $$ = PcodeCompile_createOp(slgh->pcode, CPUI_NEW,$3); }
  | OP_NEW '(' expr ',' expr ')' { $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_NEW,$3,$5); }
  | OP_POPCOUNT '(' expr ')' { $$ = PcodeCompile_createOp(slgh->pcode, CPUI_POPCOUNT,$3); }
  | specificsymbol '(' integervarnode ')' { $$ = PcodeCompile_createOp2(slgh->pcode, CPUI_SUBPIECE,ExpTree_newV(SleighSymbol_getVarnode($1)),ExpTree_newV($3)); }
  | specificsymbol ':' INTEGER	{ $$ = PcodeCompile_createBitRange(slgh->pcode, $1,0,(u4)($3 * 8)); }
  | specificsymbol '[' INTEGER ',' INTEGER ']' { $$ = PcodeCompile_createBitRange(slgh->pcode, $1,(uint4)$3,(uint4)$5); }
  | BITSYM                      { $$=PcodeCompile_createBitRange(slgh->pcode, 
                                  BitrangeSymbol_getParentSymbol($1),
                                  BitrangeSymbol_getBitOffset($1),
                                  BitrangeSymbol_numBits($1)); }
  | USEROPSYM '(' paramlist ')' { $$ = PcodeCompile_createUserOp(slgh->pcode, $1,$3); }
  | OP_CPOOLREF '(' paramlist ')'  { if ($3->len < 2) { yyerror("Must at least two inputs to cpool");  YYERROR; } $$ = PcodeCompile_createVariadic(slgh->pcode, CPUI_CPOOLREF,$3); }
  ;  
sizedstar: '*' '[' SPACESYM ']' ':' INTEGER { $$ = StarQuality_new(); $$->size = (u4)$6; $$->id=ConstTpl_newA(SleighSymbol_getSpace($3)); }
  | '*' '[' SPACESYM ']'	{ $$ = StarQuality_new(); $$->size = 0; $$->id=ConstTpl_newA(SleighSymbol_getSpace($3)); }
  | '*' ':' INTEGER		{ $$ = StarQuality_new(); $$->size = (u4)$3; $$->id=ConstTpl_newA(SleighCompile_getDefaultCodeSpace(slgh)); }
  | '*'				{ $$ = StarQuality_new(); $$->size = 0; $$->id=ConstTpl_newA(SleighCompile_getDefaultCodeSpace(slgh)); }
  ;
jumpdest: STARTSYM		{ VarnodeTpl *sym = SleighSymbol_getVarnode($1); 
                      $$ = VarnodeTpl_new(ConstTpl_new1(j_curspace),VarnodeTpl_getOffset(sym), ConstTpl_new1(j_curspace_size)); SleighSymbol_delete($1) }
  | ENDSYM			{ VarnodeTpl *sym = SleighSymbol_getVarnode($1); 
                    $$ = VarnodeTpl_new(ConstTpl_new1(j_curspace), VarnodeTpl_getOffset(sym),ConstTpl_new1(j_curspace_size)); SleighSymbol_delete($1); }
  | INTEGER			{ $$ = VarnodeTpl_new(ConstTpl_new1(j_curspace),ConstTpl_new2(real,$1),ConstTpl_new1(j_curspace_size));  }
  | BADINTEGER    { $$ = VarnodeTpl_new(ConstTpl_new1(j_curspace),ConstTpl_new2(real,0),ConstTpl_new1(j_curspace_size)); 
                    yyerror("Parsed integer is too big (overflow)"); }
  | OPERANDSYM			{ $$ = SleighSymbol_getVarnode($1); SleighSymbol_setCodeAddress($1); }
  | INTEGER '[' SPACESYM ']'	{ AddrSpace *spc = SleighSymbol_getSpace($3); 
                                $$ = VarnodeTpl_new(ConstTpl_newA(spc),ConstTpl_new2(real,$1),ConstTpl_new2(real,AddrSpace_getAddrSize(spc))); }
  | label                       { $$ = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),
                                    ConstTpl_new2(j_relative, LabelSymbol_getIndex($1)),ConstTpl_new2(real,sizeof(uintm))); SleighSymbol_incrementRefCount($1); }
  | STRING			{ yyerror("Unknown jump destination: %s", *$1); cstr_delete($1); YYERROR; }
  ;
varnode: specificsymbol		{ $$ = SleighSymbol_getVarnode($1); }
  | integervarnode		{ $$ = $1; }
  | STRING			{ yyerror("Unknown varnode parameter: %s", $1->data); cstr_delete($1); ; YYERROR; }
  | SUBTABLESYM                 { yyerror("Subtable not attached to operand: %s", SleighSymbol_getName($1)); ; YYERROR; }
  ;
integervarnode: INTEGER		{ $$ = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),
                              ConstTpl_new2(real,$1),ConstTpl_new2(real,0)); }
  | BADINTEGER                  { $$ = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),ConstTpl_new2(real,0),ConstTpl_new2(real,0)); 
                                  yyerror("Parsed integer is too big (overflow)"); }
  | INTEGER ':' INTEGER		{ $$ = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),
                                      ConstTpl_new2(real,$1),ConstTpl_new2(real,$3)); }
  | '&' varnode                 { $$ = PcodeCompile_addressOf(slgh->pcode, $2,0); }
  | '&' ':' INTEGER varnode     { $$ = PcodeCompile_addressOf(slgh->pcode, $4,(u4)$3); }
  ;
lhsvarnode: specificsymbol	{ $$ = SleighSymbol_getVarnode($1); }
  | STRING			{ yyerror("Unknown assignment varnode: %s", $1->data); cstr_delete($1); YYERROR; }
  | SUBTABLESYM                 { yyerror("Subtable not attached to operand: %s", SleighSymbol_getName($1)); YYERROR; }
  ;
label: '<' LABELSYM '>'         { $$ = $2; }
  | '<' STRING '>'              { $$ = PcodeCompile_defineLabel( slgh->pcode, $2->data ); }
  ;
exportvarnode: specificsymbol	{ $$ = SleighSymbol_getVarnode($1); }
  | '&' varnode                 { $$ = PcodeCompile_addressOf(slgh->pcode, $2,0); }
  | '&' ':' INTEGER varnode     { $$ = PcodeCompile_addressOf(slgh->pcode, $4,(u4)$3); }
  | INTEGER ':' INTEGER		{ $$ = VarnodeTpl_new(ConstTpl_newA(SleighCompile_getConstantSpace(slgh)),
                            ConstTpl_new2(real,$1),ConstTpl_new2(real,$3)); }
  | STRING			{ yyerror("Unknown export varnode: %s", $1->data); cstr_delete($1);  YYERROR; }
  | SUBTABLESYM                 { yyerror("Subtable not attached to operand: %s", SleighSymbol_getName($1)); YYERROR; }
  ;
familysymbol: VALUESYM		{ $$ = $1; }
  | VALUEMAPSYM                 { $$ = $1; }
  | CONTEXTSYM                  { $$ = $1; }
  | NAMESYM			{ $$ = $1; }
  | VARLISTSYM			{ $$ = $1; }
  ;
specificsymbol: VARSYM		{ $$ = $1; }
  | SPECSYM                     { $$ = $1; }
  | OPERANDSYM			{ $$ = $1; }
  | STARTSYM			{ $$ = $1; }
  | ENDSYM			{ $$ = $1; }
  ;
charstring: CH		{ $$ = cstr_new(NULL, 0); cstr_ccat($$, $1); }
  | charstring CH		{ $$ = $1; cstr_ccat($$, $2); }
  ;
intblist: '[' intbpart ']'	{ $$ = $2; }
  | INTEGER                     { $$ = dynarray_new(int64_cmp, int64_delete); dynarray_add($$, int64_new($1)); }
  | '-' INTEGER                 { $$ = dynarray_new(int64_cmp, int64_delete); dynarray_add($$, int64_new(-$2)); }
  ;
intbpart: INTEGER		{ $$ = dynarray_new(int64_cmp, int64_delete); dynarray_add($$, int64_new($1)); }
  | '-' INTEGER                 { $$ = dynarray_new(int64_cmp, int64_delete); dynarray_add($$, int64_new(-$2)); }
  | STRING                      { if (strcmp ($1->data, "_")) { yyerror("Expecting integer but saw: %s", $1->data); cstr_delete($1); YYERROR; }
                                  $$ = dynarray_new(int64_cmp, int64_delete); dynarray_add($$, int64_new((intb)0xBADBEEF)); cstr_delete($1); }
  | intbpart INTEGER            { $$ = $1; dynarray_add($1, int64_new($2)); }
  | intbpart '-' INTEGER        { $$ = $1; dynarray_add($1, int64_new(-$3)); }
  | intbpart STRING             { if (strcmp($2->data, "_")) { yyerror("Expecting integer but saw: %s", $2->data); cstr_delete($2);  YYERROR; }
                                  $$ = $1; dynarray_add($1, int64_new((intb)0xBADBEEF)); cstr_delete($2); }
  ;
stringlist: '[' stringpart ']'	{ $$ = $2; }
  | STRING			{ $$ = dynarray_new(NULL, cstr_delete); dynarray_add($$, $1); }
  ;
stringpart: STRING		{ $$ = dynarray_new(NULL, cstr_delete); dynarray_add($$, $1 ); }
  | stringpart STRING		{ $$ = $1; dynarray_add($$, $2);  }
  | stringpart anysymbol	{ yyerror( "%s redefined", SleighSymbol_getName($2)); YYERROR; }
  ;
anystringlist: '[' anystringpart ']' { $$ = $2; }
  ;
anystringpart: STRING           { $$ = dynarray_new(NULL, str_free); dynarray_add($$, $1); }
  | anysymbol                   { $$ = dynarray_new(NULL, NULL); dynarray_add($$, SleighSymbol_getName($1)); }
  | anystringpart STRING        { $$ = $1; dynarray_add($$, $2); }
  | anystringpart anysymbol     { $$ = $1; dynarray_add($$, SleighSymbol_getName($2)); }
  ;
valuelist: '[' valuepart ']'	{ $$ = $2; }
  | VALUESYM			{ $$ = dynarray_new(NULL, NULL); dynarray_add($$, $1); }
  | CONTEXTSYM                  { $$ = dynarray_new(NULL, NULL); dynarray_add($$, $1); }
  ;
valuepart: VALUESYM		{ $$ = dynarray_new(NULL, NULL); dynarray_add( $$, $1 ); }
  | CONTEXTSYM                  { $$ = dynarray_new(NULL, NULL); dynarray_add($$, $1); }
  | valuepart VALUESYM		{ $$ = $1; dynarray_add($$, $2); }
  | valuepart CONTEXTSYM        { $$ = $1; dynarray_add($$, $2); }
  | valuepart STRING		{ yyerror("%s: is not a value pattern", *$2); vm_free($2); YYERROR; }
  ;
varlist: '[' varpart ']'	{ $$ = $2; }
  | VARSYM			{ $$ = dynarray_new(NULL, NULL); dynarray_add($$, $1); }
  ;
varpart: VARSYM			{ $$ = dynarray_new(NULL, NULL); dynarray_add($$, $1); }
  | STRING                      { if (strcmp($1->data, "_")) { yyerror("%s: is not a varnode symbol", $1->data); cstr_delete($1); YYERROR; }
				  $$ = dynarray_new(NULL, NULL); dynarray_add($$, (SleighSymbol *)0); vm_free($1); }
  | varpart VARSYM		{ $$ = $1; dynarray_add($$, $1); }
  | varpart STRING		{ if (strcmp($2->data, "_")) { yyerror("%s: is not a varnode symbol", $2->data); cstr_delete($2); YYERROR; }
                                  $$ = $1; dynarray_add($$, (SleighSymbol *)0); cstr_delete($2); }
  ;
paramlist: /* EMPTY */		{ $$ = dynarray_new(NULL, NULL); }
  | expr			{ $$ = dynarray_new(NULL, NULL); dynarray_add($$, $1); }
  | paramlist ',' expr		{ $$ = $1; dynarray_add($$, $3); }
  ;
oplist: /* EMPTY */		{ $$ = dynarray_new(NULL, NULL); }
  | STRING			{ $$ = dynarray_new(NULL, NULL); dynarray_add($$, $1); }
  | oplist ',' STRING		{ $$ = $1; dynarray_add($$, $3);  }
  ;
anysymbol: SPACESYM		{ $$ = $1; }
  | SECTIONSYM                  { $$ = $1; }
  | TOKENSYM			{ $$ = $1; }
  | USEROPSYM			{ $$ = $1; }
  | MACROSYM			{ $$ = $1; }
  | SUBTABLESYM			{ $$ = $1; }
  | VALUESYM			{ $$ = $1; }
  | VALUEMAPSYM                 { $$ = $1; }
  | CONTEXTSYM                  { $$ = $1; }
  | NAMESYM			{ $$ = $1; }
  | VARSYM			{ $$ = $1; }
  | VARLISTSYM			{ $$ = $1; }
  | OPERANDSYM			{ $$ = $1; }
  | STARTSYM			{ $$ = $1; }
  | ENDSYM			{ $$ = $1; }
  | BITSYM                      { $$ = $1; }
  ;
%%

int yyerror(const char *s, ...)

{
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "Error(%s:%d):", SleighCompile_grabCurrentFilePath(slgh), SleighCompile_curLineNo(slgh));
  vfprintf(stderr, s, ap);
  va_end(ap);
  exit(1);

  return 0;
}
