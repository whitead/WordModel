/*
 * After some thought, I think the best thing to do is not define a
 * common tokenizer, since different word models may have different
 * needs. I should only define a set of default delimiters in a
 * header. 
 *
 */

#include <boost/tokenizer.hpp>
#define TOKENIZER_DELIMS "\t ",";:,.?!\""


