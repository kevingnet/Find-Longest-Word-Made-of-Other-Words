/////////////////////////////////////////////////////////////////////////////////
//
// Author: KevinG
// classes: CompoundWordCounter
// main()
//////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sys/stat.h>
#include <boost/unordered_set.hpp>

using namespace std;

//------------------------------------------------------------------------------
//lots of trouble just to avoid branch, it improves about 2 tenths of a second (from .37 to .35 avg)
#ifdef CONFIG_64BIT
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* CONFIG_64BIT */
#define B1 (BITS_PER_LONG-1) // bits of signed int minus one
#define MINI(x,y) (((x) & (((int)((x)-(y)))>>B1)) + ((y) & ~(((int)((x)-(y)))>>B1)))

//------------------------------------------------------------------------------
//improves by almost twice as fast
typedef boost::unordered_set<string> sset;
typedef boost::unordered_set<string>::iterator itsset;
typedef vector<string> svec;
typedef vector<string>::const_iterator itsvec;

//------------------------------------------------------------------------------
// comparison for reverse order (words with more letters first)
struct compare {
  bool operator()(const string& first, const string& second) const {
    return first.size() > second.size();
  }
};

//------------------------------------------------------------------------------
//constants
const int MIN_START = 0xffff;
const size_t MAX_PRINT = 0x0;
const int PREALLOC_WORDS = 0x1000;
const int AVG_WORD_SIZE = 12;
const int DEFAULT_LINE_SIZE = 100;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// CompoundWordCounter
//////////////////////////////////////////////////////////////////////
//NOTE: in the tests the class actually improved overall performance, it improves IsCompound because there are less parameters to pass
class CompoundWordCounter {
public:
  CompoundWordCounter(int prealloc = PREALLOC_WORDS);

  //get some stats
  int WordCount() const {
    return m_Words.size();
  }
  int CompoundCount() const {
    return m_Compounds.size();
  }

  void CalcCount();
  void AddWord(const string& s);
  void Print(size_t max = MAX_PRINT);

private:
  //disable copy const. and assignment
  CompoundWordCounter(const CompoundWordCounter&) {
  }
  CompoundWordCounter& operator=(const CompoundWordCounter&) {
    return *this;
  }

  bool IsCompound(const string w) const;

  sset m_Words;
  svec m_Compounds;
  size_t m_Min;
  int m_PreallocWords;
};
//------------------------------------------------------------------------------

//--------------------------------------------------------------------
//constructor, preallocates memory for container
CompoundWordCounter::CompoundWordCounter(int prealloc) :
    m_Min(MIN_START), m_PreallocWords(prealloc) {
}

//--------------------------------------------------------------------
// output result up to max words, default all (0)
void CompoundWordCounter::Print(size_t max) {
  if (max == 0 || max > m_Compounds.size())
    max = m_Compounds.size();
  for (itsvec it = m_Compounds.begin(); it != m_Compounds.begin() + max; ++it)
    cout << *it << endl;
}

//--------------------------------------------------------------------
//add a word from the file to container, uses minimal optimization by eliminating branching
void CompoundWordCounter::AddWord(const string& s) {
  if (!s.size())
    return;
  m_Min = MINI(m_Min, s.size()); //save a bit of time, instead of...
  m_Words.insert(s);
}

//--------------------------------------------------------------------
//main operation, adds matching words to container
void CompoundWordCounter::CalcCount() {
  for (itsset its = m_Words.begin(); its != m_Words.end(); ++its) {
    if (IsCompound(*its))
      m_Compounds.push_back(*its);
  }
  compare c;
  sort(m_Compounds.begin(), m_Compounds.end(), c);
}

//--------------------------------------------------------------------
//determines if word is made up from smaller words found in container
//(dictionary file) 
/*
 - prev word can be larger, smaller, or equal
 - next larger word usually contains prev
 - next smaller word usually contains a portion of prev
 - find diff of the two words
 */
bool CompoundWordCounter::IsCompound(const string w) const {
  /*
   - whole word
   - substract m_Min
   - search in set
   -   a) not found: subtract one more letter, until found
   -   b) found: left over, recursive search
   */
  string r;
  r.reserve(w.size() - m_Min);
  r = w.substr(w.size() - m_Min, m_Min);
  string l = w.substr(0, w.size() - m_Min);
  while (l.size() >= m_Min) {
    if (m_Words.find(l) != m_Words.end()) {
      if (m_Words.find(r) != m_Words.end()) {
        return true;
      } else {
        if (IsCompound(r))
          return true;
      }
    }
    r.insert(r.begin(), l[l.size() - 1]);
    l.erase(l.end() - 1);
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// main()
//////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  struct stat filestatus;
  stat(argv[1], &filestatus);
  ifstream ifwords(argv[1]);
  if (!ifwords.is_open())
    return 1;
  //time operations
  const clock_t begin_time = clock();

  //declare counter class
  CompoundWordCounter cwc(static_cast<int>(filestatus.st_size / AVG_WORD_SIZE));
  string line;
  line.reserve(DEFAULT_LINE_SIZE);
  //read words from file and add to counter class
  while (getline(ifwords, line))
    cwc.AddWord(line);
  ifwords.close();

  //calculate results and output stats
  cwc.CalcCount();
  cout << "Time " << float(clock() - begin_time) / CLOCKS_PER_SEC << endl;
  cout << "Words " << cwc.WordCount() << " Compounds: " << cwc.CompoundCount()
      << endl;

  cwc.Print();

  return 0;
}
//--------------------------------------------------------------------

