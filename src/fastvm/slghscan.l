/* ###
 * IP: GHIDRA
 * NOTE: flex skeletons are NOT bound by flex's BSD license
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
%option never-interactive
%{
#include "slgh_compile.h"
#include "slghparse.tab.h"
#define yywrap() 1
#define YY_SKIP_YYWRAP

struct FileStreamState {
  YY_BUFFER_STATE lastbuffer;	// Last lex buffer corresponding to the stream
  FILE *file;                   // The NEW file stream
};

extern SleighCompile *slgh;
int4 last_preproc;   // lex state before last preprocessing erasure
int4 actionon;       // whether '&' '|' and '^' are treated as actionon in pattern section
int4 withsection = 0; // whether we are between the 'with' keyword and its open brace '{'
struct dynarray filebuffers;
struct dynarray ifstack;
int4 negative_if = -1;

#define fss_back()       ((struct FileStreamState *)dynarray_back(&filebuffers))
#define skip_space(s)       while (isspace(*s)) s++;
#define ifstack_push(a)     dynarray_add(&ifstack, (void *)a)
#define ifstack_back()        ((int)dynarray_back(&ifstack))
#define ifstack_back_set(i)      (dynarray_back(&ifstack)) = (void *)i
#define ifstack_pop()      ifstack.len --

#define fss_new()         calloc(1, sizeof (struct FileStreamState))

void check_to_endofline(const char *s)

{ // Make sure there is nothing to the end of the line
  skip_space(s);
  if (s[0])
    if (s[0] != '#')
      vm_error("Extra characters in preprocessor directive"); 
}

CString *read_identifier(char **old, CString *res)

{  // Read a proper identifier from the stream
  char *s = *old;
  skip_space(s);

  char *from = s;
  while (*s && (isalnum(*s) || *s == '_')) s++;

  cstr_cat(res, from, s - from);

  *old = s;
  return res;
}

void preprocess_string(char **old, CString *res)

{  // Grab string surrounded by double quotes from stream or call preprocess_error
  char *s = *old;
  char *t;
  
  skip_space(s);
  if (s[0] != '\"')
    vm_error("Expecting double quoted string, %s", s);
  s++;
  t = strchr(s, '\"');
  if (!t)
      vm_error("Missing terminating double quote");

  cstr_cat(res, s, t - s);

  *old = ++t;
}

extern int4 preprocess_if(char **old);

int4 read_defined_operator(char **old)

{  // We have seen a -defined- keyword in an if or elif
   // Read macro name used as input, return 1 if it is defined
   char *s = *old;
  char tok = ' ';
  CSTR_INIT(macroname);
  
  skip_space(s);
  if (s[0] != '(')
    vm_error("Badly formed \"defined\" operator");
  ++s;
  read_identifier(&s, &macroname);
  int4 res = slgh_get_macro(slgh, macroname.data, NULL) ? 1 : 0;
  skip_space(s);
  if (s[0] != ')')
    vm_error("Badly formed \"defined\" operator");
  *old = ++s;
  return res;
}

int4 read_boolean_clause(char **old)

{				// We have seen an if or elif
				// return 1 if condition is true or else 0
  int ret = 0;
  char *s = *old;
  skip_space(s);
  if (*s =='(') {		// Parenthetical expression spawns recursion
    int4 val = *++s;
    int4 res = preprocess_if(&s);
    skip_space(s);
    val = *++s;
    if (val != ')')
      vm_error("Unbalanced parentheses");

    *old = s;
    return res;
  }
				// Otherwise we must have a normal comparison operator
  CSTR_INIT(lhs);
  CSTR_INIT(rhs);
  char *comp, *lhs_val = "\0", *rhs_val = "\0";

  if (s[0] =='\"')		// Read left-hand side string
    preprocess_string(&s,&lhs);
  else {
    read_identifier(&s, &lhs);
    if (!strcmp(lhs.data, "defined")) {
      ret = read_defined_operator(&s);
      goto exit_label;
    }
    if (!slgh_get_macro(slgh, lhs.data, &lhs_val))
      vm_error("Could not find preprocessor macro %s", lhs.data);
  }

  skip_space(s);

  comp = s;
  s += 2;

  skip_space(s);
  if (*s =='\"') {// Read right-hand side string
    preprocess_string(&s,&rhs);
  }
  else {
    read_identifier(&s, &rhs);
    if (!slgh_get_macro(slgh, rhs.data, &rhs_val))
      vm_error("Could not find preprocessor macro %s", rhs.data);
  }

  if (!strncmp(comp, "==", 2)) {
    ret = !strcmp (lhs_val, rhs_val) ? 1 : 0;
  }
  else if (!strncmp(comp, "!=", 2))
    ret = strcmp(lhs_val, rhs_val) ? 1 : 0;
  else
    vm_error("Syntax error in condition");

exit_label:
  cstr_free(&lhs);
  cstr_free(&rhs);

  *old = s;
  return ret;
}

int4 preprocess_if(char **old)

{
  char *s = *old;
  int4 res = read_boolean_clause(&s);
  skip_space(s);
  while(s[0] && (s[0] != ')')) {
    char *boolop = s;

    s += 2;
    int4 res2 = read_boolean_clause(&s);
    if (!strncmp(boolop, "&&", 2))
      res = res & res2;
    else if (!strncmp(boolop, "||", 2))
      res = res | res2;
    else if (!strncmp(boolop, "^^", 2))
      res = res ^ res2;
    else
      vm_error("Syntax error in expression");

    skip_space(s);
  }

  *old = s;
  return res;
}

void expand_preprocmacros(CString *cstr)

{
  CSTR_INIT(macro);
  CSTR_INIT(res);
  char *lastpos = NULL, *value;
  char *pos = strstr(cstr->data, "$(");
  char *end = (char *)cstr->data + cstr->size;
  if (!pos) return;

  for(;;) {
    if (NULL == pos) {
      cstr_cat(&res, lastpos, end - lastpos);
      cstr_free(cstr);
      cstr_cat(cstr, res.data, res.size);

      cstr_free(&res);
      return;
    }

    cstr_cat(&res, lastpos, (pos - lastpos));
    lastpos = strchr(pos, ')');
    if (!lastpos) {
      vm_error("Unterminated macro in string, %s", cstr->data);
    }

    macro.data = pos + 2;
    macro.size = lastpos - (char *)macro.data;
    if (!slgh_get_macro(slgh, macro.data, &value)) {
      vm_error("Unknown preprocessing macro %s", macro.data);
    }
    cstr_cat(&res, value, strlen(value));

    pos = strstr(++lastpos, "$(");
  }
}


int4 preprocess(int4 cur_state,int4 blank_state)

{
  CSTR_INIT(varname);
  CSTR_INIT(value);
  char *str = str_new(yytext, 0);
  char *comment = strchr (str, '#');
  char *filename;
  if (comment) {
    *comment = 0;
  }
  char *s = str;
  CSTR_INIT(type);

  if (cur_state != blank_state)
    last_preproc = cur_state;

// skip @
  s++; 
  read_identifier(&s, &type);
  if (!strcmp(type.data, "include")) {
    if (negative_if == -1) {  // Not in the middle of a false if clause
      dynarray_add(&filebuffers, fss_new());
      fss_back()->lastbuffer = YY_CURRENT_BUFFER;
      fss_back()->file = 0;
      skip_space(s);
      CSTR_INIT(fname);
      preprocess_string(&s,&fname);
      expand_preprocmacros(&fname);
      SleighCompile_parseFromNewFile(slgh, fname.data);
      cstr_free(&fname);
      filename = SleighCompile_grabCurrentFilePath(slgh);
      yyin = fopen(filename,"r");
      if (yyin == (FILE *)0) {
        vm_error("Could not open included file %s", filename);
      }
      fss_back()->file = yyin;
      yy_switch_to_buffer( yy_create_buffer(yyin, YY_BUF_SIZE) );
      check_to_endofline(s);
    }
  }
  else if (!strcmp(type.data, "define")) {
    if (negative_if == -1) {
      read_identifier(&s, &varname);   // Get name of variable being defined
      skip_space(s);
      if (s[0] == '\"')
        preprocess_string(&s,&value);
      else
        read_identifier(&s, &value);
      if (varname.size ==0)
        vm_error("Error in preprocessor definition");
      slgh_set_macro(slgh, varname.data, value.data);
      check_to_endofline(s);
    }
  }
  else if (!strcmp (type.data, "undef")) {
    if (negative_if == -1) {
      read_identifier(&s, &varname);		// Name of variable to undefine
      if (varname.size ==0)
        vm_error("Error in preprocessor undef");
      slgh_del_macro(slgh, varname.data);
      check_to_endofline(s);
    }
  }
  else if (!strcmp(type.data, "ifdef")) {
    read_identifier(&s, &varname);
    if (varname.size ==0)
      vm_error("Error in preprocessor ifdef");
    int4 truth = (slgh_get_macro(slgh, varname.data, NULL)) ? 1 : 0;

    ifstack_push(truth);
    check_to_endofline(s);
  }
  else if (!strcmp(type.data, "ifndef")) {
    read_identifier(&s, &varname);
    if (varname.size ==0)
      vm_error("Error in preprocessor ifndef");
    int4 truth = (slgh_get_macro(slgh, varname.data, NULL)) ? 0 : 1;	// flipped from ifdef

    ifstack_push(truth);
    check_to_endofline(s);
  }
  else if (!strcmp(type.data, "if")) {
    int4 truth = preprocess_if(&s);
    if (s[0])
      vm_error("Unbalanced parentheses");
    ifstack_push(truth);
  }
  else if (!strcmp(type.data, "elif")) {
    if (ifstack.len == 0)
      vm_error("elif without preceding if");
    if ((ifstack_back()&2)!=0)		// We have already seen an else clause
      vm_error("elif follows else");
    if ((ifstack_back()&4)!=0)          // We have already seen a true elif clause
      ifstack_back_set(4);               // don't include any other elif clause
    else if ((ifstack_back()&1)!=0)     // Last clause was a true if
      ifstack_back_set(4);               // don't include this elif
    else {
      int4 truth = preprocess_if(&s);
      if (!s[0])
        vm_error("Unbalanced parentheses");
      if (truth==0)
        ifstack_back_set(0);
      else
        ifstack_back_set(5);
    }
  }
  else if (!strcmp(type.data, "endif")) {
    if (!ifstack.len)
      vm_error("preprocessing endif without matching if");
    ifstack_pop();
    check_to_endofline(s);
  }
  else if (!strcmp(type.data, "else")) {
    if (!ifstack.len)
      vm_error("preprocessing else without matching if");
    if ((ifstack_back()&2)!=0)
      vm_error("second else for one if");
    if ((ifstack_back()&4)!=0)       // Seen a true elif clause before
      ifstack_back_set(6);
    else if (ifstack_back()==0)
      ifstack_back_set(3);
    else
      ifstack_back_set(2);
    check_to_endofline(s);
  }
  else
    vm_error("Unknown preprocessing directive: %s", type);

  if (negative_if >= 0) {  // We were in a false state
    if (negative_if+1 < ifstack.len)
      return blank_state;  // false state is still deep in stack
    else                   // false state is popped off or is current and changed
      negative_if = -1;
  }
  if (!ifstack.len) return last_preproc;
  if ((ifstack_back()&1)==0) {
    negative_if = ifstack.len -1;
    return blank_state;
  }

  cstr_free(&varname);
  cstr_free(&value);
  cstr_free(&type);
  str_free(str);

  return last_preproc;
}

void preproc_macroexpand(void)

{
  dynarray_add(&filebuffers, vm_mallocz(sizeof (struct FileStreamState)));
  fss_back()->lastbuffer = YY_CURRENT_BUFFER;
  fss_back()->file = (FILE *)0;
  CSTR_INIT(macro);
  char *value = NULL;

  cstr_cat(&macro, yytext + 2, strlen(yytext) - 3);

  if (!slgh_get_macro(slgh, macro.data, &value))
    vm_error("Unknown preprocessing macro %s", macro.data);
  yy_switch_to_buffer( yy_scan_string( value ) );
  SleighCompile_parsePreprocMacro(slgh);
  cstr_free(&macro);
}

int4 find_symbol(void) {
  CString *newstring = cstr_new(yytext, strlen(yytext));
  SleighSymbol *sym = SleighCompile_findSymbol(slgh, newstring->data);
  if (sym == (SleighSymbol *)0) {
    yylval.str = newstring;
    return STRING;
  }
  cstr_delete (newstring);
  switch(sym->type) {
  case section_symbol:
    yylval.sectionsym = (SectionSymbol *)sym;
    return SECTIONSYM;
  case space_symbol:
    yylval.spacesym = (SpaceSymbol *)sym;
    return SPACESYM;
  case token_symbol:
    yylval.tokensym = (TokenSymbol *)sym;
    return TOKENSYM;
  case userop_symbol:
    yylval.useropsym = (UserOpSymbol *)sym;
    return USEROPSYM;
  case value_symbol:
    yylval.valuesym = (ValueSymbol *)sym;
    return VALUESYM;
  case valuemap_symbol:
    yylval.valuemapsym = (ValueMapSymbol *)sym;
    return VALUEMAPSYM;
  case name_symbol:
    yylval.namesym = (NameSymbol *)sym;
    return NAMESYM;
  case varnode_symbol:
    yylval.varsym = (VarnodeSymbol *)sym;
    return VARSYM;
  case bitrange_symbol:
    yylval.bitsym = (BitrangeSymbol *)sym;
    return BITSYM;
  case varnodelist_symbol:
    yylval.varlistsym = (VarnodeListSymbol *)sym;
    return VARLISTSYM;
  case operand_symbol:
    yylval.operandsym = (OperandSymbol *)sym;
    return OPERANDSYM;
  case start_symbol:
    yylval.startsym = (StartSymbol *)sym;
    return STARTSYM;
  case end_symbol:
    yylval.endsym = (EndSymbol *)sym;
    return ENDSYM;
  case subtable_symbol:
    yylval.subtablesym = (SubtableSymbol *)sym;
    return SUBTABLESYM;
  case macro_symbol:
    yylval.macrosym = (MacroSymbol *)sym;
    return MACROSYM;
  case label_symbol:
    yylval.labelsym = (LabelSymbol *)sym;
    return LABELSYM;
  case epsilon_symbol:
    yylval.specsym = (SpecificSymbol *)sym;
    return SPECSYM;
  case context_symbol:
    yylval.contextsym = (ContextSymbol *)sym;
    return CONTEXTSYM;
  case dummy_symbol:
    break;
  }
  //return -1;   // Should never reach here
  vm_error("should never reach here, sym->name = %s, sym->type = %d", sym->name, sym->type);
  return -1;
}

int4 scan_number(char *numtext,YYSTYPE *lval,bool signednum)

{
  uintb val;
  if (numtext[0] == '0' && numtext[1] == 'b') {
    val = 0;
    numtext += 2;
    while ((*numtext) != 0) {
      val <<= 1;
      if (*numtext == '1') {
        val |= 1;
      }
      ++numtext;
    }
  } 
  else if (numtext[0] == '0' && (numtext[1] == 'x' || numtext[1] == 'X')) {
    val = strtoull(numtext, NULL, 16);
  }
  else if (numtext[0] == '0'){
    val = strtoull(numtext, NULL, 8);
  }
  else if (numtext[0] >= '1' && numtext[0] <= '9') {
    val = strtoull(numtext, NULL, 10);
  }
  else {
      vm_error("bad integer %s", numtext);
  }
  if (signednum) {
    lval->big = val;
    return INTB;
  }
  lval->i = val;
  return INTEGER;
}

%}

%x defblock
%x macroblock
%x print
%x pattern
%x sem
%x preproc
%%

^@[^\n]*\n?  { SleighCompile_nextLine(slgh); BEGIN( preprocess(INITIAL,preproc) ); }
\$\([a-zA-Z0-9_.][a-zA-Z0-9_.]*\)  { preproc_macroexpand(); }
[(),\-] { yylval.ch = yytext[0]; return yytext[0]; }
\:    { BEGIN(print); SleighCompile_calcContextLayout(slgh); yylval.ch = yytext[0]; return yytext[0]; }
\{    { BEGIN(sem); yylval.ch = yytext[0]; return yytext[0]; }
#.*$
[\r\ \t\v]+
\n             { SleighCompile_nextLine(slgh); }
macro  { BEGIN(macroblock); return MACRO_KEY; }
define { BEGIN(defblock); return DEFINE_KEY; }
attach { BEGIN(defblock); SleighCompile_calcContextLayout(slgh); return ATTACH_KEY; }
with   { BEGIN(pattern); withsection = 1; SleighCompile_calcContextLayout(slgh); return WITH_KEY; }
[a-zA-Z_.][a-zA-Z0-9_.]* {  return find_symbol();  }
.      { return yytext[0]; }

<macroblock>^@[^\n]*\n?  { SleighCompile_nextLine(slgh); BEGIN( preprocess(macroblock,preproc) ); }
<macroblock>\$\([a-zA-Z0-9_.][a-zA-Z0-9_.]*\)  { preproc_macroexpand(); }
<macroblock>[(),]  { yylval.ch = yytext[0]; return yytext[0]; }
<macroblock>\{     { BEGIN(sem); return yytext[0]; }
<macroblock>[a-zA-Z_.][a-zA-Z0-9_.]*   {  yylval.str = cstr_new(yytext, strlen(yytext)); return STRING;  }
<macroblock>[\r\ \t\v]+
<macroblock>\n     { SleighCompile_nextLine(slgh); }
<macroblock>.      { return yytext[0]; }

<defblock>^@[^\n]*\n?  { SleighCompile_nextLine(slgh); BEGIN( preprocess(defblock,preproc) ); }
<defblock>\$\([a-zA-Z0-9_.][a-zA-Z0-9_.]*\)  { preproc_macroexpand(); }
<defblock>[(),=:\[\]]  { yylval.ch = yytext[0]; return yytext[0]; }
<defblock>\;   { BEGIN(INITIAL); yylval.ch = yytext[0]; return yytext[0]; }
<defblock>space     { return SPACE_KEY; }
<defblock>type      { return TYPE_KEY; }
<defblock>ram_space { return RAM_KEY; }
<defblock>default   { return DEFAULT_KEY; }
<defblock>register_space  { return REGISTER_KEY; }
<defblock>token     { return TOKEN_KEY; }
<defblock>context   { return CONTEXT_KEY; }
<defblock>bitrange  { return BITRANGE_KEY; }
<defblock>signed    { return SIGNED_KEY; }
<defblock>noflow    { return NOFLOW_KEY; }
<defblock>hex       { return HEX_KEY; }
<defblock>dec       { return DEC_KEY; }
<defblock>endian    { return ENDIAN_KEY; }
<defblock>alignment { return ALIGN_KEY; }
<defblock>big       { return BIG_KEY; }
<defblock>little    { return LITTLE_KEY; }
<defblock>size      { return SIZE_KEY; }
<defblock>wordsize  { return WORDSIZE_KEY; }
<defblock>offset    { return OFFSET_KEY; }
<defblock>names     { return NAMES_KEY; }
<defblock>values    { return VALUES_KEY; }
<defblock>variables { return VARIABLES_KEY; }
<defblock>pcodeop   { return PCODEOP_KEY; }
<defblock>#.*$
<defblock>[a-zA-Z_.][a-zA-Z0-9_.]* {  return find_symbol();  }
<defblock>[0-9]|[1-9][0-9]+	{ return scan_number(yytext,&yylval,false); }
<defblock>0x[0-9a-fA-F]+	{ return scan_number(yytext,&yylval,false); }
<defblock>0b[01]+		{ return scan_number(yytext,&yylval,false); }
<defblock>\"([^\"]|\"\")*\"	{ yylval.str = cstr_new(yytext+1,strlen(yytext)-2); return STRING; }
<defblock>[\r\ \t\v]+
<defblock>\n        { SleighCompile_nextLine(slgh); }
<defblock>.         { return yytext[0]; }


<print>^@[^\n]*\n?  { SleighCompile_nextLine(slgh); BEGIN( preprocess(print,preproc) ); }
<print>\$\([a-zA-Z0-9_.][a-zA-Z0-9_.]*\)  { preproc_macroexpand(); }
<print>[~!@#$%&*()\-=+\[\]{}|;:<>?,/0-9] { yylval.ch = yytext[0]; return CH; }
<print>\^           { yylval.ch = '^'; return '^'; }
<print>is           { BEGIN(pattern); actionon=0; return IS_KEY; }
<print>[a-zA-Z_.][a-zA-Z0-9_.]*   {  yylval.str = cstr_new(yytext, strlen(yytext)); return SYMBOLSTRING;  }
<print>\"([^\"]|\"\")*\"       { yylval.str = cstr_new(yytext+1,strlen(yytext)-2); return STRING; }
<print>[\r\ \t\v]+  { yylval.ch = ' '; return ' '; }
<print>\n           { SleighCompile_nextLine(slgh); return ' '; }
<print>.            { return yytext[0]; }

<pattern>^@[^\n]*\n?  { SleighCompile_nextLine(slgh); BEGIN( preprocess(pattern,preproc) ); }
<pattern>\$\([a-zA-Z0-9_.][a-zA-Z0-9_.]*\)  { preproc_macroexpand(); }
<pattern>\{         { BEGIN((withsection==1) ? INITIAL:sem); withsection=0; yylval.ch = yytext[0]; return yytext[0]; }
<pattern>unimpl     { BEGIN(INITIAL); return OP_UNIMPL; }
<pattern>globalset  { return GLOBALSET_KEY; }
<pattern>\>\>       { return OP_RIGHT; }
<pattern>\<\<       { return OP_LEFT; }
<pattern>\!\=       { return OP_NOTEQUAL; }
<pattern>\<\=       { return OP_LESSEQUAL; }
<pattern>\>\=       { return OP_GREATEQUAL; }
<pattern>\$and      { return OP_AND; }
<pattern>\$or       { return OP_OR; }
<pattern>\$xor      { return OP_XOR; }
<pattern>\.\.\.     { return ELLIPSIS_KEY; }
<pattern>\[         { actionon = 1; yylval.ch = yytext[0]; return yytext[0]; }
<pattern>\]         { actionon = 0; yylval.ch = yytext[0]; return yytext[0]; }
<pattern>\&         { yylval.ch = yytext[0];  return (actionon==0) ? yytext[0] : OP_AND; }
<pattern>\|         { yylval.ch = yytext[0];  return (actionon==0) ? yytext[0] : OP_OR; }
<pattern>\^         { return OP_XOR; }
<pattern>[=(),:;+\-*/~<>]   { yylval.ch = yytext[0]; return yytext[0]; }
<pattern>#.*$
<pattern>[a-zA-Z_.][a-zA-Z0-9_.]*   { return find_symbol();   }
<pattern>[0-9]|[1-9][0-9]+ { return scan_number(yytext,&yylval,true); }
<pattern>0x[0-9a-fA-F]+  { return scan_number(yytext,&yylval,true); }
<pattern>0b[01]+         { return scan_number(yytext,&yylval,true); }
<pattern>[\r\ \t\v]+
<pattern>\n        { SleighCompile_nextLine(slgh); }
<pattern>.         { return yytext[0]; }

<sem>^@[^\n]*\n?   { SleighCompile_nextLine(slgh); BEGIN( preprocess(sem,preproc) ); }
<sem>\$\([a-zA-Z0-9_.][a-zA-Z0-9_.]*\)  { preproc_macroexpand(); }
<sem>\}            { BEGIN(INITIAL); yylval.ch = yytext[0]; return yytext[0]; }
<sem>\|\|          { return OP_BOOL_OR; }
<sem>\&\&          { return OP_BOOL_AND; }
<sem>\^\^          { return OP_BOOL_XOR; }
<sem>\>\>          { return OP_RIGHT; }
<sem>\<\<          { return OP_LEFT; }
<sem>\=\=          { return OP_EQUAL; }
<sem>\!\=          { return OP_NOTEQUAL; }
<sem>\<\=          { return OP_LESSEQUAL; }
<sem>\>\=          { return OP_GREATEQUAL; }
<sem>s\/           { return OP_SDIV; }
<sem>s\%           { return OP_SREM; }
<sem>s\>\>         { return OP_SRIGHT; }
<sem>s\<           { return OP_SLESS; }
<sem>s\>           { return OP_SGREAT; }
<sem>s\<\=         { return OP_SLESSEQUAL; }
<sem>s\>\=         { return OP_SGREATEQUAL; }
<sem>f\+           { return OP_FADD; }
<sem>f\-           { return OP_FSUB; }
<sem>f\*           { return OP_FMULT; }
<sem>f\/           { return OP_FDIV; }
<sem>f\=\=         { return OP_FEQUAL; }
<sem>f\!\=         { return OP_FNOTEQUAL; }
<sem>f\<           { return OP_FLESS; }
<sem>f\>           { return OP_FGREAT; }
<sem>f\<\=         { return OP_FLESSEQUAL; }
<sem>f\>\=         { return OP_FGREATEQUAL; }
<sem>zext          { return OP_ZEXT; }
<sem>carry         { return OP_CARRY; }
<sem>borrow        { return OP_BORROW; }
<sem>sext          { return OP_SEXT; }
<sem>scarry        { return OP_SCARRY; }
<sem>sborrow       { return OP_SBORROW; }
<sem>nan           { return OP_NAN; }
<sem>abs           { return OP_ABS; }
<sem>sqrt          { return OP_SQRT; }
<sem>ceil          { return OP_CEIL; }
<sem>floor         { return OP_FLOOR; }
<sem>round         { return OP_ROUND; }
<sem>int2float     { return OP_INT2FLOAT; }
<sem>float2float   { return OP_FLOAT2FLOAT; }
<sem>trunc         { return OP_TRUNC; }
<sem>cpool         { return OP_CPOOLREF; }
<sem>newobject     { return OP_NEW; }
<sem>popcount      { return OP_POPCOUNT; }
<sem>if            { return IF_KEY; }
<sem>goto          { return GOTO_KEY; }
<sem>call          { return CALL_KEY; }
<sem>return        { return RETURN_KEY; }
<sem>delayslot     { return DELAYSLOT_KEY; }
<sem>crossbuild    { return CROSSBUILD_KEY; }
<sem>export        { return EXPORT_KEY; }
<sem>build         { return BUILD_KEY; }
<sem>local         { return LOCAL_KEY; }
<sem>[=(),:\[\];!&|^+\-*/%~<>]   { yylval.ch = yytext[0]; return yytext[0]; }
<sem>#.*$
<sem>[a-zA-Z_.][a-zA-Z0-9_.]*   { return find_symbol();   }
<sem>[0-9]|[1-9][0-9]+ { return scan_number(yytext,&yylval,false); }
<sem>0x[0-9a-fA-F]+  { return scan_number(yytext,&yylval,false); }
<sem>0b[01]+         { return scan_number(yytext,&yylval,false); }
<sem>[\r\ \t\v]+
<sem>\n         { SleighCompile_nextLine(slgh); }
<sem>.          { return yytext[0]; }

<preproc>^@.*\n?  { SleighCompile_nextLine(slgh); BEGIN( preprocess(preproc,preproc) ); }
<preproc>^.*\n    { SleighCompile_nextLine(slgh); }

<<EOF>> { yy_delete_buffer( YY_CURRENT_BUFFER );
          if (!filebuffers.len)
            yyterminate(); 
          yy_switch_to_buffer( fss_back()->lastbuffer );
	  FILE *curfile = fss_back()->file;
	  if (curfile != (FILE *)0)
	    fclose(curfile);
          vm_free(fss_back());
          filebuffers.len--;
          SleighCompile_parseFileFinished(slgh);
        }
