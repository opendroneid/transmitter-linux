
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#include "print_bt_features.h"

struct bitfield_data {
    uint64_t bit;
    const char *str;
};

static const struct bitfield_data features_le[] = {
        {  0, "LE Encryption"					},
        {  1, "Connection Parameter Request Procedure"		},
        {  2, "Extended Reject Indication"			},
        {  3, "Slave-initiated Features Exchange"		},
        {  4, "LE Ping"						},
        {  5, "LE Data Packet Length Extension"			},
        {  6, "LL Privacy"					},
        {  7, "Extended Scanner Filter Policies"		},
        {  8, "LE 2M PHY"					},
        {  9, "Stable Modulation Index - Transmitter"		},
        { 10, "Stable Modulation Index - Receiver"		},
        { 11, "LE Coded PHY"					},
        { 12, "LE Extended Advertising"				},
        { 13, "LE Periodic Advertising"				},
        { 14, "Channel Selection Algorithm #2"			},
        { 15, "LE Power Class 1"				},
        { 16, "Minimum Number of Used Channels Procedure"	},
        { 17, "Connection CTE Request"				},
        { 18, "Connection CTE Response"				},
        { 19, "Connectionless CTE Transmitter"			},
        { 20, "Connectionless CTE Receiver"			},
        { 21, "Antenna Switching During CTE Transmission (AoD)"	},
        { 22, "Antenna Switching During CTE Reception (AoA)"	},
        { 23, "Receiving Constant Tone Extensions"		},
        { 24, "Periodic Advertising Sync Transfer - Sender"	},
        { 25, "Periodic Advertising Sync Transfer - Recipient"	},
        { 26, "Sleep Clock Accuracy Updates"			},
        { 27, "Remote Public Key Validation"			},
        { 28, "Connected Isochronous Stream - Master"		},
        { 29, "Connected Isochronous Stream - Slave"		},
        { 30, "Isochronous Broadcaster"				},
        { 31, "Synchronized Receiver"				},
        { 32, "Isochronous Channels (Host Support)"		},
        { }
};

#define COLOR_OFF	"\x1B[0m"
#define COLOR_WHITE_BG	"\x1B[0;47;30m"
#define COLOR_UNKNOWN_FEATURE_BIT	COLOR_WHITE_BG

#define print_text(color, fmt, args...) \
		print_indent(8, COLOR_OFF, "", "", color, fmt, ## args)

#define print_field(fmt, args...) \
		print_indent(8, COLOR_OFF, "", "", COLOR_OFF, fmt, ## args)

static pid_t pager_pid = 0;

bool use_color(void)
{
    static int cached_use_color = -1;

    if (__builtin_expect(!!(cached_use_color < 0), 0))
        cached_use_color = isatty(STDOUT_FILENO) > 0 || pager_pid > 0;

    return cached_use_color;
}

#define print_indent(indent, color1, prefix, title, color2, fmt, args...) \
do { \
	printf("%*c%s%s%s%s" fmt "%s\n", (indent), ' ', \
		use_color() ? (color1) : "", prefix, title, \
		use_color() ? (color2) : "", ## args, \
		use_color() ? COLOR_OFF : ""); \
} while (0)

static inline uint64_t print_bitfield(int indent, uint64_t val, const struct bitfield_data *table)
{
    uint64_t mask = val;
    for (int i = 0; table[i].str; i++) {
        if (val & (((uint64_t) 1) << table[i].bit)) {
            print_field("%*c%s", indent, ' ', table[i].str);
            mask &= ~(((uint64_t) 1) << table[i].bit);
        }
    }
    return mask;
}

void print_bt_le_features(const uint8_t *features_array)
{
    uint64_t mask, features = 0;
    char str[41];

    for (int i = 0; i < 8; i++) {
        sprintf(str + (i * 5), " 0x%2.2x", features_array[i]);
        features |= ((uint64_t) features_array[i]) << (i * 8);
    }
    print_field("Features:%s", str);

    mask = print_bitfield(2, features, features_le);
    if (mask)
        print_text(COLOR_UNKNOWN_FEATURE_BIT, "  Unknown features (0x%16.16" PRIx64 ")", mask);
}
