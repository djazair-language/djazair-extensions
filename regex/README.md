# regex Library

POSIX regular expression engine for Djazair. Provides `Pattern` (compiled regex), `Match` (result), and convenience functions for search, findall, substitution, and splitting. Backed by the TRE library (POSIX Extended Regular Expressions).

## Quick Start

```dz
use regex

let m = regex.search("(\\w+)@(\\w+)", "user@host.com")
print(m.group(0))  # "user@host"
print(m.group(1))  # "user"
print(m.group(2))  # "host"

let swapped = regex.sub("(\\w+) (\\w+)", "$2 $1", "hello world")
print(swapped)  # "world hello"

let words = regex.split("\\s+", "one  two   three")
print(words)  # ["one", "two", "three"]
```

## Flag Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `I` / `IGNORECASE` | 1 | Case-insensitive matching |
| `M` / `MULTILINE` | 2 | `^` and `$` match at line boundaries |

```dz
use regex

let pat = regex.compile("hello", regex.I)
pat.search("HELLO")  # Match found

let pat2 = regex.compile("^world", regex.MULTILINE)
pat2.search("hello\nworld")  # Match found (^ matches after \n)
```

## API Reference

### Convenience Functions

These accept either a pattern string or a compiled `Pattern` object as the first argument.

| Function | Returns | Description |
|----------|---------|-------------|
| `regex.compile(pattern, flags?)` | `Pattern` | Compile a pattern with optional flags |
| `regex.fullMatch(pattern, text)` | `Match` or `Null` | Match the entire string |
| `regex.search(pattern, text)` | `Match` or `Null` | Find first match anywhere |
| `regex.findAll(pattern, text)` | `Array` | Find all non-overlapping matches |
| `regex.sub(pattern, replacement, text)` | `String` | Replace all matches |
| `regex.split(pattern, text)` | `Array` | Split text around matches |

```dz
use regex

# String pattern (auto-compiled)
regex.search("\\d+", "abc 42 def")   # Match: "42"
regex.findAll("\\w+", "a b c")       # ["a", "b", "c"]

# Compiled pattern (reusable, faster for repeated use)
let pat = regex.compile("\\d+")
regex.search(pat, "abc 42")           # Match: "42"
regex.sub(pat, "#", "a1b2c3")         # "a#b#c#"
```

### Pattern Class

A compiled regular expression ready for matching.

| Method | Returns | Description |
|--------|---------|-------------|
| `new Pattern(pattern, flags?)` | Pattern | Compile with optional flags |
| `pat.fullMatch(text)` | `Match` or `Null` | Match entire string |
| `pat.search(text)` | `Match` or `Null` | Find first match |
| `pat.findAll(text)` | `Array` | Find all matches |
| `pat.sub(replacement, text)` | `String` | Replace all matches |
| `pat.split(text)` | `Array` | Split around matches |

```dz
use regex

let pat = regex.compile("(\\w+) (\\w+)", regex.IGNORECASE)

pat.fullMatch("Hello World")  # Match: "Hello World"
pat.search("say hello world") # Match: "hello world"

pat.findAll("hi bye cya")     # [["hi"], ["bye"], ["cya"]]
pat.sub("$2 $1", "hello world")  # "world hello"

pat.split("one two three")    # ["one", "two", "three"]
```

### Match Class

Represents a single regex match with access to captured groups and positional info.

| Method | Returns | Description |
|--------|---------|-------------|
| `m.group(n?)` | `String` or `Null` | Substring for group `n` (default 0) |
| `m.groups()` | `Array` | All captured groups (excluding group 0) |
| `m.start(n?)` | `Number` | Start position of group `n` (default 0) |
| `m.endPos(n?)` | `Number` | End position of group `n` (default 0) |
| `m.span(n?)` | `Array` | `[start, endPos]` of group `n` |
| `m.pos()` | `Number` | Start of entire match |
| `m.toString()` | `String` | String representation |

```dz
use regex

let m = regex.search("(\\w+)@(\\w+)", "email: user@host.com")
m.group(0)     # "user@host"
m.group(1)     # "user"
m.group(2)     # "host"
m.start(0)     # 7  (position in original string)
m.endPos(0)    # 16
m.span(0)      # [7, 16]
m.pos()        # 7
m.toString()   # "new Match(user@host)"

# Groups that didn't participate return Null
let m2 = regex.search("(a)?(b)", "xb")
m2.group(1)    # Null (optional group didn't match)
m2.group(2)    # "b"
```

### Backreferences in `sub`

Replacement strings support `$1`, `$2`, etc. to reference captured groups, and `$$` for a literal `$`.

```dz
use regex

regex.sub("(\\w+) (\\w+)", "$2 $1", "hello world")
# "world hello"

regex.sub("(\\d+)", "$1$1", "a3b5")
# "a33b55"

regex.sub("x", "$$1", "x y x")
# "$1 y $1"
```

### Split Behavior

`split` produces a trailing empty string when the text ends with a match (standard behavior).

```dz
use regex

regex.split(",", "a,b,c")    # ["a", "b", "c"]
regex.split(",", "a,b,c,")   # ["a", "b", "c", ""]
regex.split(",", ",a,b")     # ["", "a", "b"]
regex.split(",", ",,,")      # ["", "", "", ""]
```

## Notes

- Uses POSIX Extended Regular Expressions (TRE library)
- `REG_NEWLINE` is used for MULTILINE mode (`^`/`$` match line boundaries)
- Case-insensitive matching uses `REG_ICASE`
- UTF-8 text is supported (byte indices are converted to character offsets)
- Compiled patterns are automatically freed by the garbage collector
