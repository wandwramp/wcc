%{
#define INTTMP 0x00003f00
#define INTVAR 0x000000fc

#define FLTTMP 0x000f0ff0
#define FLTVAR 0xfff00000

#define INTRET 0x00000002
#define FLTRET 0x00000003

#define readsreg(p) \
        (generic((p)->op)==INDIR && (p)->kids[0]->op==VREG+P)
#define setsrc(d) ((d) && (d)->x.regnode && \
        (d)->x.regnode->set == src->x.regnode->set && \
        (d)->x.regnode->mask&src->x.regnode->mask)

#define relink(a, b) ((b)->x.prev = (a), (a)->x.next = (b))

#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)
static void address(Symbol, Symbol, long);
static void blkfetch(int, int, int, int);
static void blkloop(int, int, int, int, int, int[]);
static void blkstore(int, int, int, int);
static void defaddress(Symbol);
static void defconst(int, int, Value);
static void defstring(int, char *);
static void defsymbol(Symbol);
static void doarg(Node);
static void emit2(Node);
static void export(Symbol);
static void clobber(Node);
static void function(Symbol, Symbol [], Symbol [], int);
static void global(Symbol);
static void import(Symbol);
static void local(Symbol);
static void progbeg(int, char **);
static void progend(void);
static void segment(int);
static void space(int);
static void target(Node);
static int      bitcount       (unsigned);

static Symbol ireg[32], freg2[32];
static Symbol iregw, freg2w;
static int tmpregs[] = {8, 10, 11};
static Symbol blkreg;

static int cseg;
%}
%start stmt
%term CNSTF1=1041
%term CNSTI1=1045
%term CNSTU1=1046
%term CNSTP1=1047

%term ARGB=41
%term ARGF1=1057
%term ARGI1=1061
%term ARGP1=1063
%term ARGU1=1062

%term ASGNB=57
%term ASGNF1=1073
%term ASGNI1=1077
%term ASGNP1=1079
%term ASGNU1=1078

%term INDIRB=73
%term INDIRF1=1089
%term INDIRI1=1093
%term INDIRP1=1095
%term INDIRU1=1094

%term CVFF1=1137
%term CVFI1=1141

%term CVIF1=1153
%term CVII1=1157
%term CVIU1=1158

%term CVPU1=1174

%term CVUI1=1205
%term CVUP1=1207
%term CVUU1=1206

%term NEGF1=1217
%term NEGI1=1221

%term CALLB=217
%term CALLF1=1233
%term CALLI1=1237
%term CALLP1=1239
%term CALLU1=1238
%term CALLV=216

%term RETF1=1265
%term RETI1=1269
%term RETP1=1271
%term RETU1=1270
%term RETV=248

%term ADDRGP1=1287

%term ADDRFP1=1303

%term ADDRLP1=1319

%term ADDF1=1329
%term ADDI1=1333
%term ADDP1=1335
%term ADDU1=1334

%term SUBF1=1345
%term SUBI1=1349
%term SUBP1=1351
%term SUBU1=1350

%term LSHI1=1365
%term LSHU1=1366

%term MODI1=1381
%term MODU1=1382

%term RSHI1=1397
%term RSHU1=1398

%term BANDI1=1413
%term BANDU1=1414

%term BCOMI1=1429
%term BCOMU1=1430

%term BORI1=1445
%term BORU1=1446

%term BXORI1=1461
%term BXORU1=1462

%term DIVF1=1473
%term DIVI1=1477
%term DIVU1=1478

%term MULF1=1489
%term MULI1=1493
%term MULU1=1494

%term EQF1=1505
%term EQI1=1509
%term EQU1=1510

%term GEF1=1521
%term GEI1=1525
%term GEU1=1526

%term GTF1=1537
%term GTI1=1541
%term GTU1=1542

%term LEF1=1553
%term LEI1=1557
%term LEU1=1558

%term LTF1=1569
%term LTI1=1573
%term LTU1=1574

%term NEF1=1585
%term NEI1=1589
%term NEU1=1590

%term JUMPV=584

%term LABELV=600

%term LOADF1=1249
%term LOADI1=1253
%term LOADU1=1254
%term LOADP1=1255

%term VREGP=711

%%

reg: LOADI1(reg)  "\taddu\t$%c, $0, $%0\n"  1
reg: LOADU1(reg)  "\taddu\t$%c, $0, $%0\n"  1
reg: LOADP1(reg)  "\taddu\t$%c, $0, $%0\n"  1

reg:  INDIRI1(VREGP)     "# read register\n"
reg:  INDIRU1(VREGP)     "# read register\n"
reg:  INDIRP1(VREGP)     "# read register\n"

stmt: ASGNI1(VREGP,reg)  "# write register\n"
stmt: ASGNU1(VREGP,reg)  "# write register\n"
stmt: ASGNP1(VREGP,reg)  "# write register\n"

con16: CNSTI1	"%a"	 range(a, 0, 0xffff)
con16: CNSTU1	"%a"	 range(a, 0, 0xffff)
con16: CNSTP1	"%a"	 range(a, 0, 0xffff)

con20: CNSTI1	"%a"	 range(a, 0, 0xfffff)
con20: CNSTU1	"%a"	 range(a, 0, 0xfffff)
con20: CNSTP1	"%a"	 range(a, 0, 0xfffff)

stmt: reg	""

acon: con20	"%0"
acon: ADDRGP1	"%a"

addr: ADDI1(reg,acon)  "%1($%0)"
addr: ADDU1(reg,acon)  "%1($%0)"
addr: ADDP1(reg,acon)  "%1($%0)"
addr: acon  "%0($0)"
addr: reg   "0($%0)"
addr: ADDRFP1  "%S($sp)"
addr: ADDRLP1  "%S($sp)"

reg: con16    "\taddui\t$%c, $0, %0\n"  1
reg: ADDRGP1  "\tla\t$%c, %a\n"  1
reg: CNSTI1   "\tlhi\t$%c, %H\n\tori\t$%c, $%c, %L\n"	2
reg: CNSTU1   "\tlhi\t$%c, %H\n\tori\t$%c, $%c, %L\n"	2
reg: CNSTP1   "\tlhi\t$%c, %H\n\tori\t$%c, $%c, %L\n"	2

reg: ADDRLP1  "\taddui\t$%c, $sp, %S\n"  1
reg: ADDRFP1  "\taddui\t$%c, $sp, %S\n"  1

reg: CNSTI1  "# reg\n"  range(a, 0, 0)
reg: CNSTU1  "# reg\n"  range(a, 0, 0)
reg: CNSTP1  "# reg\n"  range(a, 0, 0)

stmt: ASGNI1(addr,reg)  "\tsw\t$%1, %0\n"  1
stmt: ASGNU1(addr,reg)  "\tsw\t$%1, %0\n"  1
stmt: ASGNP1(addr,reg)  "\tsw\t$%1, %0\n"  1

reg:  INDIRI1(addr)     "\tlw\t$%c, %0\n"  1
reg:  INDIRU1(addr)     "\tlw\t$%c, %0\n"  1
reg:  INDIRP1(addr)     "\tlw\t$%c, %0\n"  1

reg: DIVI1(reg,reg)  "\tdiv\t$%c, $%0, $%1\n"   1
reg: DIVU1(reg,reg)  "\tdivu\t$%c, $%0, $%1\n"  1
reg: MODI1(reg,reg)  "\trem\t$%c, $%0, $%1\n"   1
reg: MODU1(reg,reg)  "\tremu\t$%c, $%0, $%1\n"  1
reg: MULI1(reg,reg)  "\tmult\t$%c, $%0, $%1\n"   1
reg: MULU1(reg,reg)  "\tmultu\t$%c, $%0, $%1\n"   1

reg: DIVI1(reg,imm)  "\tdivi\t$%c, $%0, %1\n"   1
reg: DIVU1(reg,imm)  "\tdivui\t$%c, $%0, %1\n"  1
reg: MODI1(reg,imm)  "\tremi\t$%c, $%0, %1\n"   1
reg: MODU1(reg,imm)  "\tremui\t$%c, $%0, %1\n"  1
reg: MULI1(reg,imm)  "\tmulti\t$%c, $%0, %1\n"   1
reg: MULU1(reg,imm)  "\tmultui\t$%c, $%0, %1\n"   1

imm: CNSTI1         "%a"                range(a,0,65535)

reg: ADDI1(reg,imm)   "\taddui\t$%c, $%0, %1\n"  1
reg: ADDU1(reg,imm)   "\taddui\t$%c, $%0, %1\n"  1
reg: BANDI1(reg,imm)  "\tandi\t$%c, $%0, %1\n"   1
reg: BORI1(reg,imm)   "\tori\t$%c, $%0, %1\n"    1
reg: BXORI1(reg,imm)  "\txori\t$%c, $%0, %1\n"   1
reg: BANDU1(reg,imm)  "\tandi\t$%c, $%0, %1\n"   1
reg: BORU1(reg,imm)   "\tori\t$%c, $%0, %1\n"    1
reg: BXORU1(reg,imm)  "\txori\t$%c, $%0, %1\n"   1
reg: SUBI1(reg,imm)   "\tsubui\t$%c, $%0, %1\n"  1
reg: SUBU1(reg,imm)   "\tsubui\t$%c, $%0, %1\n"  1

reg: ADDP1(reg,imm)   "\taddui\t$%c, $%0, %1\n"  1
reg: SUBP1(reg,imm)   "\tsubui\t$%c, $%0, %1\n"  1

reg: ADDI1(reg,reg)   "\taddu\t$%c, $%0, $%1\n"  1
reg: ADDU1(reg,reg)   "\taddu\t$%c, $%0, $%1\n"  1
reg: BANDI1(reg,reg)  "\tand\t$%c, $%0, $%1\n"   1
reg: BORI1(reg,reg)   "\tor\t$%c, $%0, $%1\n"    1
reg: BXORI1(reg,reg)  "\txor\t$%c, $%0, $%1\n"   1
reg: BANDU1(reg,reg)  "\tand\t$%c, $%0, $%1\n"   1
reg: BORU1(reg,reg)   "\tor\t$%c, $%0, $%1\n"    1
reg: BXORU1(reg,reg)  "\txor\t$%c, $%0, $%1\n"   1
reg: SUBI1(reg,reg)   "\tsubu\t$%c, $%0, $%1\n"  1
reg: SUBU1(reg,reg)   "\tsubu\t$%c, $%0, $%1\n"  1

reg: ADDP1(reg,reg)   "\taddu\t$%c, $%0, $%1\n"  1
reg: SUBP1(reg,reg)   "\tsubu\t$%c, $%0, $%1\n"  1

reg: LSHI1(reg,imm)  "\tslli\t$%c, $%0, %1\n"  1
reg: LSHU1(reg,imm)  "\tslli\t$%c, $%0, %1\n"  1
reg: RSHI1(reg,imm)  "\tsrai\t$%c, $%0, %1\n"  1
reg: RSHU1(reg,imm)  "\tsrli\t$%c, $%0, %1\n"  1

reg: BCOMI1(reg)  "\tsubu\t$%c, $0, $%0\n\tsubui\t$%c, $%c, 1\n"   2
reg: BCOMU1(reg)  "\tsubu\t$%c, $0, $%0\n\tsubui\t$%c, $%c, 1\n"   2
reg: NEGI1(reg)   "\tsubu\t$%c, $0, $%0\n"  1

reg: LSHI1(reg,reg)  "\tsll\t$%c, $%0, $%1\n"  1
reg: LSHU1(reg,reg)  "\tsll\t$%c, $%0, $%1\n"  1
reg: RSHI1(reg,reg)  "\tsra\t$%c, $%0, $%1\n"  1
reg: RSHU1(reg,reg)  "\tsrl\t$%c, $%0, $%1\n"  1

reg: CVII1(reg)  "%0"
reg: CVUI1(reg)  "%0"
reg: CVUU1(reg)  "%0"

stmt: LABELV  "%a:\n"

stmt: JUMPV(acon)  "\tj\t%0\n"   1
stmt: JUMPV(reg)   "\tjr\t$%0\n"  1

stmt: EQI1(reg,reg)  "\tseq\t$13, $%0, $%1\n\tbnez\t$13, %a\n"	2
stmt: EQU1(reg,reg)  "\tsequ\t$13, $%0, $%1\n\tbnez\t$13, %a\n"   2
stmt: GEI1(reg,reg)  "\tsge\t$13, $%0, $%1\n\tbnez\t$13, %a\n"	2
stmt: GEU1(reg,reg)  "\tsgeu\t$13, $%0, $%1\n\tbnez\t$13, %a\n"   2
stmt: GTI1(reg,reg)  "\tsgt\t$13, $%0, $%1\n\tbnez\t$13, %a\n"	2
stmt: GTU1(reg,reg)  "\tsgtu\t$13, $%0, $%1\n\tbnez\t$13, %a\n"   2
stmt: LEI1(reg,reg)  "\tsle\t$13, $%0, $%1\n\tbnez\t$13, %a\n"	2
stmt: LEU1(reg,reg)  "\tsleu\t$13, $%0, $%1\n\tbnez\t$13, %a\n"   2
stmt: LTI1(reg,reg)  "\tslt\t$13, $%0, $%1\n\tbnez\t$13, %a\n"	2
stmt: LTU1(reg,reg)  "\tsltu\t$13, $%0, $%1\n\tbnez\t$13, %a\n"   2
stmt: NEI1(reg,reg)  "\tsne\t$13, $%0, $%1\n\tbnez\t$13, %a\n"	2
stmt: NEU1(reg,reg)  "\tsneu\t$13, $%0, $%1\n\tbnez\t$13, %a\n"   2

stmt: EQI1(reg,imm)  "\tseqi\t$13, $%0, %1\n\tbnez\t$13, %a\n"	2
stmt: EQU1(reg,imm)  "\tsequi\t$13, $%0, %1\n\tbnez\t$13, %a\n"   2
stmt: GEI1(reg,imm)  "\tsgei\t$13, $%0, %1\n\tbnez\t$13, %a\n"	2
stmt: GEU1(reg,imm)  "\tsgeui\t$13, $%0, %1\n\tbnez\t$13, %a\n"   2
stmt: GTI1(reg,imm)  "\tsgti\t$13, $%0, %1\n\tbnez\t$13, %a\n"	2
stmt: GTU1(reg,imm)  "\tsgtui\t$13, $%0, %1\n\tbnez\t$13, %a\n"   2
stmt: LEI1(reg,imm)  "\tslei\t$13, $%0, %1\n\tbnez\t$13, %a\n"	2
stmt: LEU1(reg,imm)  "\tsleui\t$13, $%0, %1\n\tbnez\t$13, %a\n"   2
stmt: LTI1(reg,imm)  "\tslti\t$13, $%0, %1\n\tbnez\t$13, %a\n"	2
stmt: LTU1(reg,imm)  "\tsltui\t$13, $%0, %1\n\tbnez\t$13, %a\n"   2
stmt: NEI1(reg,imm)  "\tsnei\t$13, $%0, %1\n\tbnez\t$13, %a\n"	2
stmt: NEU1(reg,imm)  "\tsneui\t$13, $%0, %1\n\tbnez\t$13, %a\n"   2

reg:  CALLI1(acon)  "\tjal\t%0\n"  1
reg:  CALLP1(acon)  "\tjal\t%0\n"  1
reg:  CALLU1(acon)  "\tjal\t%0\n"  1
stmt: CALLV(acon)   "\tjal\t%0\n"  1

reg:  CALLI1(reg)  "\tjalr\t$%0\n"  1
reg:  CALLP1(reg)  "\tjalr\t$%0\n"  1
reg:  CALLU1(reg)  "\tjalr\t$%0\n"  1
stmt: CALLV(reg)   "\tjalr\t$%0\n"  1

stmt: RETI1(reg)  "# ret\n"  1
stmt: RETU1(reg)  "# ret\n"  1
stmt: RETP1(reg)  "# ret\n"  1
stmt: RETV(reg)   "# ret\n"  1

stmt: ARGI1(reg)  "# arg\n"  1
stmt: ARGP1(reg)  "# arg\n"  1
stmt: ARGU1(reg)  "# arg\n"  1

stmt: ARGB(INDIRB(reg))       "# argb %0\n"      1
stmt: ASGNB(reg,INDIRB(reg))  "# asgnb %0 %1\n"  1

%%
static void progend(void){}

static void progbeg(int argc, char *argv[]) {
  int i;
  {
    union {
      char c;
      int i;
    } u;
    u.i = 0;
    u.c = 1;
    swap = ((int)(u.i == 1)) != IR->little_endian;
  }

  parseflags(argc, argv);

  for (i = 0 ; i < 31 ; i += 2)
    freg2[i] = mkreg("%d", i, 3, FREG);

  for (i = 0 ; i < 32 ; i++)
    ireg[i]  = mkreg("%d", i, 1, IREG);

  ireg[14]->x.name = "sp";
  freg2w = mkwildcard(freg2);
  iregw = mkwildcard(ireg);
  tmask[IREG] = INTTMP; tmask[FREG] = FLTTMP;
  vmask[IREG] = INTVAR; vmask[FREG] = FLTVAR;
  blkreg = mkreg("9", 9, 7, IREG);
}

static Symbol rmap(int opk) {
//       fprintf(stderr, "in rmap\n");
  switch (optype(opk)) {
    case I: case U: case P: case B:
//    fprintf(stderr, "need integer reg\n");
      return iregw;
    case F:
      return freg2w;
    default:
      return 0;
  }
}

static void target(Node p) {
  assert(p);

  switch (specific(p->op)) {
    case CNST+I: case CNST+U: case CNST+P:
      if (range(p, 0, 0) == 0) {
        setreg(p, ireg[0]);
        p->x.registered = 1;
      }
      break;
    case CALL+V:
      break;
    case CALL+F:
      setreg(p, freg2[0]);
      break;
    case CALL+I: case CALL+P: case CALL+U:
      setreg(p, ireg[1]);
      break;
    case RET+F:
      rtarget(p, 0, freg2[0]);
      break;
    case RET+I: case RET+U: case RET+P:
      rtarget(p, 0, ireg[1]);
      break;
    case ASGN+B:
      rtarget(p->kids[1], 0, blkreg);
      break;
    case ARG+B:
      rtarget(p->kids[0], 0, blkreg);
      break;
  }
}

static void clobber(Node p) {
  assert(p);

//  fprintf(stderr, "Called clobber()\n");

  switch (specific(p->op)) {
    case CALL+F:
      spill(INTRET, IREG, p);
      break;
    case CALL+I: case CALL+P: case CALL+U:
      break;
    case CALL+V:
      spill(INTRET, IREG, p);
      break;
    case ASGN+B:
      spill((1 << 8), IREG, p);
      break;
    case ARG+B:
      spill((1 << 7), IREG, p);
      break;
  }
  /* The tests make use of $13,
     so we must spill this register if we see one of these nodes. */
  switch (generic(p->op)) {
    case EQ: case GE: case GT: case LE: case LT: case NE:
      spill((1 << 13), IREG, p);
      break;
  }

//  fprintf(stderr, "Done with clobber\n");
}

static void emit2(Node p) {
  int src;

  switch (specific(p->op)) {
    case ARG+I: case ARG+P: case ARG+U:
      src = getregnum(p->x.kids[0]);
      print("\tsw\t$%d, %d($sp)\n", src, p->syms[2]->u.c.v.i);
      break;
    case ASGN+B:
      dalign = salign = p->syms[1]->u.c.v.i;
      blkcopy(getregnum(p->x.kids[0]), 0,
              getregnum(p->x.kids[1]), 0,
              p->syms[0]->u.c.v.i, tmpregs);
      break;
    case ARG+B:
      dalign = salign = p->syms[1]->u.c.v.i;
      blkcopy(14, p->syms[2]->u.c.v.i,
              getregnum(p->x.kids[0]), 0,
              p->syms[0]->u.c.v.i, tmpregs);
      break;
  }
}

static void doarg(Node p) {
  static int argno;
  int align;

  if (argoffset == 0)
    argno = 0;

  p->x.argno = argno++;

  align = p->syms[1]->u.c.v.i;

  p->syms[2] = intconst(mkactual(align, p->syms[0]->u.c.v.i));
}

static void local(Symbol p) {
  if (askregvar(p, rmap(ttob(p->type))) == 0)
    mkauto(p);
}

static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls) {
  int i, saved, sizeisave;
  Symbol r;

  usedmask[0] = usedmask[1] = 0;
  freemask[0] = freemask[1] = ~(unsigned)0;
  offset = maxoffset = maxargoffset = 0;

  for (i = 0 ; callee[i] ; i++) {
    Symbol p = callee[i];
    Symbol q = caller[i];
    assert(q);
    offset = roundup(offset, q->type->align);

    p->x.offset = q->x.offset = offset;
    p->x.name = q->x.name = stringd(offset);
    p->sclass = q->sclass = AUTO;

    offset += q->type->size;
  }

  assert(!caller[i]);

  offset = 0;

  gencode(caller, callee);

  if (ncalls)
    usedmask[IREG] |= ((unsigned)1) << 15;

  usedmask[IREG] &= 0xffffbffc;
  usedmask[FREG] &= 0xffff0000;

  sizeisave = bitcount(usedmask[IREG]);

  framesize = maxargoffset + sizeisave + maxoffset;

  segment(CODE);
  print("%s:\n", f->x.name);

//  print("\t.frame\t$sp, %d, $ra\t# params = %d, regsave = %d, locals = %d\n",
//        framesize, maxargoffset, sizeisave, maxoffset);

//  if (usedmask[IREG])
//    print("\t.mask\t0x%x\n", usedmask[IREG]);

  if (framesize > 0)
    print("\tsubui\t$sp, $sp, %d\n", framesize);


  saved = maxargoffset;

  for (i = 1 ; i <= 31 ; i++)
    if (usedmask[IREG] & (1 << i)) {
      if (i == 15)
	print("\tsw\t$ra, %d($sp)\n", saved);
      else
        print("\tsw\t$%d, %d($sp)\n", i, saved);
      saved++;
    }


  emitcode();

  saved = maxargoffset;

  for (i = 1 ; i <= 31 ; i++)
    if (usedmask[IREG] & (1 << i)) {
      if (i == 15)
	print("\tlw\t$ra, %d($sp)\n", saved);
      else
        print("\tlw\t$%d, %d($sp)\n", i, saved);
      saved++;
    }

  if (framesize > 0)
    print("\taddui\t$sp, $sp, %d\n", framesize);
  print("\tjr\t$ra\n");
}


static void defconst(int suffix, int size, Value v) {
  if (suffix == F) {
    float f = v.d;
    print("\t.word\t0x%x\n", *(unsigned *)&f);
  }
  else if (suffix == P)
    print("\t.word\t0x%x\n", v.p);
  else if (size == 1)
    print("\t.word\t0x%x\n", suffix == I ? v.i : v.u);
}

static void defaddress(Symbol p) {
  print("\t.word\t%s\n", p->x.name);
}

static void defstring(int n, char *str) {
  char *s = str;

  if (s[n - 1] == '\0')
    print("\t.asciiz\t\"");
  else
    print("\t.ascii\t\"");

  for ( ; s < (str + n - 1) ; s++) {
    if (*s == '\n')
      print("\\n");
    else if (*s == '\"')
      print("\\\"");
    else if (*s == '\r')
      print("\\r");
    else if (*s == '\t')
      print("\\t");
    else if (*s == '\0')
      print("\\0");
    else if (isprint(*s))
      print("%c", *s);
    else
      assert(0); //fprintf(stderr, "I can't print this character (line 617 wramp.md)\n");
  }

  print("\"\n");
  
/*
  for (s = str ; s < str + n ; s++)
    print("\t.word\t%d\n", (*s)&0377);
*/
}

static void export(Symbol p) {
  print(".global\t%s\n", p->x.name);
}

static void import(Symbol p) {
  if (!isfunc(p->type))
    print(".extern\t%s %d\n", p->name, p->type->size);
}

static void defsymbol(Symbol p) {
  if (p->scope >= LOCAL && p->sclass == STATIC)
    p->x.name = stringf("L.%d", genlabel(1));
  else if (p->generated)
    p->x.name = stringf("L.%s", p->name);
  else
    assert(p->scope != CONSTANTS || isint(p->type) || isptr(p->type)), p->x.name = p->name;
}

static void address(Symbol q, Symbol p, long n) {
  if (p->scope == GLOBAL || p->sclass == STATIC || p->sclass == EXTERN)
    q->x.name = stringf("%s%s%D", p->x.name, n >= 0 ? "+" : "", n);
  else {
    assert(n <= INT_MAX && n >= INT_MIN);
    q->x.offset = p->x.offset + n;
    q->x.name = stringd(q->x.offset);
  }
}

static void global(Symbol p) {
  segment(p->u.seg);
  print("%s:\n", p->x.name);
}

static void segment(int n) {
  if (cseg != n) {
    cseg = n;
    switch (n) {
      case CODE: print(".text\n");  break;
      case LIT:  print(".data\n"); break;
      case BSS:  print(".bss\n"); break;
    }
  }
}

static void space(int n) {
int i;
if (cseg == BSS) {
  print("\t.space\t%d\n", n);
}
else {
  for (i = 0 ; i < n ; i++)
    print("\n.word\t0\n");
}
}

static void blkloop(int dreg, int doff, int sreg, int soff, int size, int tmps[]) {
  int lab = genlabel(1);

  print("\taddui\t$%s, $%s, %d\n", ireg[sreg]->x.name, ireg[sreg]->x.name, size);
  print("\taddui\t$%s, $%s, %d\n", ireg[tmps[2]]->x.name, ireg[dreg]->x.name, size);

  print("L.%d:\n", lab);

  print("\tsubui\t$%s, $%s, %d\n", ireg[sreg]->x.name, ireg[sreg]->x.name, 1);
  print("\tsubui\t$%d, $%d, %d\n", tmps[2], tmps[2], 1);

  blkcopy(tmps[2], doff, sreg, soff, 1, tmps);

  print("\tsltu\t$%d, $%s, $%d\n", tmps[1], ireg[dreg]->x.name, tmps[2]);
  print("\tbnez\t$%d, L.%d\n", tmps[1], lab);
}

static void blkfetch(int size, int off, int reg, int tmp) {
  assert(size == 1);
  print("\tlw\t$%s, %d($%s)\n",  ireg[tmp]->x.name, off, ireg[reg]->x.name);
}

static void blkstore(int size, int off, int reg, int tmp) {
  assert(size == 1);
  print("\tsw\t$%s, %d($%s)\n",  ireg[tmp]->x.name, off, ireg[reg]->x.name);
}

static void stabinit(char *, int, char *[]);
static void stabline(Coordinate *);
static void stabsym(Symbol);

static char *currentfile;

static int bitcount(unsigned mask) {
  unsigned i, n = 0;

  for (i = 1 ; i ; i <<= 1)
    if (mask & i)
      n++;
  return n;
}

/* stabinit - initialize stab output */
static void stabinit(char *file, int argc, char *argv[]) {
  if (file) {
    print(".file 2,\"%s\"\n", file);
    currentfile = file;
  }
}

/* stabline - emit stab entry for source coordinate *cp */
static void stabline(Coordinate *cp) {
  if (cp->file && cp->file != currentfile) {
    print(".file 2,\"%s\"\n", cp->file);
    currentfile = cp->file;
  }
  print(".loc 2,%d\n", cp->y);
}

/* stabsym - output a stab entry for symbol p */
static void stabsym(Symbol p) {
  if (p == cfunc && IR->stabline)
    (*IR->stabline)(&p->src);
}

Interface wrampIR = {
        1, 1, 0,  /* char */
        1, 1, 0,  /* short */
        1, 1, 0,  /* int */
        1, 1, 0,  /* long */
        1, 1, 0,  /* long long */
        1, 1, 1,  /* float */
        1, 1, 1,  /* double */
        1, 1, 1,  /* long double */
        1, 1, 0,  /* T * */
        0, 1, 0,  /* struct */
        0,      /* little_endian */
        0,  /* mulops_calls */
        0,  /* wants_callb */
        1,  /* wants_argb */
        1,  /* left_to_right */
        0,  /* wants_dag */
        0,  /* unsigned_char */
	32, /* unit size */
        address,
        blockbeg,
        blockend,
        defaddress,
        defconst,
        defstring,
        defsymbol,
        emit,
        export,
        function,
        gen,
        global,
        import,
        local,
        progbeg,
        progend,
        segment,
        space,
        0, 0, 0, stabinit, stabline, stabsym, 0,
        {
                1,      /* max_unaligned_load */
                rmap,
                blkfetch, blkstore, blkloop,
                _label,
                _rule,
                _nts,
                _kids,
                _string,
                _templates,
                _isinstruction,
                _ntname,
                emit2,
                doarg,
                target,
                clobber,

        }
};
static char rcsid[] = "$Id: wramp.md,v 1.2 2002/10/25 01:22:06 daa1 Exp $";
