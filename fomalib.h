/*     Foma: a finite-state toolkit and library.                             */
/*     Copyright © 2008-2011 Mans Hulden                                     */

/*     This file is part of foma.                                            */

/*     Foma is free software: you can redistribute it and/or modify          */
/*     it under the terms of the GNU General Public License version 2 as     */
/*     published by the Free Software Foundation. */

/*     Foma is distributed in the hope that it will be useful,               */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */

/*     You should have received a copy of the GNU General Public License     */
/*     along with foma.  If not, see <http://www.gnu.org/licenses/>.         */

#ifdef  __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "zlib.h"

#define FEXPORT __attribute__((visibility("default")))

/* Library version */
#define MAJOR_VERSION 0
#define MINOR_VERSION 9
#define BUILD_VERSION 15
#define STATUS_VERSION "alpha"

/* Special symbols on arcs */
#define EPSILON 0
#define UNKNOWN 1
#define IDENTITY 2

/* Variants of ignore operation */
#define OP_IGNORE_ALL 1
#define OP_IGNORE_INTERNAL 2

/* Replacement direction */
#define OP_UPWARD_REPLACE 1
#define OP_RIGHTWARD_REPLACE 2
#define OP_LEFTWARD_REPLACE 3
#define OP_DOWNWARD_REPLACE 4

/* Arrow types in fsmrules */
#define ARROW_RIGHT 1
#define ARROW_LEFT 2
#define ARROW_OPTIONAL 4
#define ARROW_DOTTED 8         /* This is for the [..] part of a dotted rule */
#define ARROW_LONGEST_MATCH 16
#define ARROW_SHORTEST_MATCH 32
#define ARROW_LEFT_TO_RIGHT 64
#define ARROW_RIGHT_TO_LEFT 128

/* Flag types */
#define FLAG_UNIFY 1
#define FLAG_CLEAR 2
#define FLAG_DISALLOW 4
#define FLAG_NEGATIVE 8
#define FLAG_POSITIVE 16
#define FLAG_REQUIRE 32
#define FLAG_EQUAL 64

#define NO  0
#define YES 1
#define UNK 2

#define PATHCOUNT_CYCLIC -1
#define PATHCOUNT_OVERFLOW -2
#define PATHCOUNT_UNKNOWN -3

#define M_UPPER 1
#define M_LOWER 2

/* Automaton structures */

/** Main automaton structure */
struct fsm {
  char name[40];
  int arity;
  int arccount;
  int statecount;
  int linecount;
  int finalcount;
  long long pathcount;
  int is_deterministic;
  int is_pruned;
  int is_minimized;
  int is_epsilon_free;
  int is_loop_free;
  int is_completed;
  struct fsm_state *states;             /* pointer to first line */
  struct sigma *sigma;
  struct medlookup *medlookup;
};

/* Minimum edit distance structure */

struct medlookup {
    int bytes_per_letter_array; /* How many bytes we need to store fut. symbols */
    uint8_t *letterbits;        /* Future symbols of length inf */
    uint8_t *nletterbits;       /* Future symbols of length n   */
    int *confusion_matrix;      /* Confusion matrix */
};

/** Array of states */
struct fsm_state {
    int state_no; /* State number */
    short int in ;
    short int out ;
    int target;
    char final_state ;
    char start_state ;
};

struct fsmcontexts {
    struct fsm *left;
    struct fsm *right;
    struct fsmcontexts *next;
    struct fsm *cpleft;      /* Only used internally when compiling rewrite rules */
    struct fsm *cpright;     /* ditto */
};

struct fsmrules {
    struct fsm *left;
    struct fsm *right;   
    struct fsm *right2;    /*Only needed for A -> B ... C rules*/
    struct fsm *cross_product;
    struct fsmrules *next;
    int arrow_type;
    int dotted;           /* [.A.] rule */
};

struct rewrite_set {
    struct fsmrules *rewrite_rules;
    struct fsmcontexts *rewrite_contexts;
    struct rewrite_set *next;
    struct fsm *cpunion;
    int rule_direction;    /* || \\ // \/ */
};

FEXPORT void fsm_clear_contexts(struct fsmcontexts *contexts);

/** Linked list of sigma */
/** number < IDENTITY is reserved for special symbols */
struct sigma {
    int number;
    char *symbol;
    struct sigma *next;
};

#include "fomalibconf.h"

/********************/
/* Basic operations */
/********************/

FEXPORT char *fsm_get_library_version_string();

FEXPORT struct fsm *fsm_determinize(struct fsm *net);
FEXPORT struct fsm *fsm_epsilon_remove(struct fsm *net);
    FEXPORT struct fsm *fsm_find_ambiguous(struct fsm *net, int **extras);
FEXPORT struct fsm *fsm_minimize(struct fsm *net);
FEXPORT struct fsm *fsm_coaccessible(struct fsm *net);
FEXPORT struct fsm *fsm_topsort(struct fsm *net);

FEXPORT struct fsm *fsm_mark_ambiguous(struct fsm *net);
FEXPORT struct fsm *fsm_sequentialize(struct fsm *net);
FEXPORT struct fsm *fsm_bimachine(struct fsm *net);
FEXPORT struct fsm *fsm_parse_regex(char *regex);
FEXPORT struct fsm *fsm_reverse(struct fsm *net);
FEXPORT struct fsm *fsm_invert(struct fsm *net);
FEXPORT struct fsm *fsm_lower(struct fsm *net);
FEXPORT struct fsm *fsm_upper(struct fsm *net);
FEXPORT struct fsm *fsm_kleene_star(struct fsm *net);
FEXPORT struct fsm *fsm_kleene_plus(struct fsm *net);
FEXPORT struct fsm *fsm_optionality(struct fsm *net);
FEXPORT struct fsm *fsm_boolean(int value);
FEXPORT struct fsm *fsm_concat(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_concat_n(struct fsm *net1, int n);
FEXPORT struct fsm *fsm_concat_m_n(struct fsm *net1, int m, int n);
FEXPORT struct fsm *fsm_union(struct fsm *net_1, struct fsm *net_2);
FEXPORT struct fsm *fsm_priority_union_upper(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_priority_union_lower(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_intersect(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_compose(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_lenient_compose(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_cross_product(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_shuffle(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_precedes(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_follows(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_symbol(char *symbol);
FEXPORT struct fsm *fsm_explode(char *symbol);
FEXPORT struct fsm *fsm_escape(char *symbol);
FEXPORT struct fsm *fsm_copy(struct fsm *net);
FEXPORT struct fsm *fsm_complete(struct fsm *net);
FEXPORT struct fsm *fsm_complement(struct fsm *net);
FEXPORT struct fsm *fsm_term_negation(struct fsm *net1);
FEXPORT struct fsm *fsm_minus(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_simple_replace(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_context_restrict(struct fsm *X, struct fsmcontexts *LR);
FEXPORT struct fsm *fsm_contains(struct fsm *net);
FEXPORT struct fsm *fsm_contains_opt_one(struct fsm *net);
FEXPORT struct fsm *fsm_contains_one(struct fsm *net);
FEXPORT struct fsm *fsm_ignore(struct fsm *net1, struct fsm *net2, int operation);
FEXPORT struct fsm *fsm_quotient_right(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_quotient_left(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_quotient_interleave(struct fsm *net1, struct fsm *net2);
FEXPORT struct fsm *fsm_substitute_label(struct fsm *net, char *original, struct fsm *substitute);
FEXPORT struct fsm *fsm_substitute_symbol(struct fsm *net, char *original, char *substitute);
FEXPORT struct fsm *fsm_universal();
FEXPORT struct fsm *fsm_empty_set();
FEXPORT struct fsm *fsm_empty_string();
FEXPORT struct fsm *fsm_identity();
FEXPORT struct fsm *fsm_quantifier(char *string);
FEXPORT struct fsm *fsm_logical_eq(char *string1, char *string2);
FEXPORT struct fsm *fsm_logical_precedence(char *string1, char *string2);
FEXPORT struct fsm *fsm_lowerdet(struct fsm *net);
FEXPORT struct fsm *fsm_lowerdeteps(struct fsm *net);
FEXPORT struct fsm *fsm_markallfinal(struct fsm *net);
FEXPORT struct fsm *fsm_extract_nonidentity(struct fsm *net);
FEXPORT struct fsm *fsm_extract_ambiguous_domain(struct fsm *net);
FEXPORT struct fsm *fsm_extract_ambiguous(struct fsm *net);
FEXPORT struct fsm *fsm_extract_unambiguous(struct fsm *net);
FEXPORT struct fsm *fsm_sigma_net(struct fsm *net);
FEXPORT struct fsm *fsm_sigma_pairs_net(struct fsm *net);
FEXPORT struct fsm *fsm_equal_substrings(struct fsm *net, struct fsm *left, struct fsm *right);
FEXPORT struct fsm *fsm_letter_machine(struct fsm *net);

/* Remove those symbols from sigma that have the same distribution as IDENTITY */
FEXPORT void fsm_compact(struct fsm *net);

/* Eliminate flag diacritics and return equivalent FSM          */
/* with name = NULL the function eliminates all flag diacritics */
FEXPORT struct fsm *flag_eliminate(struct fsm *net, char *name);

/* Enforce twosided flag diacritics */
FEXPORT struct fsm *flag_twosided(struct fsm *net);

/* Compile a rewrite rule */
FEXPORT struct fsm *fsm_rewrite();

/* Boolean tests */
FEXPORT int fsm_isempty(struct fsm *net);
FEXPORT int fsm_isfunctional(struct fsm *net);
FEXPORT int fsm_isunambiguous(struct fsm *net);
FEXPORT int fsm_isidentity(struct fsm *net);
FEXPORT int fsm_isuniversal(struct fsm *net);
FEXPORT int fsm_issequential(struct fsm *net);

/* Test if a symbol occurs in a FSM */
/* side = M_UPPER (upper side) M_LOWER (lower side), M_UPPER+M_LOWER (both) */
FEXPORT int fsm_symbol_occurs(struct fsm *net, char *symbol, int side);

/* Merges two alphabets destructively */
FEXPORT void fsm_merge_sigma(struct fsm *net1, struct fsm *net2);

/* Copies an alphabet */

FEXPORT struct sigma *sigma_copy(struct sigma *sigma);

/* Create empty FSM */
FEXPORT struct fsm *fsm_create(char *name);
FEXPORT struct fsm_state *fsm_empty();

/* Frees alphabet */
FEXPORT int fsm_sigma_destroy(struct sigma *sigma);

/* Frees a FSM, associated data such as alphabet and confusion matrix */
FEXPORT int fsm_destroy(struct fsm *net);



/* IO functions */
FEXPORT struct fsm *read_att(char *filename);
FEXPORT int net_print_att(struct fsm *net, FILE *outfile);
FEXPORT struct fsm *fsm_read_prolog(char *filename);
FEXPORT char *file_to_mem(char *name);
FEXPORT struct fsm *fsm_read_binary_file(char *filename);
FEXPORT struct fsm *fsm_read_binary_file_multiple(fsm_read_binary_handle fsrh);
FEXPORT fsm_read_binary_handle fsm_read_binary_file_multiple_init(char *filename);
FEXPORT struct fsm *fsm_read_text_file(char *filename);
FEXPORT struct fsm *fsm_read_spaced_text_file(char *filename);
FEXPORT int load_defined(char *filename);
FEXPORT int save_defined();
FEXPORT int save_stack_att();
FEXPORT int write_prolog(struct fsm *net, char *filename);
FEXPORT int foma_net_print(struct fsm *net, gzFile *outfile);

/* Lookups */

/* Frees memory alloced by apply_init */
FEXPORT void apply_clear(struct apply_handle *h);
/* To be called before applying words */
FEXPORT struct apply_handle *apply_init(struct fsm *net);

FEXPORT char *apply_down(struct apply_handle *h, char *word);
FEXPORT char *apply_up(struct apply_handle *h, char *word);
FEXPORT int   apply_med(struct fsm *net, char *word);
FEXPORT char *apply_upper_words(struct apply_handle *h);
FEXPORT char *apply_lower_words(struct apply_handle *h);
FEXPORT char *apply_words(struct apply_handle *h);
FEXPORT char *apply_random_lower(struct apply_handle *h);
FEXPORT char *apply_random_upper(struct apply_handle *h);
FEXPORT char *apply_random_words(struct apply_handle *h);
/* Reset the iterator to start anew with enumerating functions */
FEXPORT void apply_reset_enumerator(struct apply_handle *h);
/* Minimum edit distance & spelling correction */
FEXPORT void fsm_create_letter_lookup(struct fsm *net);
FEXPORT void cmatrix_init(struct fsm *net);
FEXPORT void cmatrix_default_substitute(struct fsm *net, int cost);
FEXPORT void cmatrix_default_insert(struct fsm *net, int cost);
FEXPORT void cmatrix_default_delete(struct fsm *net, int cost);
FEXPORT void cmatrix_set_cost(struct fsm *net, char *in, char *out, int cost);
FEXPORT void cmatrix_print(struct fsm *net);
FEXPORT void cmatrix_print_att(struct fsm *net, FILE *outfile);

/* Lexc */
FEXPORT struct fsm *fsm_lexc_parse_file(char *myfile);
FEXPORT struct fsm *fsm_lexc_parse_string(char *mystring);

/*************************/
/* Construction routines */
/*************************/

FEXPORT struct fsm_construct_handle *fsm_construct_init(char *name);
FEXPORT void fsm_construct_set_final(struct fsm_construct_handle *handle, int state_no);
FEXPORT void fsm_construct_set_initial(struct fsm_construct_handle *handle, int state_no);
FEXPORT void fsm_construct_add_arc(struct fsm_construct_handle *handle, int source, int target, char *in, char *out);
FEXPORT void fsm_construct_add_arc_nums(struct fsm_construct_handle *handle, int source, int target, int in, int out);
FEXPORT int fsm_construct_add_symbol(struct fsm_construct_handle *handle, char *symbol);
FEXPORT int fsm_construct_check_symbol(struct fsm_construct_handle *handle, char *symbol);
FEXPORT void fsm_construct_copy_sigma(struct fsm_construct_handle *handle, struct sigma *sigma);
FEXPORT struct fsm *fsm_construct_done(struct fsm_construct_handle *handle);


/******************/
/* String hashing */
/******************/

struct sh_handle {
    struct sh_hashtable *hash;
};

struct sh_hashtable {
    char *string;
    struct sh_hashtable *next;
};

struct sh_handle *sh_init();
void sh_done(struct sh_handle *sh);
char *sh_find_string(struct sh_handle *sh, char *string);
char *sh_find_add_string(struct sh_handle *sh, char *string);
char *sh_add_string(struct sh_handle *sh, char *string);

/*********************/
/* Trie construction */
/*********************/

struct trie_hash {
    char *insym;
    char *outsym;
    unsigned int sourcestate;
    unsigned int targetstate;
    struct trie_hash *next;
};

struct trie_states {
    _Bool is_final;
};

struct fsm_trie_handle {
    struct trie_states *trie_states;
    unsigned int trie_cursor;
    struct trie_hash *trie_hash;
    unsigned int used_states;
    unsigned int statesize;
    struct sh_handle *sh_hash;
};

FEXPORT struct fsm_trie_handle *fsm_trie_init();
FEXPORT struct fsm *fsm_trie_done(struct fsm_trie_handle *th);
FEXPORT void fsm_trie_new_word(struct fsm_trie_handle *th);
FEXPORT void fsm_trie_add_word(struct fsm_trie_handle *th, char *word);
FEXPORT void fsm_trie_end_word(struct fsm_trie_handle *th);
FEXPORT void fsm_trie_symbol(struct fsm_trie_handle *th, char *insym, char *outsym);

/***********************/
/* Extraction routines */
/***********************/

struct fsm_read_handle {
    struct fsm_state *arcs_head;
    struct fsm_state *arcs_cursor;
    int *finals_head;
    int *finals_cursor;
    int *states_head;
    int *states_cursor;
    int *initials_head;
    int *initials_cursor;
    struct fsm_sigma_list *fsm_sigma_list;
    int sigma_list_size;
};


FEXPORT struct fsm_read_handle *fsm_read_init(struct fsm *net);
FEXPORT void fsm_read_reset(struct fsm_read_handle *handle);
/* Move iterator one arc forward. Returns 0 on no more arcs */
FEXPORT int fsm_get_next_arc(struct fsm_read_handle *handle);
FEXPORT int fsm_get_arc_source(struct fsm_read_handle *handle);
FEXPORT int fsm_get_arc_target(struct fsm_read_handle *handle);
FEXPORT char *fsm_get_arc_in(struct fsm_read_handle *handle);
FEXPORT char *fsm_get_arc_out(struct fsm_read_handle *handle);
FEXPORT int fsm_get_arc_num_in(struct fsm_read_handle *handle);
FEXPORT int fsm_get_arc_num_out(struct fsm_read_handle *handle);
FEXPORT int fsm_get_symbol_number(struct fsm_read_handle *handle, char *symbol);

/* Iterates over initial and final states, returns -1 on end */
FEXPORT int fsm_get_next_initial(struct fsm_read_handle *handle);
FEXPORT int fsm_get_next_final(struct fsm_read_handle *handle);
FEXPORT int fsm_get_next_state(struct fsm_read_handle *handle);
/* Frees memory associated with a read handle */
FEXPORT void fsm_read_done(struct fsm_read_handle *handle);

#ifdef  __cplusplus
}
#endif
