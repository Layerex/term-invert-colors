#include <ansiescape.hh>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static bool invert_standart_background = false, invert_standart_foreground = false,
            invert_bright_background = false, invert_bright_foreground = false,
            invert_216_background = false, invert_216_foreground = false,
            invert_grayscale_background = false, invert_grayscale_foreground = false,
            invert_rgb_background = false, invert_rgb_foreground = false;

static const char usage[] =
        "Usage: %s [-bg -fg -8 -8bg -8fg -b8 -b8bg -b8fg -16 -16bg -16fg -216 -216bg -216fg "
        "-grayscale -grayscalebg -grayscalefg -256 -256bg -256fg -rgb -rgbbg -rgbfg]\n"
        "Flags specify which colors to invert. "
        "If no flags specified, all colors all inverted.\n"
        "\"bg\" and \"fg\" flag suffixes specify whether to invert only background or only "
        "foreground colors respectively. "
        "Flags without such suffixes invert both background and foreground colors.\n"
        "-fg        All foreground colors.\n"
        "-bg        All background colors.\n"
        "-8         8 standart colors.\n"
        "-b8        8 bright colors.\n"
        "-16        All colors of 16-color palette. Equal to -8 -b8.\n"
        "-216       216 colors of color cube of 256-color palette.\n"
        "-grayscale 24 shades of gray of 256-color palette.\n"
        "-256       All colors of 256-color palette. Equal to -16 -216 -grayscale.\n"
        "-rgb       All rgb colors.\n"
        "Take a look at https://en.wikipedia.org/wiki/ANSI_escape_code if in doubt.\n";

inline int invert_color(int color, int palette_lower_bound, int palette_upper_bound)
{
    return palette_upper_bound - color + palette_lower_bound;
}

// assign color an inverted color
#define INVERT(color, palette_lower_bound, palette_upper_bound)                                    \
    color = invert_color(color, palette_lower_bound, palette_upper_bound);

// invert color if needed and color is in palette, return from function on success
#define INVERT_IF_NEEDED(predicative, color, palette_lower_bound, palette_upper_bound)             \
    if (predicative && color >= palette_lower_bound && color <= palette_upper_bound) {             \
        INVERT(color, palette_lower_bound, palette_upper_bound)                                    \
        return;                                                                                    \
    }

void invert_color_sequence(ansi_sequence *sequence)
{
    // if not a SGR, we have nothing to do here
    if (sequence->mode == 'm') {
        switch (sequence->values->size()) {
        case 1:
            // 16 color palette
            INVERT_IF_NEEDED(invert_standart_background, sequence->values->at(0), 40, 47)
            INVERT_IF_NEEDED(invert_standart_foreground, sequence->values->at(0), 30, 37)
            INVERT_IF_NEEDED(invert_bright_background, sequence->values->at(0), 100, 107)
            INVERT_IF_NEEDED(invert_bright_foreground, sequence->values->at(0), 90, 97)
            break;
        case 3:
            // 256 color palette
            // check if escape code is really setting an 8-bit color
            if (sequence->values->at(1) == 5) {
                if (sequence->values->at(0) == 48) {
                    INVERT_IF_NEEDED(invert_standart_background, sequence->values->at(2), 0, 7)
                    INVERT_IF_NEEDED(invert_bright_background, sequence->values->at(2), 8, 15)
                    INVERT_IF_NEEDED(invert_216_background, sequence->values->at(2), 16, 231)
                    INVERT_IF_NEEDED(invert_grayscale_background, sequence->values->at(2), 232, 255)
                } else if (sequence->values->at(0) == 38) {
                    INVERT_IF_NEEDED(invert_standart_foreground, sequence->values->at(2), 0, 7)
                    INVERT_IF_NEEDED(invert_bright_foreground, sequence->values->at(2), 8, 15)
                    INVERT_IF_NEEDED(invert_216_foreground, sequence->values->at(2), 16, 231)
                    INVERT_IF_NEEDED(invert_grayscale_foreground, sequence->values->at(2), 232, 255)
                }
            }
            break;
        case 5:
            // rgb
            // check if escape code is really setting a rgb color
            if (sequence->values->at(1) == 2) {
                if ((sequence->values->at(0) == 48 && invert_rgb_background)
                    || (sequence->values->at(0) == 38 && invert_rgb_foreground)) {
                    INVERT(sequence->values->at(2), 0, 256)
                    INVERT(sequence->values->at(3), 0, 256)
                    INVERT(sequence->values->at(4), 0, 256)
                }
            }
            break;
        }
    }
}

inline void reprint_integer(int n)
{
    // see https://stackoverflow.com/questions/18006748/using-putchar-unlocked-for-fast-output
    if (n == 0) {
        putchar_unlocked('0');
    } else if (n > 0) {
        int reverse = n, zeroes_count = 0;
        while ((reverse % 10) == 0) {
            ++zeroes_count;
            reverse /= 10;
        }
        reverse = 0;
        while (n != 0) {
            reverse = (reverse << 3) + (reverse << 1) + n % 10;
            n /= 10;
        }
        while (reverse != 0) {
            putchar_unlocked(reverse % 10 + '0');
            reverse /= 10;
        }
        for (; zeroes_count > 0; --zeroes_count) {
            putchar_unlocked('0');
        }
    }
    // CSI_EMPTY_VALUE is -1; negative numbers are not supported in escape codes,
    // so we don't have to do anything if passed number is negative
}

void reprint_sequence(ansi_sequence *sequence)
{
    putchar_unlocked('[');
    if (sequence->priv)
        putchar_unlocked(sequence->priv);
    reprint_integer(sequence->values->at(0));
    for (size_t i = 1; i < sequence->values->size(); ++i) {
        putchar_unlocked(';');
        reprint_integer(sequence->values->at(i));
    }
    putchar_unlocked(sequence->mode);
}

int main(int argc, char *argv[])
{
    // if no args, then invert everything
    if (argc <= 1) {
        invert_standart_background = true;
        invert_standart_foreground = true;
        invert_bright_background = true;
        invert_bright_foreground = true;
        invert_216_background = true;
        invert_216_foreground = true;
        invert_grayscale_background = true;
        invert_grayscale_foreground = true;
        invert_rgb_background = true;
        invert_rgb_foreground = true;
    } else {
        for (int i = 1; i < argc; ++i) {
            if (!strcmp(argv[i], "-bg")) {
                invert_standart_background = true;
                invert_bright_background = true;
                invert_216_background = true;
                invert_grayscale_background = true;
                invert_rgb_background = true;
            } else if (!strcmp(argv[i], "-fg")) {
                invert_standart_foreground = true;
                invert_bright_foreground = true;
                invert_216_foreground = true;
                invert_grayscale_foreground = true;
                invert_rgb_foreground = true;
            } else if (!strcmp(argv[i], "-8")) {
                invert_standart_background = true;
                invert_standart_foreground = true;
            } else if (!strcmp(argv[i], "-8bg")) {
                invert_standart_background = true;
            } else if (!strcmp(argv[i], "-8fg")) {
                invert_standart_foreground = true;
            } else if (!strcmp(argv[i], "-b8")) {
                invert_bright_background = true;
                invert_bright_foreground = true;
            } else if (!strcmp(argv[i], "-b8bg")) {
                invert_bright_background = true;
            } else if (!strcmp(argv[i], "-b8fg")) {
                invert_bright_foreground = true;
            } else if (!strcmp(argv[i], "-16")) {
                invert_standart_background = true;
                invert_standart_foreground = true;
                invert_bright_background = true;
                invert_bright_foreground = true;
            } else if (!strcmp(argv[i], "-16bg")) {
                invert_standart_background = true;
                invert_bright_background = true;
            } else if (!strcmp(argv[i], "-16fg")) {
                invert_standart_foreground = true;
                invert_bright_foreground = true;
            } else if (!strcmp(argv[i], "-216")) {
                invert_216_background = true;
                invert_216_foreground = true;
            } else if (!strcmp(argv[i], "-216bg")) {
                invert_216_background = true;
            } else if (!strcmp(argv[i], "-216fg")) {
                invert_216_foreground = true;
            } else if (!strcmp(argv[i], "-256")) {
                invert_standart_background = true;
                invert_standart_foreground = true;
                invert_bright_background = true;
                invert_bright_foreground = true;
                invert_grayscale_background = true;
                invert_grayscale_foreground = true;
                invert_216_background = true;
                invert_216_foreground = true;
            } else if (!strcmp(argv[i], "-256bg")) {
                invert_standart_background = true;
                invert_bright_background = true;
                invert_grayscale_background = true;
                invert_216_background = true;
            } else if (!strcmp(argv[i], "-256fg")) {
                invert_standart_foreground = true;
                invert_bright_foreground = true;
                invert_grayscale_foreground = true;
                invert_216_foreground = true;
            } else if (!strcmp(argv[i], "-grayscale")) {
                invert_grayscale_background = true;
                invert_grayscale_foreground = true;
            } else if (!strcmp(argv[i], "-grayscalebg")) {
                invert_grayscale_background = true;
            } else if (!strcmp(argv[i], "-grayscalefg")) {
                invert_grayscale_foreground = true;
            } else if (!strcmp(argv[i], "-rgb")) {
                invert_rgb_background = true;
                invert_rgb_foreground = true;
            } else if (!strcmp(argv[i], "-rgbbg")) {
                invert_rgb_background = true;
            } else if (!strcmp(argv[i], "-rgbfg")) {
                invert_rgb_foreground = true;
            } else {
                fprintf(stderr, usage, argv[0]);
                return EXIT_FAILURE;
            }
        }
    }

    if (setlocale(LC_ALL, "") == NULL) {
        fprintf(stderr, "Failed to set locale.");
    }

    ansi_escape_parser_reset();
    char c;
    // thread unsafe getchar_unlocked() is used since program operates in a single thread
    // same with putchar_unlocked()
    while ((c = getchar_unlocked()) != EOF) {
        // since library does not support parsing non-CSI escape codes, here is a workaround to
        // to correctly reprint escape codes library can't parse
        // ideally, this should be reprinted in reprint_sequence function
        if (c == 0x1B) {
            putchar_unlocked(0x1B);
        }
        switch (ansi_escape_parser_feed(c)) {
        case ANSI_ESCAPE_PARSE_OK:
            invert_color_sequence(ansi_parser_last_parsed);
            reprint_sequence(ansi_parser_last_parsed);
            break;
        case ANSI_ESCAPE_PARSE_BAD:
            // not an escape sequence, just reprint character
            putchar_unlocked(c);
            ansi_escape_parser_reset();
            break;
        case ANSI_ESCAPE_PARSE_INCOMPLETE:
            // do nothing, wait till sequence gets parsed
            break;
        }
    }
    return EXIT_SUCCESS;
}
