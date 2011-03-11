fuzzydups
=========

Fuzzydups is a suite of programs for finding near-duplicate strings in
large lists. Its goals are to be efficient and to provide a flexible
Unix-style command-line interface.

Applications
------------

I use fuzzydups to help manage tags and playlists in my music
collection. Some of the tasks it helps with are:

  - looking for near-duplicates in tags; this can help find accidental
    duplicates in the collection (even if they are not byte-for-byte the
    same), but can also help correct typos when there are multiple
    versions of the same tag (for example, you have many tracks with the
    same artist, but have made a typo in some of the tags).

  - I keep m3u-style text playlists that refer to tracks by their
    filename, which contain the artist and title in the filename.
    Occasionally I correct typos or other information in the artist or
    title, and the filename is updated to reflect the fix. This makes
    the playlist stale. Fuzzydups can efficiently find the most likely
    candidate for such a track.

Examples
--------

These examples will illustrate more explicitly the applications
mentioned above. See also the `fuzzydups` and `fuzzymunge` manpages in
the `Documentation/` directory for more examples.

Imagine you have a large collection of music tracks, and a program to
print some particular tag. You could then do something like:

    find /path/to/music -name '*.flac' |
    xargs print_tag artist |
    sort -u |
    fuzzydups

to get a list of near-duplicate pairs.

For freshening stale playlists, you would do something like:

    find /path/to/music -name '*.flac' >tracks
    fuzzymunge -b tracks -i playlist1 playlist2 playlist3

which will modify in-place the three playlists, correcting any line in
each playlist that does not have a matching entry in the `tracks` file
to its nearest match.

Algorithm
---------

One naïve way to do near-duplicate detection is to calculate the
Levenshtein edit distance between each input record and each possible
match; sort the distances and return the best match for each input
record. When finding duplicates within a set of N records, this ends up
being `O(n^2)` Levenshtein calculations, each of which is somewhat
expensive.

Fuzzydups takes a different approach.  The core of the algorithm is a
trie data structure; this structure breaks strings down into a sequence
of character steps, with similar prefixes sharing nodes. For example, a
trie containing "ab", "abc", and "abd" would look like:

                       a, terminal=false ("a" does not exist)
                       |
                       b, terminal=true ("ab" exists)
                      / \
                     c   d
         terminal=true   terminal=true
       ("abc" exists")   ("abd" exists)

We build the trie either from a set of "base" items, or incrementally
from the input list.  For each input line, we do a budgeted walk of the
trie, spending zero "points" for an exact match, or one point for a
single-character edit (i.e., a substitution, deletion, or addition).
This has two advantages over the naïve solution:

  1. Some of the edit-distance calculations are shared for multiple
     records. That is, if we have three records "foo1", "foo2", and
     "foo3", we traverse the "foo" part of the trie only one time to
     match all three records.

  2. We can bound the walk dynamically. If we run out of budget, we
     know there is no point in continuing our walk and can return early.
     This avoids calculating a very high edit-distance that will not be
     of use to us. Similarly, if we have already found a match with
     budget `N`, we can stop walking other parts of the trie once our
     budget exceeds `N`.

Building
--------

To build from the git repository, you will need the `mfm` tool. You can
get it here: <http://github.com/peff/mfm>

Then run:

    mfm
    make
    make install

Asciidoc documentation is in the `Documentation/` directory, and can be
read as-is or built into manpages with `make doc` from the top-level.

Contact
-------

Send questions or comments to peff@peff.net.
