# osort
Opition sorting utility.

![Latest build](https://github.com/mgeorgoulopoulos/osort/workflows/Auto%20build/badge.svg)

* Write a list of things in a text file.
* Pass the text file to osort.exe
* Answer the questions.
* A file named YourFile.txt.sorted.txt will appear with the items of your input file sorted based on answers given.

# To be more specific
This utility will rank a list of items from best to worst according to the answers you provide. The items may be anything you like: movie titles, band names and what not. I have included a sample input file with my personal top 5 relatively unknown anime.

After populating the input file with your list, you run osort.exe and it asks you a number of questions. The tool will do its best to ask you as few questions as possible, while keeping it interesting.
See technical notes below for a definition of "interesting".

The result (sorted list) will be written to a file having the same name as your input file and the extension .sorted.txt.

# Technical notes
I started this project in order to save some time (yes, I know...) while having such a list to sort.

## Someone else has coded it for me
My first instinct was to somehow take advantage of std::sort, and provide it with a custom comparison function that actually asks the user. This way, I could enjoy std::sort's inherent optimization of comparison count.

## But...
It turns out that std::sort actually calls the comparison function twice for each pair. Reminder: the comparison function implements '<'. So, to test for equality, std::sort calls twice for "a less than b" and for "b less than a".

To prevent that, I implemented a small cache with the already answered questions.

## Getting greedy
But then it occured to me that I could optimize this even more: why not keep in memory both the user's answers and what we can deduce from them? For instance, if a is greater than b and b is greater than c, we can safely assume that a is greater than C. That would save some more questions.

In practice though, std::sort is almost unbeatable. At least Visual Studio's implementation which I use. It does no redundant comparisons.

## On the other hand...
The problem with std::sort though, is that it asks you very repetitive questions: it laches on to "pivot" elements in your list (say "Dragon Ball") and then it compares the pivot against all other items.

Apart from being boring, this distributes your attention to the list's elements unevenly.

## Improvements
So I added a preprocess step. My thinking was that you at least need to ask N-1 questions (to verify an already sorted list). So why not ask these few questions in advance, cache the result and let std::sort handle the rest of the questions?

And so I did.

But which n-1 questions to ask?

It took me a few experiments, but I think I found the best solution: sort the items in ascending order of "how many things we know about each". Then ask the question to compare the first and second elements of the list.

## Boring!
Doing this, I actually beat std::sort by 1-2 questions, depending on the ordering. And I was happy about it.
Until I noticed, that the majority of the questions are asked by std::sort after my preprocessing step (of course, it's n*logn questions), and are of the boring kind.

On the other hand, the preprocessing questions were better. Comparing the two items that we hadn't bothered with earlier was refreshing and also provided the kind of "surprise factor" required to get an honest opinion. You get a good sense of your built-in biases when you have to compare "Dragon Ball" with every other anime. Suddently "Dragon Ball" becomes special.

Not that it isn't.

But, if I am to ask some more of the interesting questions, exactly how many?

I decided to ask them all.

The result is in the order of n*logn questions. So I left it at that. It is far more enjoyable (and productive) than going for the absolute minimum, but biased.

## Finishing touches
It can be really painful when you happen to answer 100 questions and one of them wrong. To prevent the user from starting all over, I added a "cache.txt" file which saves the answers, so you can kill the program and continue from where you left off. Also this way, you can actually edit the initial list (add/remove items), re-run the program and all the already answered questions will be skipped.

# Have fun
I'm putting this on Github in the hopes that someone can use it and have fun with it.

If nothing else, you could check out my list of obscure anime in TestList.txt. They are awesome!
