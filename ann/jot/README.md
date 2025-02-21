## Project Jot

Jot, my note-taking markup language, is simple, intuitive, and highly efficient.
It offers a clean and attractive experience in Vim and produces polished outputs
in both HTML and PDF formats.

### Markups

Paragraph:
```
\p{
  This is a paragraph.
}
```

Code
```
\p{
  This is a \c{let a = b + c}.
}
```

URL
```
\p{
  This is a \url[tgt=http:google.com]]{url}.
}
```

Quote block
```
\p{
  This is a \q{
    Kind of the world.
  }.
}
```
