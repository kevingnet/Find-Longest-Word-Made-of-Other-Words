# Find-Longest-Word-Made-of-Other-Words

The program reads a file containing a sorted list of words (one word per line, no spaces, all lower case), then identifies 
the longest word in the file that can be constructed by concatenating copies of shorter words also found in the file. 
The program should then go on to report the second longest word found as well as how many of the words in the list can be constructed of other words in the list.
Please reply to this email with your solution as source code along with the (2) longest words that the program finds and the count of words that can be constructed as an output in the body of the email and any comments you have on the approach you took.
For example, if the file contained:

       cat
       cats
       catsdogcats
       catxdogcatsrat
       dog
       dogcatsdog
       hippopotamuses
       rat
       ratcatdogcat

The longest word would be 'ratcatdogcat' - at 12 letters, it is the longest word made up of other words in the list.  

Please send your solution in source code form, written in C or C++.   This is not just a puzzle or classroom assignment; 
it is your opportunity to demonstrate your engineering judgment in a way that you cannot do in a personal interview.  
Performance matters: the program should return results quickly even for very large lists (100,000+ items).  

Please find attached a file, words for problem.txt, containing a word list, with 173k rows, for testing purposes.

sorted list of words (no spaces, lower case)
longest word constructed by concatenating copies of shorter words
NOTE: shorter, not shortest

The program should then go on to report the second longest word found
as well as how many of the words in the list can be constructed of other words in the list.
