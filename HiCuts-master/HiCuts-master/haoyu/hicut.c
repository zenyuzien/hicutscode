#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <time.h>       // for clock_t, clock(), CLOCKS_PER_SEC

typedef char bit;
// const int false = 0;
// const int true = 1;

const int Null = -1;
const int BIGINT = 0x7fffffff;
const int EOS = '\0';

inline int max(int x, int y) { return x > y ? x : y; }
inline unsigned max(unsigned x, unsigned y) { return x > y ? x : y; }
inline double max(double x, double y) { return x > y ? x : y; }
inline int min(int x, int y) { return x < y ? x : y; }
inline unsigned min(unsigned x, unsigned y) { return x < y ? x : y; }
inline double min(double x, double y) { return x < y ? x : y; }
inline int abs(int x) { return x < 0 ? -x : x; }

inline void warning(char* p) { fprintf(stderr,"Warning:%s \n",p); }
inline void fatal(char* string) {fprintf(stderr,"Fatal:%s\n",string); exit(1); }

double pow(double,double);
double log(double);

// long random();
double exp(double),log(double);

// Return a random number in [0,1] 
inline double randfrac() { return ((double) random())/BIGINT; }

// Return a random integer in the range [lo,hi].
// Not very good if range is larger than 10**7.
inline int randint(int lo, int hi) { return lo + (random() % (hi + 1 - lo)); }

// Return a random number from an exponential distribution with mean mu 
inline double randexp(double mu) { return -mu*log(randfrac()); }

// Return a random number from a geometric distribution with mean 1/p
inline int randgeo(double p) { return int(.999999 + log(randfrac())/log(1-p)); }

// Return a random number from a Pareto distribution with mean mu and shape s
inline double randpar(double mu, double s) {
	return mu*(1-1/s)/exp((1/s)*log(randfrac()));
}
#define MAXNODES 1000000
#define MAXRULES 10000
#define MAXBUCKETS 40 
#define MAXCUTS 64
#define MAXDIMENSIONS 5
#define RULESIZE 4.5 
#define NODESIZE 4 
#define RULEPTSIZE 0.5 

struct range{
  unsigned low;
  unsigned high;
};

struct pc_rule{
  struct range field[MAXDIMENSIONS];
};


class trie {
	struct nodeItem {
		bool isleaf;            //is a leaf node if true
  		int nrules;             //number of rules in this node  
            int *ruleid;             //rule ids in this node
  		struct range field[MAXDIMENSIONS];      
  		unsigned int dim : 3;  	//0:sip; 1:dip; 2:proto; 3:sp; 4:dp
  		unsigned int ncuts; //number of cuts
            int *child;             //child pointers
	};
	int	N;			// max number of nodes in trie
	int	n;			// current number of nodes in trie
	int   pass;             // max trie depth
	int   n2;               // removed rules during preprocessing
	int   n3;               // number of rules stored
	float n4;               // number of memory access;
	int   binth;                 
	float spfac;
	int   numrules;
	int   opt;              // dimension choose option
	struct pc_rule *rule;
	int 	root;			// root of trie
	int 	freelist;		// first nodeItem on free list
	nodeItem *nodeSet;	// base of array of NodeItems
	
	void    choose_np_dim(nodeItem *v, int *d, int *np);
	void    remove_redundancy(nodeItem *v); 
	void    createtrie();

public:		//trie(int=10000, int=100, int=16, int=1000, struct pc_rule*, int=1);
            trie(int, int, int, float, struct pc_rule*, int);
		~trie();

	int     trieLookup(int*);
	int	  trieSize();
	int     trieDepth();
	int     trieRedun();
	int     trieRule();
	float   trieMemAccess();
};

inline int trie::trieSize() {return n;}
inline int trie::trieDepth() {return pass;}
inline int trie::trieRedun() {return n2;}
inline int trie::trieRule() {return n3;}
inline float trie::trieMemAccess() {return n4;}



int binth = 16;      // leaf threashold
float spfac = 2;     // space explosion factor
int opt = 3;         // dimension selection
FILE *fpr;           // ruleset file
FILE *fpt;           // test trace file

int loadrule(FILE *fp, pc_rule *rule){
  
  int tmp;
  unsigned sip1, sip2, sip3, sip4, siplen;
  unsigned dip1, dip2, dip3, dip4, diplen;
  unsigned proto, protomask;
  int i = 0;
  
  while(1){
    
    if(fscanf(fp,"@%d.%d.%d.%d/%d\t%d.%d.%d.%d/%d\t%d : %d\t%d : %d\t%x/%x\n", 
        &sip1, &sip2, &sip3, &sip4, &siplen, &dip1, &dip2, &dip3, &dip4, &diplen, 
        &rule[i].field[2].low, &rule[i].field[2].high, &rule[i].field[3].low, &rule[i].field[3].high,
        &proto, &protomask) != 16) break;
    if(siplen == 0){
      rule[i].field[0].low = 0;
      rule[i].field[0].high = 0xFFFFFFFF;
    }else if(siplen > 0 && siplen <= 8){
      tmp = sip1<<24;
      rule[i].field[0].low = tmp;
      rule[i].field[0].high = rule[i].field[0].low + (1<<(32-siplen)) - 1;
    }else if(siplen > 8 && siplen <= 16){
      tmp = sip1<<24; tmp += sip2<<16;
      rule[i].field[0].low = tmp; 	
      rule[i].field[0].high = rule[i].field[0].low + (1<<(32-siplen)) - 1;	
    }else if(siplen > 16 && siplen <= 24){
      tmp = sip1<<24; tmp += sip2<<16; tmp +=sip3<<8; 
      rule[i].field[0].low = tmp; 	
      rule[i].field[0].high = rule[i].field[0].low + (1<<(32-siplen)) - 1;			
    }else if(siplen > 24 && siplen <= 32){
      tmp = sip1<<24; tmp += sip2<<16; tmp += sip3<<8; tmp += sip4;
      rule[i].field[0].low = tmp; 
      rule[i].field[0].high = rule[i].field[0].low + (1<<(32-siplen)) - 1;	
    }else{
      printf("Src IP length exceeds 32\n");
      return 0;
    }
    if(diplen == 0){
      rule[i].field[1].low = 0;
      rule[i].field[1].high = 0xFFFFFFFF;
    }else if(diplen > 0 && diplen <= 8){
      tmp = dip1<<24;
      rule[i].field[1].low = tmp;
      rule[i].field[1].high = rule[i].field[1].low + (1<<(32-diplen)) - 1;
    }else if(diplen > 8 && diplen <= 16){
      tmp = dip1<<24; tmp +=dip2<<16;
      rule[i].field[1].low = tmp; 	
      rule[i].field[1].high = rule[i].field[1].low + (1<<(32-diplen)) - 1;	
    }else if(diplen > 16 && diplen <= 24){
      tmp = dip1<<24; tmp +=dip2<<16; tmp+=dip3<<8;
      rule[i].field[1].low = tmp; 	
      rule[i].field[1].high = rule[i].field[1].low + (1<<(32-diplen)) - 1;			
    }else if(diplen > 24 && diplen <= 32){
      tmp = dip1<<24; tmp +=dip2<<16; tmp+=dip3<<8; tmp +=dip4;
      rule[i].field[1].low = tmp; 	
      rule[i].field[1].high = rule[i].field[1].low + (1<<(32-diplen)) - 1;	
    }else{
      printf("Dest IP length exceeds 32\n");
      return 0;
    }
    if(protomask == 0xFF){
      rule[i].field[4].low = proto;
      rule[i].field[4].high = proto;
    }else if(protomask == 0){
      rule[i].field[4].low = 0;
      rule[i].field[4].high = 0xFF;
    }else{
      printf("Protocol mask error\n");
      return 0;
    }
    
    i++;
  }
  return i;
}

void parseargs(int argc, char *argv[]) {
  int	c;
  bool	ok = 1;
  while ((c = getopt(argc, argv, "b:s:o:r:t:h")) != -1) {
    switch (c) {
	case 'b':
	  binth = atoi(optarg);
	  break;
	case 's':
	  spfac = atof(optarg);
	  break;
	case 'o':
	  opt = atoi(optarg);
	  break;
	case 'r':
	  fpr = fopen(optarg, "r");
          break;
	case 't':
	  fpt = fopen(optarg, "r");
	  break;
	case 'h':
	  printf("hicut [-b binth][-s spfac][-o opt][-r ruleset][-t trace]\n");
	  printf("mail me: hs1@arl.wustl.edu\n");
	  exit(1);
	  break;
	default:
	  ok = 0;
    }
  }
  
  if(binth <= 0 || binth > MAXBUCKETS){
    printf("binth should be greater than 0 and less than %d\n", MAXBUCKETS);
    ok = 0;
  }	
  if(spfac < 1.0){
    printf("space factor should be >= 1\n");
    ok = 0;
  }	
  if(opt < 0 || opt > 3){
    printf("option shoudl be between 0 and 3\n");
    ok = 0;	
  }
  if(fpr == NULL){
    printf("can't open ruleset file\n");
    ok = 0;
  }
  if (!ok || optind < argc) {
    fprintf (stderr, "hicut [-b binth][-s spfac][-o opt][-r ruleset][-t trace]\n");
    fprintf (stderr, "Type \"hicut -h\" for help\n");
    exit(1);
  }
  
  printf("Bucket Size =  %d\n", binth);
  printf("Space Factor = %f\n", spfac);
  printf("Option = %d\n", opt);
  
}

int main(int argc, char* argv[]){

  int numrules=0;  // actual number of rules in rule set
  struct pc_rule *rule; 
  int i,j;
  int header[MAXDIMENSIONS];
  int matchid, fid;
  char *s = (char *)calloc(200, sizeof(char));
  
  double time_spent = 0.0;
  clock_t begin = clock();
  parseargs(argc, argv);
   
  while(fgets(s, 200, fpr) != NULL)numrules++;
  rewind(fpr);
          
  rule = (pc_rule *)calloc(numrules, sizeof(pc_rule));
  numrules = loadrule(fpr, rule);
  
  printf("the number of rules = %d\n", numrules);
  
  //for(i=0;i<numrules;i++){
  //  printf("%d: %x:%x %x:%x %u:%u %u:%u %u:%u\n", i,
  //    rule[i].field[0].low, rule[i].field[0].high, 
  //    rule[i].field[1].low, rule[i].field[1].high,
  //    rule[i].field[2].low, rule[i].field[2].high,
  //    rule[i].field[3].low, rule[i].field[3].high, 
  //    rule[i].field[4].low, rule[i].field[4].high);
  //}
  
  trie T(1000000, numrules, binth, spfac, rule, opt);

  clock_t end = clock();
    
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC * 1000.0; // Convert to milliseconds
 
    printf("Time taken: %f milliseconds\n", time_spent);
  
  printf("*************************\n");
  printf("number of nodes = %d\n", T.trieSize());
  printf("max trie depth = %d\n", T.trieDepth()); 
  printf("remove redun = %d\n", T.trieRedun());
  printf("Strored rules = %d\n", T.trieRule());
  printf("Bytes/filter = %f\n", (T.trieSize()*NODESIZE + numrules*RULESIZE + T.trieRule()*RULEPTSIZE)*4/numrules);
  printf("kB = %f\n", ((T.trieSize()*NODESIZE + numrules*RULESIZE + T.trieRule()*RULEPTSIZE)*4)/1024);
  printf("*************************\n");
  
  if(fpt != NULL){
    i=0; j=0;
    while(fscanf(fpt,"%u %u %d %d %d %d\n", 
          &header[0], &header[1], &header[2], &header[3], &header[4], &fid) != Null){
      i++;
      
      if((matchid = T.trieLookup(header)) == -1){
        printf("? packet %d match NO rule, should be %d\n", i, fid+1);
        j++;
      }else if(matchid == fid-1){
        //printf("packet %d match rule %d\n", i, matchid);
      }else if(matchid > fid-1){
        printf("? packet %d match lower priority rule %d, should be %d\n", i, matchid+1, fid);
        j++;
      }else{
        printf("! packet %d match higher priority rule %d, should be %d\n", i, matchid+1, fid);
      }
    }
    printf("%d packets are classified, %d of them are misclassified\n", i, j);
    printf("# of bytes accessed/pkt = %f\n", T.trieMemAccess()*4.0/i);
  }else{
    printf("No packet trace input\n");
  }
  
}  
