// FSAE 2018
// August Trollback
//
// Interface for OLEDs driven by an US2066 chip.

#ifndef _NHD_US2066_H_
#define _NHD_US2066_H_

#define NHD_0420CW_NLINES 4
#define NHD_0420CW_NCOLS  20

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // need to be nlines * ncols long
    char buf[NHD_0420CW_NLINES * NHD_0420CW_NCOLS];
    bool lineupdates[NHD_0420CW_NLINES];

    uint8_t contrast;

    int line, col;
    int nlines, ncols;
} NHD_US2066_OLED;

// Initialize an oled struct for a screen with nlines and ncols
void oled_init(NHD_US2066_OLED *oled, int nlines, int ncols);

// Run the commands needed to initialize the oled for viewing
void oled_init_commands(NHD_US2066_OLED *oled);

// Set the position to print next
void oled_set_pos(NHD_US2066_OLED *oled, int line, int col);

// Print a null terminated string at the current position with wrapping
void oled_print_wrap(NHD_US2066_OLED *oled, char *str);

// Print a null terminated string at the current position (no wrapping)
void oled_print(NHD_US2066_OLED *oled, char *str);

// Print an integer number at the current position
void oled_print_num(NHD_US2066_OLED *oled, int num);

// Print a single character at the current position.
// Useful for printing special symbols defined in NHD_US2066_charset.h
void oled_print_char(NHD_US2066_OLED *oled, char chr);

// Clear the oled screen
void oled_clear(NHD_US2066_OLED *oled);

// Clear a single oled line
void oled_clearline(NHD_US2066_OLED *oled, int line);

// Display updates to the screen
void oled_update(NHD_US2066_OLED *oled);

#endif
