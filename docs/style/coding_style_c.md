# A. Coding style

## Naming convention

Capitalization styles:

1. `UPPERCASE_WITH_UNDERSCORE`
2. `lowercase_with_underscore`

**Use `UPPERCASE_WITH_UNDERSCORE` for constant, and `lowercase_with_underscore` for other names.**

A name cannot have the same spelling as a reserved word:

```C
  auto         break        case
  char         const        continue
  default      do           double
  else         enum         extern
  float        for          goto
  if           int          long
  while        return       short
  signed       sizeof       static
  struct       switch       register
  union        unsigned     void
  volatile     typedef
```

### Constant

```C
#define TRUE           (0)
#define FALSE          (1)
#define DEVICE_MEM_ID  (0xAB12)
#define MAGIC_NUMBER   (7)

const uint32_t THIS_IS_A_CONSTANT = 5;
```

### Enum

- Enum name is written in `lowercase_with_underscore`, following by `_enum`
- Enum's item is written in `UPPERCASE_WITH_UNDERSCORE`

```C
enum basic_color_enum
{
  BC_RED   = 0,
  BC_GREEN = 1,
  BC_BLUE  = 2
};
typedef enum basic_color_enum basic_color_t;
```

```C
typedef enum
{
  BC_RED   = 0,
  BC_GREEN = 1,
  BC_BLUE  = 2
}
basic_color_t;
```

### Struct

- Struct name is written in `lowercase_with_underscore`, following by `_struct`
- Struct's item is written in `lowercase_with_underscore`

```C
struct packet_header_struct
{
  uint32_t id;
  uint32_t type;
  uint32_t primary_key;
};
typedef struct packet_header_struct packet_header_t;
```

```C
typedef struct
{
  uint32_t id;
  uint32_t type;
  uint32_t primary_key;
}
packet_header_t;
```

### Global public variable

Global public variable must have prefix `g_`.

```C
extern uint32_t g_system_setting;
```

### Label

```C
__LBL__<LABEL_NAME_IN_UPPERCASE_WITH_UNDERSCORE>__
```

Example:

```c
goto __LBL_RECALCULATE__;
```

## Indentation

- Lines longer than **110 columns** should be avoided.
- **Two spaces** for each indentation should be used. Tabs are not allowed.

```C
if (parent)
{
  name = parent->get_name();
  age = parent->get_age();
}
```

- A switch statement should have its case keywords aligned with the switch keyword.

```C
switch (state)
{
case FSM_STATE_A:
  do_something_a();
  break;
case FSM_STATE_B:
  do_something_b();
  break;
default:
  break;
}
```

## White spaces

- Unary operators shall never be separated from their operands with space.

```C
!p   ~b     i++    j--    *p    &x

// Instead of

! p   ~ b   i ++   j --   * p   & x
```

- Binary operators except comma (`“,”`) **should** be written with a space between them and their operand.

```C
a + b = c + d;
a = (a + b) / (c * d);

// Instead of

a+b=c+d;
a=(a+b)/(c*d);
```

- Comma (`“,”`) shall be followed by one space if it is not the last character on a line.

```C
ret_value = function(int param1, int param2, int param3);
```

- Keywords (`if, while, for, switch`) shall be followed by a space and a left parenthesis.

```C
if (counter != 0)

while (forever == 1)

for (i = 0; i < 10; i++)

switch (state)
```

- Write `*` next to the pointer name without white space.

```C
int *file = NULL;

void fill_with_pattern(void *buffer, uint8_t pattern);

int time = *((int *)p_time);
```

## Parenthesis

Use redundant parentheses to make compound expressions clearer.

```C
if ((a < b) && (c > d))

// Instead of

if (a < b && c > d)
```

## Consistent line folding

When parameter lists (in function declarations, definitions, and calls) or enumerators in an enum eclaration
cannot fit on a single line, the line should be broken after each list element and each element should be placed on a separate line.

```C
void a_long_function_name(parameter_type_t *param1,
                          parameter_type_t *param2,
                          int param3);
```

When a conditional expression becomes complex and long, it should be broken up so that each expression is placed on a separate line.

```C
if ((evt.get_key_code() == EVENT_VK_ABORT_THERAPY) || // Comment 1
    (evt.get_key_code() == EVENT_VK_ICD_PC_SHOCK)  || // Comment 2
    (evt.get_key_code() == EVENT_VK_RECORDER))
```

When a text string cannot fit on a single line, it should be broken into smaller catenated strings, each placed on a separate line.

```C
printf("The command cannot be completed at this time,"
       " the sequence is out of order\n");
```

## Control structures

- Brackets are not required if all branches of the `if` structure contain only 1 statement.

```C
if (state != 0)
  while (__STREXH(__LDREXH(pData) | pin, pData)); // Set
else
  while (__STREXH(__LDREXH(pData) & (~pin), pData)); // Clear
```

- Brackets are not required if the body of `while, for` structures contain only 1 statement.

```C
while (counter > 0)
  counter--;
```

- Do not open/close the bracket on same line with `if, while, for, switch` if the structure cannot fit in a single line.

```C
if (a > b)
{
  min = b;
}

// Instead of

if (a > b) {
  min = b;
}
```

## Macro

Macro name could be written in `UPPERCASE_WITH_UNDERSCORE` (preferred) or `lowercase_with_underscore`.

```C
#define delay_ms(t) HAL_Delay(t) // Wraparround of STM32 HAL_Delay

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Structure padding
#define _addstr1(a,b,c) a ## b ## c
#define _addstr2(a,b,c) _addstr1(a,b,c)
#define _pad(n) char _addstr2(_____pad,__LINE__,_____)[n]

// Preprocessor assertion
// Example: example.h
//   typedef uint32_t time_t;
//   PRE_ASSERT(sizeof(time_t) == 4, time_t);
#define PRE_ASSERT(exp, msg) typedef char _addstr2(assert_,__LINE__,_##msg)[(exp) ? 1 : -1]
```

Always use structure `do {...} while (0)` for a multiple statements macro.

```C
#define RUN_ALL() \
  do              \
  {               \
    step_1();     \
    step_2();     \
    step_3();     \
  }               \
  while (0)
```

# B. File

## File name

File name and directory name uses `lowercase_with_underscore`. If a feature includes multiple source and/or header files, use abbreviation to start the file names or put them in a separated folder.

Example: `\doc\coding_style_c\code_template.c`

## Version scheme

Given a version number `major.minor.patch`, increment the:

1. `major` version when you make incompatible API changes,
2. `minor` version when you add functionality in a backwards-compatible manner, and
3. `patch` version when you make backwards-compatible bug fixes.

## Header file structure

`code_template.h`

## Source file structure

`code_template.c`
